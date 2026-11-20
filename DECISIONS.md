# Decisions about what comes next

One person (P01), left biceps brachii, isometric elbow flexion against a table. Two classes: movement and rest.
Rule for every choice below: we choose with sessions S01 to S05 only. The test session S06 is opened once, at the end.

| Part | Our answer |
|---|---|
| 7 Features | Chose: RMS and mean absolute value (MAV) per 200 ms window. Reason: in our checks, pushing made the signal about 20 times wider than at rest, so the size of the signal should separate the classes. Not understood yet: does a frequency feature, like median frequency, add anything, and how long should the window be? |
| 8 Model | Not decided yet. We compare three models, from simplest to most flexible: a threshold on RMS, logistic regression and a random forest. Our AI tool suggested logistic regression because it is simple and small enough for the Arduino. Not understood yet: how to tune the model without opening the test session. |
| 9 Route | Chose: the model runs in our Python software on the computer, which already receives the data from the Arduino over USB. Each measurement is saved with its metadata under a patient code (P01), encrypted, in a cloud-synced folder. Reason: sending to Google Sheets over WiFi failed, and patient privacy comes first. Not understood yet: could the model run on the Arduino Uno R4 as TinyML instead? |

---

## 7. Features

1. **Chose:** two numbers per window of 200 ms (100 samples at 500 Hz): RMS and mean absolute value (MAV). They are computed on the filtered signal (offset removal + band-pass 26-240 Hz).
2. **Reason:** our sensor gives the raw EMG, not an envelope. In our checks the signal during pushing was about 20 times wider than at rest, so the size of the signal should be enough to tell movement from rest. RMS and MAV are the standard measures of size and are cheap to compute, which matters if the model ever runs on the Arduino. Each window gets its label from the cue phase (green = movement, red = rest). We cut the first part of rest after a push and the first part of movement after the green light (reaction time), so that the labels are not wrong near the changes.
3. **Not understood yet:** does a frequency feature (median frequency) add anything on top of the size? How long should the window be: shorter reacts faster, longer is more stable? And what do we do about the 60 Hz interference in rest (our band-pass keeps 60 Hz and the quality report says some sessions have a lot of it)? We will check this on S01-S05 only.

## 8. Model

1. **Not decided yet.** We compare three options on the training sessions:

| Option | In plain words | What it keeps after training |
|---|---|---|
| Threshold on RMS | "movement if the size is above this number" | One number |
| Logistic regression | "a weighted sum of the features; above a limit means movement" | A few weights and one bias |
| Random forest | "many small decision trees vote" | Many trees (much bigger) |

2. **Reason:** we start from the simplest one and only go to a more flexible one if it is clearly better on unseen sessions. Our AI tool suggested logistic regression because it is simple and small enough for the Arduino. We have 5 training sessions, so a very flexible model could memorise each electrode placement instead of learning the movement.
3. **Not understood yet:** how to tune the model (threshold, regularisation, forest depth) without opening the test sessions. Our plan: leave-one-session-out cross-validation on S01-S05 (train on 4 sessions, validate on the 5th, repeat), so every score is on a placement the model did not see, like the real test. S06 is used once, at the end. Goal: more than 75 % correct on S06.

## 9. Route

1. **Chose:** the model runs in our Python software on the computer. The Arduino Uno R4 WiFi reads the EMG on pin A0 and sends it over USB serial (as in `grabar.py`). Python computes the features, runs the model and saves each measurement with its metadata (participant code P01, session, date, fs_hz, gain, result) in a cloud-synced folder, encrypted. If a signal is needed on a second board, Python can send a yes/no ("contraction detected") to it.
2. **Reason:** the computer already receives the data and has the filters and the model code, so there is only one place to maintain. Sending to Google Sheets over WiFi failed in our tests, and patient privacy comes first: data uses codes, never names.
3. **Not understood yet:** could the model run on the Arduino R4 as TinyML instead? A threshold or logistic regression looks small enough, but we do not know how to port the filter and the features to the board, or how the board would send the record to the cloud by itself.
