# Report 1, the data stage
One person, one movement, one model. We record surface EMG from the biceps brachii of one person with a sensor circuit we built ourselves and an Arduino Uno R4 WiFi. We train a model to tell movement from rest, and we test it on sessions with new electrodes that the model never saw. Goal: more than 75 % correct on unseen sessions.

## Team 6 Application of Bioinstrumentation Technologies Roles
|Member code|GitHub user|In charge of|AI tool|
|-----|-----|-----|-----|
|M01|yur029292|test subject and creating and sharing GitHub repository|Claude|
|M02|a01779350-coder|finished decisions|Claude|
|M03|a01712808-sys|coding and recording for data acquisition and cue video|Claude|
|M04|jamesmorrisey123|creation/organisation and completion of files in GitHub | Claude |

We measure one person, member M01. In the data that person has the code P01.

## The Six Parts

### 1. Data acquisition
18 recordings, 144 trials, 6 sessions, 1 person. One pair of Ag/AgCl gel electrodes, 2 cm apart, on the biceps brachii of the left arm, with the reference on the elbow. Our own amplifier circuit (AD620, 8th-order band-pass 26–433 Hz, total gain 300, 2.5 V offset) feeds pin A0 of an Arduino Uno R4 WiFi. 14-bit converter, sampled at 500 Hz, measured from the t_us column. The sensor gives the raw signal, not a smoothed one. The person follows the LED and LCD cue, shown in cue/cue-video.mp4. One trial lasts 14 s.

**Evidence:** `data/raw/` (18 files), `data/metadata.csv` (18 rows), `cue/`.

### 2. Task
The person pushes the palm up against the edge of a table and holds it, without moving the arm (isometric elbow flexion). Two classes: movement and rest. The person sits, with the elbow and forearm rested on the seated leg, elbow at 90° and palm up against the underside of the table.

**Evidence:** `PROTOCOL.md`, `figures/placement_S01.jpeg`.

### 3. Trial and cue timing
| Window | Seconds | What the person sees |
|---|---|---|
| Rest before | 0 to 5 | Red LED, screen: "RELAJA EL BRAZO Trial X/8" |
| Warning cue | 5 to 8 | Yellow LED, screen: "AL VERDE EMPUJA Empuja en: 3...2...1" |
| Action window | 8 to 13 | Green LED, screen: "AL VERDE EMPUJA Empuja YA!" |
| Rest between trials | 13 to 14 | Red LED, screen: "RELAJA EL BRAZO Trial (X+1)/8" |

One recording holds 8 trials, so it lasts 112 s. Per trial: 5 s of movement and 6 s of rest.

**Evidence:** `cue/cue-video.mp4`, the columns `rest_s`, `warning_s`, `move_s`, `between_s` of `data/metadata.csv`.

### 4. Sessions
1 person. 6 sessions on 1 day, split into sessions 1-3 and 4-6 with a 1.5 hour break inbetween the two groupings. Every session starts with new electrodes, placed again from zero. Each session has 3 recordings. 

**Evidence:** the columns `participant`, `session` and `date` of `data/metadata.csv`, one photo per session in `figures/`.

### 5. Cleaning
Our circuit already filters in hardware (26–433 Hz) but has no notch. PROTOCOL.md lists the filter with its type, order, limits and source. We fixed the rejection rules before training any model. A trial is dropped when its clipped counter is above 0 (a sample reached 0 or 16383), when its late counter is above 0 or t_us shows a gap, or when the signal sticks near 3150, which means the amplifier saturated. Outcome: 5 trials dropped, 139 kept. Nothing in data/raw/ was edited or deleted.

**Evidence:** `QUALITY-REPORT.md`, `code/check_quality.py`, the #TRIAL lines (clipped, late) of every raw file.

### 6. Fair split
We split by whole sessions, never by windows. S01 to S05 (126 N trials) are for training and for every choice of features and model. S06 (18 N trials) is the test, each with its own electrode placement, and we open them only once, at the end. Because one recording never feeds both sets, the score shows how the model handles a new placement. Scope: one person (P01) and one movement, so the result says nothing about other people or other muscles.

**Evidence:** the column `set` of `data/metadata.csv`.

## What comes next
Our decisions about features, the model, and the route are in `DECISIONS.md`. The rationale of each member is in `rationale/`.

## How to repeat our recording
Read `PROTOCOL.md`. It is enough to record the same way without asking us.

## Versions
Two versions are marked on GitHub: **First version** and **Report 1**. Open "Releases" on the right of the main page to see them with their dates.
