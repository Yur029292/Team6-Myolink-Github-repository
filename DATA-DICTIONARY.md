# Data dictionary (example)

One row of `data/metadata.csv` describes one recording.

| Column | Meaning | Unit or allowed values |
|---|---|---|
| file_name | the file in `data/raw/` | participant_session_recording.csv |
| participant | the code of the person | P01, P02, ... never a name |
| session | one electrode placement | S01, S02, ... |
| date | the day of the session | year-month-day |
| recording | the number of the recording inside its session | 1, 2, ... |
| trials | trials in the recording | count |
| rest_s, warning_s, move_s, between_s | the windows of one trial | seconds |
| fs_hz | sampling frequency | hertz |
| muscle, side | where the sensor sits | text |
| electrodes | when the electrodes were placed | text |
| electrode_type, electrode_distance_cm | the electrodes and the distance between them | text, centimetres |
| skin_preparation | how the skin was cleaned | text |
| sensor_model, gain | the sensor and its gain | text |
| adc_bits | the bits of the converter | count |
| cue | the cue the person watched | path of the file |
| set | the use of the recording | train or test |
| trials_rejected | trials that the cleaning rule rejected | count |
| notes | the reason for each rejection | text |

One row of a file in `data/raw/` is one sample: `time_ms` in milliseconds, `emg_adc` as the number from the converter (0 to 1023).
