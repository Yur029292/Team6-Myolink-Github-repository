# Decisions about what comes next

|Part|Our answer|
|-|-|
|7 Features|Chose: RMS and mean absolute value per 200 ms window. Reason: in our checks, pushing made the signal about 20 times wider than at rest, so the size of the signal should separate the classes. Not understood yet: does a frequency feature, like median frequency, add anything, and how long should the window be?|
|8 Model|Not decided yet. We compare a threshold on RMS, logistic regression and a random forest. Our AI tool suggested logistic regression because it is simple and small enough for the Arduino. Not understood yet: how to tune the model without opening the test sessions.|
|9 Route|Chose: the model runs in our Python software on the computer, which already receives the data. Each measurement is saved with its metadata, under a patient code and encrypted, in a cloud-synced folder. Reason: sending to Google Sheets over WiFi failed, and patient privacy comes first. Not understood yet: could the model run on the Arduino R4 as TinyML instead?|

## 7. Features

## 8. Model

| Option | In plain words | What it keeps after training |
|---|---|---|
| Threshold | "movement if the size is above this number" | One number |

## 9. Route
Chose: The model runs through an Arduino Master that receive the signal from the EMG on Pin A0, then it transfer the data via bluetooth at the computer and connects via Bluetooth to an Arduino slave that receives a boolean signal if the system detects a contraction,
