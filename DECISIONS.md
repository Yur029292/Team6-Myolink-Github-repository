# Decisions about what comes next

## 7. Features

## 8. Model

| Option | In plain words | What it keeps after training |
|---|---|---|
| Threshold | "movement if the size is above this number" | One number |

## 9. Route
Chose: The model runs through an Arduino Master that receive the signal from the EMG on Pin A0, then it transfer the data via bluetooth at the computer and connects via Bluetooth to an Arduino slave that receives a boolean signal if the system detects a contraction,
