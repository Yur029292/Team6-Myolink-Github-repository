# Data dictionary

## `data/metadata.csv`

One row of `data/metadata.csv` describes one recording (23 rows, one per file in `data/raw/`, rejected recordings included).

| Column | Meaning | Unit or allowed values |
|---|---|---|
| file | the file in `data/raw/` | `S<session>_R<recording>.csv`, e.g. `S03_R02.csv` |
| participant | the code of the person | P01, never a name |
| session | one electrode placement | 1–6 |
| recording | the number of the recording inside its session | 1, 2, ... |
| date | the day of the session | year-month-day |
| trials | trials in the recording | count (8) |
| rest_s, warning_s, move_s, between_s | the windows of one trial | seconds (5, 3, 5, 1) |
| fs_hz | sampling frequency | hertz (1000) |
| muscle, side | where the sensor sits | text (biceps brachii, left) |
| task | the movement the person does | text |
| electrodes_changed | which electrodes were replaced at the start of the session | `all`, `A` (active near the elbow), `B` (other active), `R` (reference), `A+B` |
| electrode_type, electrode_distance_cm | the electrodes and the distance between the two actives, centre to centre | text, centimetres |
| electrode_to_elbow_cm | distance from the elbow crease to the centre of the active electrodes | centimetres |
| reference_site | where the reference electrode sits | text |
| skin_preparation | how the skin was cleaned | text (`none`) |
| sensor_model, gain | our amplifier circuit and its total gain | text, times (300) |
| adc_bits | the bits of the converter | count (14) |
| offset_mean | mean of the 10 s rest check (`c` command) at the start of the session; used to remove the offset in cleaning | ADC counts |
| cue | the cue the person watched | path of the file |
| set | the use of the recording | `train` (sessions 1–5) or `test` (session 6) |
| trials_rejected | trials that the cleaning rules rejected | count |
| notes | the reason for each rejection | text |

## Files in `data/raw/`

Each file is one recording, saved by `code/grabar.py` exactly as the Arduino sent it. Never edit it.

**Lines that start with `#`** are information, not samples:

| Line | Meaning |
|---|---|
| `#session`, `#recording`, `#date` | written by `grabar.py` when the recording starts |
| `#PRECHECK,min,…,max,…,mean,…,clipped,…` | 10 s rest check done just before recording (from S04_R04 on) |
| `#BEGIN` | the Arduino starts the recording |
| `#muscle`, `#fs_hz`, `#adc_bits`, `#rest_s`, `#warning_s`, `#move_s`, `#between_s`, `#trials`, `#phase_codes` | the settings used by the Arduino |
| `#TRIAL,<n>,clipped,<x>,late,<y>` | written at the end of each trial: `clipped` = samples at the converter limit, `late` = samples taken late during rest or movement |
| `#END` | the recording finished |

**Every other row is one sample:**

| Column | Meaning | Unit or allowed values |
|---|---|---|
| idx | sample number since the start of the recording | count, from 0 |
| t_us | time when the sample was taken (Arduino clock) | microseconds |
| trial | trial number | 1–8 |
| phase | the part of the trial | 0 = rest, 1 = warning, 2 = movement, 3 = between |
| emg | the raw number from the converter, no offset removed | 0 to 16383 (≈ 8000 at rest) |

One trial has 14 000 samples (14 s × 1000 Hz). One ADC count is about 0.31 mV at the Arduino, or about 1 µV at the electrodes (gain 300).

## `data/quality_trials.csv`

Written by `code/check_quality.py`. One row per trial.

| Column | Meaning | Unit or allowed values |
|---|---|---|
| file, session, recording, set, trial | which trial | as above |
| samples, expected | samples found and samples expected in the trial | count (14 000 expected) |
| clipped, late | the counters from the `#TRIAL` line | count |
| trial_median | median of the whole trial; below 5000 means the amplifier is stuck | ADC counts |
| rest_rms | RMS of the rest after its first second | ADC counts |
| baseline_jump | difference between the medians of rest and between | ADC counts |
| decision | `keep` or `reject` | text |
| reasons | the rules that rejected the trial | R1 clipped, R2 incomplete, R3 stuck, R4 noise, R5 jump, manual |
