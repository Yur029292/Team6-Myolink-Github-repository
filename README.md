# Test-1-Github-repository
Primer repositorio de github para probar su funcionamiento // dummy

Añadi codigos Robot_Test
en cada etapa de avance del codigo de la garra robotica


# Report 1, the data stage
One person, one movement, one model. We record surface EMG from the biceps brachii of one person with a sensor circuit we built ourselves and an Arduino Uno R4 WiFi. We train a model to tell movement from rest, and we test it on sessions with new electrodes that the model never saw. Goal: more than 75 % correct on unseen sessions.

## Team 6 Application of Bioinstrumentation Technologies Roles
|Member code|GitHub user|In charge of|AI tool|
|-----|-----|-----|-----|
|M01|yur029292|code and versions|Visual Studio with Claude|
|M02|a01779350-coder|quality report|Claude|
|M03|     |code and versions|Claude|
|M04|jamesmorrisey123|protocol and cue video|Claude|

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

**Evidence:** `cue/cue-video.mp4`, the columns `rest_s`, `warning_s`, `move_s`, `between_s` of `data/metadata.csv`, `figures/one_recording_marked.png`.

### 4. Sessions
1 person. 6 sessions on 1 day, split into sessions 1-3 and 4-6 with a 1.5 hour break inbetween the two groupings. Every session starts with new electrodes, placed again from zero. Each session has 3 recordings. 

**Evidence:** the columns `participant`, `session` and `date` of `data/metadata.csv`, one photo per session in `figures/`.

### 5. Cleaning
Our circuit already filters in hardware (26–433 Hz) but has no notch. PROTOCOL.md lists the filter with its type, order, limits and source. We fixed the rejection rules before training any model. A trial is dropped when its clipped counter is above 0 (a sample reached 0 or 16383), when its late counter is above 0 or t_us shows a gap, or when the signal sticks near 3150, which means the amplifier saturated. Outcome: ✏️ N trials dropped, ✏️ N kept. Nothing in data/raw/ was edited or deleted.

**Evidence:** `QUALITY-REPORT.md`, `code/check_quality.py`, the #TRIAL lines (clipped, late) of every raw file.

### 6. Fair split
Training: sessions S01 to S04, 75 trials. Test: sessions S05 and S06, 38 trials, recorded on another day with new electrodes. The model never sees these two sessions in training. Limit: the model is built and tested on one person and one movement. We do not claim that it works for other people. We do not shuffle windows of one recording between training and test.

We split by whole sessions, never by windows. S01 to S04 (96 N trials) are for training and for every choice of features and model. S05 and S06 (48 N trials) are the test, each with its own electrode placement, and we open them only once, at the end. Because one recording never feeds both sets, the score shows how the model handles a new placement. Scope: one person (P01) and one movement, so the result says nothing about other people or other muscles.

**Evidence:** the column `set` of `data/metadata.csv`.

## What comes next
Our decisions about features, the model and the route are in `DECISIONS.md`. The rationale of each member is in `rationale/`.

## Where to search
- How to organise the files: Wilson and others, [Good enough practices in scientific computing](https://journals.plos.org/ploscompbiol/article?id=10.1371/journal.pcbi.1005510), 2017.
- What to report about a muscle recording: Merletti, [Standards for Reporting EMG Data](https://isek.org/wp-content/uploads/2015/05/Standards-for-Reporting-EMG-Data.pdf), 1999, and the newer checklist [CEDE-Check](https://cede.isek.org/matrix/cede-check/).
- The steps from signal to model, and the fair split: Riascos Salas, Molinas and Lotte, [Machine learning methods for BCI](https://www.esann.org/sites/default/files/proceedings/2024/ES2024-4.pdf), 2024. It is about brain signals, and the same steps apply to muscle signals.
- Features and models: the [scikit-learn user guide](https://scikit-learn.org/stable/user_guide.html). Models for a microcontroller: [emlearn](https://emlearn.readthedocs.io/en/latest/).

## How to present it well
Write numbers, not adjectives. Give seconds, counts and file names. Use codes, never names.

## How to repeat our recording
Read `PROTOCOL.md`. It is enough to record the same way without asking us.

## Versions
Two versions are marked on GitHub: **First version** and **Report 1**. Open "Releases" on the right of the main page to see them with their dates.
