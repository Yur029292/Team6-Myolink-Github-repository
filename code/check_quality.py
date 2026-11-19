"""
check_quality.py - applies the QUALITY-REPORT rejection rules to every raw file.

Put it in code/ and run it (Thonny: open and Run). It reads
    data/raw/*.csv        (never modified)
    data/metadata.csv     (only to know which recordings are train / test)
and writes
    data/quality_trials.csv   one row per trial: metrics, decision, reasons
It also prints a per-recording summary as a Markdown table for QUALITY-REPORT.md.

Only the Python standard library is used (no pandas / numpy needed).

All rules are technical and use the rest phase, the between phase or the whole
trial. No rest-versus-movement comparison is computed, so the same script can
run on the test session without looking at how well the classes separate.

Rules (a trial is rejected if any applies):
  R1 clipped   : a sample hit the ADC limit (clipped > 0 in the #TRIAL line)
  R2 incomplete: fewer samples than expected, or no #TRIAL line
  R3 stuck     : trial median below STUCK_LEVEL (amplifier stuck near ~3140)
  R4 noise     : rest RMS (after the first REST_TRIM_S) above NOISE_RMS_MAX
                 (strong 60 Hz interference, or activity during rest)
  R5 jump      : |median(rest) - median(between)| above JUMP_MAX
                 (baseline jump from lead / electrode movement)
Manual decisions taken by the team are listed in MANUAL_REJECT.
"""

import csv
import glob
import math
import os
import statistics
from pathlib import Path

# ----------------------------- rules (fixed before training) -----------------------------
STUCK_LEVEL = 5000      # ADC counts; normal baseline is ~7400-8300, stuck is ~3140
NOISE_RMS_MAX = 100     # ADC counts, rest RMS after trimming the first second
JUMP_MAX = 300          # ADC counts
REST_TRIM_S = 1.0       # seconds removed at the start of rest (release after pushing)

# Whole-recording decisions taken by the team, with the reason (file -> reason)
MANUAL_REJECT = {
    "S04_R02.csv": "team decision: whole recording rejected after the output got stuck in trial 2",
}
# -----------------------------------------------------------------------------------------

ROOT = Path(__file__).resolve().parent.parent
RAW = ROOT / "data" / "raw"
META = ROOT / "data" / "metadata.csv"
OUT = ROOT / "data" / "quality_trials.csv"


def rms(values):
    if not values:
        return float("nan")
    m = sum(values) / len(values)
    return math.sqrt(sum((v - m) ** 2 for v in values) / len(values))


def read_sets():
    sets = {}
    if META.exists():
        with open(META, newline="", encoding="utf-8-sig") as f:
            for row in csv.DictReader(f):
                sets[row["file"]] = row.get("set", "")
    return sets


def read_recording(path):
    header, trial_lines = {}, {}
    data = {}          # trial -> {phase: [values]}
    bad_lines = 0
    with open(path, encoding="utf-8", errors="ignore") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            if line.startswith("#"):
                parts = line[1:].split(",")
                if parts[0] == "TRIAL" and len(parts) >= 6:
                    trial_lines[int(parts[1])] = {"clipped": int(parts[3]), "late": int(parts[5])}
                elif len(parts) >= 2:
                    header[parts[0]] = parts[1]
                continue
            if line.startswith("idx"):
                continue
            parts = line.split(",")
            if len(parts) != 5:
                bad_lines += 1
                continue
            try:
                trial, phase, emg = int(parts[2]), int(parts[3]), int(parts[4])
            except ValueError:
                bad_lines += 1
                continue
            data.setdefault(trial, {}).setdefault(phase, []).append(emg)
    return header, trial_lines, data, bad_lines


def main():
    sets = read_sets()
    files = sorted(glob.glob(str(RAW / "*.csv")))
    if not files:
        raise SystemExit(f"No CSV files found in {RAW}")

    rows, summary = [], []
    for path in files:
        name = os.path.basename(path)
        header, trial_lines, data, bad_lines = read_recording(path)
        fs = float(header.get("fs_hz", 1000))
        n_trials = int(header.get("trials", 8))
        expected = round(fs * sum(float(header.get(k, 0)) for k in
                                  ("rest_s", "warning_s", "move_s", "between_s")))
        trim = int(REST_TRIM_S * fs)

        rejected, reasons_rec, lates = [], set(), []
        for t in range(1, n_trials + 1):
            phases = data.get(t, {})
            allv = [v for p in sorted(phases) for v in phases[p]]
            rest = phases.get(0, [])[trim:]
            between = phases.get(3, [])
            info = trial_lines.get(t)

            reasons = []
            if info and info["clipped"] > 0:
                reasons.append("R1 clipped")
            if info is None or len(allv) < expected:
                reasons.append("R2 incomplete")
            med = statistics.median(allv) if allv else float("nan")
            if allv and med < STUCK_LEVEL:
                reasons.append("R3 stuck")
            rest_rms = rms(rest)
            if rest and rest_rms > NOISE_RMS_MAX and "R3 stuck" not in reasons:
                reasons.append("R4 noise")
            jump = (abs(statistics.median(rest) - statistics.median(between))
                    if rest and between else float("nan"))
            if jump == jump and jump > JUMP_MAX and "R3 stuck" not in reasons:
                reasons.append("R5 jump")
            if name in MANUAL_REJECT:
                reasons.append("manual")

            decision = "reject" if reasons else "keep"
            if reasons:
                rejected.append(t)
                reasons_rec.update(r for r in reasons)
            if info:
                lates.append(info["late"])
            rows.append({
                "file": name, "session": header.get("session", ""),
                "recording": header.get("recording", ""), "set": sets.get(name, ""),
                "trial": t, "samples": len(allv), "expected": expected,
                "clipped": info["clipped"] if info else "", "late": info["late"] if info else "",
                "trial_median": round(med, 1) if allv else "",
                "rest_rms": round(rest_rms, 1) if rest else "",
                "baseline_jump": round(jump, 1) if jump == jump else "",
                "decision": decision, "reasons": "; ".join(reasons),
            })

        clipped_total = sum(v["clipped"] for v in trial_lines.values())
        late_txt = f"{min(lates)}-{max(lates)}" if lates else "n/a"
        rej_txt = ("all" if len(rejected) == n_trials else
                   ", ".join(map(str, rejected)) if rejected else "none")
        note = MANUAL_REJECT.get(name, "")
        if bad_lines:
            note = (note + "; " if note else "") + f"{bad_lines} corrupted lines"
        summary.append((name, sets.get(name, ""), clipped_total, late_txt, rej_txt,
                        ", ".join(sorted(reasons_rec)), note, n_trials - len(rejected)))

    with open(OUT, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
        w.writeheader()
        w.writerows(rows)

    print("| File | Set | clipped | late per trial | Rejected trials | Rules | Notes | Valid trials |")
    print("|---|---|---|---|---|---|---|---|")
    for s in summary:
        print("| " + " | ".join(str(x) for x in s) + " |")
    total = {}
    for s in summary:
        total[s[1]] = total.get(s[1], 0) + s[7]
    print("\nValid trials per set:", total)
    print(f"Per-trial details written to {OUT}")


if __name__ == "__main__":
    main()
