# Quality report

Participant P01, left biceps brachii, isometric elbow flexion pushing up against a table. All recordings on 2026-11-18, 1000 Hz, 14-bit ADC, 8 trials per recording (rest 5 s, warning 3 s, movement 5 s, between 1 s).

**Rejection criteria.** A trial is rejected if:
- any sample hits the limit of the ADC (`clipped` > 0);
- the circuit output is stuck (flat at ~3140 counts, no EMG signal);
- samples are missing (fewer than 14 000 per trial or corrupted lines);
- 60 Hz interference in rest is larger than the movement signal (≈ rest noise RMS > 100 counts);
- the baseline jumps (lead or electrode movement), or the participant clearly moved or pushed during rest.

`clipped` and `late` come from the `#TRIAL` lines saved inside every CSV. `late` (15–19 samples per trial) always falls in the first ~16 ms of the movement phase, when the LCD changes to "Empuja YA!"; this is within the reaction time and is trimmed in cleaning. Brief activity in the first ~0.5–1 s of rest (release after pushing) is also handled by trimming rest and movement onsets.

| File | Session | Set | clipped | late per trial | Rejected trials | Reason / notes |
|---|---|---|---|---|---|---|
| S01_R01.csv | 1 | train | 0 | 15–18 | none | Trial 4: spike during rest (~2.4 s). Trials 6, 8: incomplete relaxation at start of rest. Trial 2: early activation at end of rest. |
| S01_R02.csv | 1 | train | 0 | 16–19 | none | Trial 4: slightly incomplete relaxation in the first ~1 s of rest. |
| S01_R03.csv | 1 | train | 0 | 16–19 | **7, 8** | Data loss on the computer side (other programs running during recording): trials 7–8 have ~7 750 of 14 000 samples and corrupted lines. Trials 1–6 complete. |
| S02_R01.csv | 2 | train | 0 | 16–19 | **4** | Trial 4: strong activity at end of rest (~3.5–5 s), likely early push. Trials 1, 3: incomplete relaxation at start of rest. |
| S02_R02.csv | 2 | train | 0 | 16–19 | **5** | Trial 5: strong activity during rest (from ~3 s). |
| S02_R03.csv | 2 | train | 0 | 16–19 | none | Brief activity in first ~0.5 s of rest in several trials. Baseline drifted down during session 2 (~8100 → ~7400): low battery (7.77 V), replaced before S3. |
| S03_R01.csv | 3 | train | 0 | 15–18 | none | Trial 2: slight activity in first ~1 s of rest. Stable after battery replacement. |
| S03_R02.csv | 3 | train | 0 | 15–18 | none | Clean. |
| S03_R03.csv | 3 | train | 0 | 15–18 | **1–8 (all)** | Circuit output stuck at ~3137 from the start. No EMG signal. |
| S03_R04.csv | 3 | train | 0 | 15–18 | **1–8 (all)** | Circuit output stuck at ~3135 from the start. No EMG signal. |
| S03_R05.csv | 3 | train | 0 | 15–18 | **6** | Recorded after power-cycling the circuit. Trial 6: baseline jump (~7550 → ~8050 → ~7300), likely lead movement. Trial 4: slight activity at end of rest. |
| S04_R01.csv | 4 | train | 0 | 15–18 | none | Higher 60 Hz interference in rest (notch in cleaning). |
| S04_R02.csv | 4 | train | 0 | 15–18 | **1–8 (all)** | Output stuck at ~3137 from 1.3 s into the warning of trial 2 (sudden step, lead contact loss). Recording rejected. |
| S04_R03.csv | 4 | train | 0 | 15–18 | **1–8 (all)** | Output stuck at ~3140 from the first sample. Fixed by reconnecting electrode leads one by one; automatic pre-check added to the recording script from S04_R04 on. |
| S04_R04.csv | 4 | train | 0 | 15–18 | none | 60 Hz interference in rest. |
| S04_R05.csv | 4 | train | 0 | 15–18 | none | Trial 2: brief activity at start of rest. 60 Hz in rest. |
| S05_R01.csv | 5 | train | 0 | 16–19 | none | Clean. |
| S05_R02.csv | 5 | train | 0 | 16–19 | **1–8 (all)** | Strong 60 Hz interference (rest noise ~30× higher than R01, larger than movement), likely electrode contact loss. |
| S05_R03.csv | 5 | train | 0 | 16–19 | none | Trial 5: brief spike near end of rest (~4 s). |
| S05_R04.csv | 5 | train | 0 | 16–19 | none | Clean. |
| S06_R01.csv | 6 | test | 0 | 15–18 | none | Technical check only (test set not analysed). Mild 60 Hz in rest. |
| S06_R02.csv | 6 | test | 0 | 15–18 | none | Technical check only. Trials 7–8: increased 60 Hz in rest; kept. |
| S06_R03.csv | 6 | test | 0 | 15–18 | none | Technical check only. Moderate 60 Hz in rest. |

## Summary

| Session | Set | Electrodes changed | Recordings | Valid trials |
|---|---|---|---|---|
| 1 | train | all | 3 | 22 |
| 2 | train | A | 3 | 22 |
| 3 | train | B | 5 | 23 |
| 4 | train | all | 5 | 24 |
| 5 | train | R | 4 | 24 |
| 6 | test | A + B | 3 | 24 |
| **Total** | | | **23** | **139** (115 train, 24 test) |

## Session notes

- Batteries before S3: 7.77 V and 8.77 V; the 7.77 V battery was replaced. Later readings: 8.87 V and 8.56 V.
- Recurring problem: the circuit output sometimes gets stuck at ~3140 counts after cables or electrodes move. It was fixed by disconnecting and reconnecting the electrode leads one by one, or by power-cycling the batteries. From S04_R04 the recording script runs a 10 s pre-check and does not record if the signal is stuck (`#PRECHECK` line in the CSV).
- The test session (S6) was only checked for technical validity; it was not plotted or analysed.

## Pilot recordings (not in the dataset)

| What | Why it was not used |
|---|---|
| 17 Nov, recordings at 1000 Hz with LCD countdown | `late` ≈ 400 per trial: the LCD slowed down the sampling. |
| 17–18 Nov, recordings at 500 Hz | Sampling rate below 2 × 433 Hz (upper cut-off of the band-pass filter): aliasing. |
| 18 Nov, elbow flexion lifting the forearm (with and without weight) | Arm movement saturated the circuit (output stuck at ~3140). Changed to an isometric task: pushing up against a table without moving the arm. |
| 18 Nov, checks before S1 | Output stuck because of a bad contact in an electrode lead. |
