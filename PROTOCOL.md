# Protocol

Anyone outside our group should be able to record the same way by following these steps, without asking us.

1. **Sensor and site.** Two Ag/AgCl gel electrodes, 2 cm apart center to center, on the belly of the left biceps brachii, in line with the fibers. Per SENIAM, they go on the line from the medial acromion to the cubital fossa, one third of the way up from the fossa. The reference electrode goes on the elbow bone, a spot that does not move when the biceps contracts. Clean the skin with alcohol and let it dry first.
2. **Circuit.** Our own amplifier: AD620 instrumentation amplifier (gain 20), 8th-order band-pass 26–433 Hz (2× TL084), post-gain ×5 and ×3 (TL082), then a 2.5 V offset. Total gain 300, the same in every session. Powered by two 9 V batteries (±9 V). Each battery must read above 8 V before a session. The circuit GND connects to the Arduino GND, and the output goes to pin A0 of an Arduino Uno R4 WiFi.
3. **Posture.** Seated next to a table. Right elbow and forearm rested of seated thigh, elbow at 90°, palm up, with the palm already under the table edge. The arm stays in this position in every phase.
4. **Task.** Following LCD instructions, the green cue, push the palm up against the table edge, firm but steady, without moving the arm (isometric elbow flexion). On red, stop pushing and relax. Classes: movement = pushing, rest = relaxed.
5. **Trial.** Rest 5 s, warning 3 s, push 5 s, pause 1 s. Total 14 s.
6. **Recording.** 8 trials in a row, 112 s in total per recording. Before pressing start, the person stays still for 20–30 s with the arm already on the table, so the circuit is stable from the first trial. 2 min of rest between recordings.
7. **Cue.** The Arduino gives the cue itself, so every recording gets exactly the same timing. Red LED + rest screen, yellow LED + "PREPARATE" with a countdown, green LED + "EMPUJA LA MESA", red LED + "DEJA DE EMPUJAR". `cue/cue-video.mp4` shows one full trial.
8. **Session.** New electrodes at the start, placed again from zero, and one photo of the placement (no face). Then a signal check with the `c` command: at rest the mean must sit near 8000 with a small range (about 100), and while pushing it must stay near 8000 with a wide range (about 2000). If the mean sticks near 3150, do not record. Fix the leads, the reference or the batteries first. Then 3 recordings. 6 sessions across 1 day. Split into Sessions 1-3 and 4-6 with a 1.5 hour gap between the two groupings.
9. **Settings.** The Arduino samples at 500 Hz with a 14-bit converter (0–16383), and we measure the rate from the `t_us` column. `grabar.py`, run in Thonny over USB serial, saves one CSV per recording as `S<session>_R<recording>.csv`. Close the Arduino IDE before recording so the port is free. Each file holds a header (`rest_s, warning_s, move_s, between_s, trials, fs_hz`), one row per sample (`t_us`, trial, phase 0–3, ✏️ ADC value), and one `#TRIAL` line per trial with its `clipped` and `late` counters. Never open a raw file in Excel and save it.
10. **Filters.** The raw files keep the signal exactly as the converter gave it. In code, on a copy, in this order:

|Order|Step|Type and limits|Source|
|-|-|-|-|
|1|Offset removal|Subtract the mean of the session's resting signal	Merletti, ISEK standards, 1999
|2|Band-pass|Butterworth, 4th order, zero-phase, 26–240 Hz	De Luca and others, Filtering the surface EMG signal, J Biomech, 2010 (lower limit near 20 Hz)

The upper limit stays below 250 Hz, half of our sampling rate. Our sensor gives the raw signal, not an envelope. Known limit: the hardware filter passes up to 433 Hz, so content between 250 and 433 Hz can fold back (alias) into the data.

11. **Reject a trial when** its `clipped` counter is above 0, its `late` counter is above 0 or `t_us` shows a gap, the signal sticks near 3150 (amplifier saturation), or the session notes say the person moved, coughed or an electrode came loose. Never delete a file. Write the reason in `QUALITY-REPORT.md`. Recordings that are not usable go to `data/pilot/`, for example arm lifting, other rates or a different setup.
12. **Consent.** The participant agreed to be recorded. The data uses codes (P01, member codes), never names.

## Sources
SENIAM, *recommendations for sensor locations: biceps brachii.* seniam.org\
Merletti R. *Standards for Reporting EMG Data.* ISEK, 1999.\
De Luca CJ, Gilmore LD, Kuznetsov M, Roy SH. *Filtering the surface EMG signal: movement artifact and baseline noise contamination.* J Biomech 43(8), 2010.

## We read the method and we agree
Each member adds their own line from their own GitHub account.

- M01, 2026-11-17, I read the method and I agree.
- M02, 2026-11-17, I read the method and I agree.
- M03, 2026-11-17, I read the method and I agree.
- M04, 2026-11-17, I read the method and I agree.
