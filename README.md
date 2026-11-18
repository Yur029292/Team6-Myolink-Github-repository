# Test-1-Github-repository
Primer repositorio de github para probar su funcionamiento // dummy

Añadi codigos Robot_Test
en cada etapa de avance del codigo de la garra robotica

# EMG Biceps — Data Stage (M6 Machine Learning and IoT Apps, Report 1)
One person, one movement, one model. We record surface EMG from the biceps brachii of one person with a sensor circuit we built ourselves and an Arduino Uno R4 WiFi. We train a model to tell movement from rest, and we test it on sessions with new electrodes that the model never saw. Goal: more than 75 % correct on unseen sessions.

## Completed Parts
|Part|Our Answer|
|-----|-----|
|1 Data acquisition|18 recordings, 144 N trials, 6 sessions, 1 person. 3 Ag/AgCl gel electrodes 2 cm apart on the left biceps, reference on the elbow. Our own amplifier circuit, read by an Arduino Uno R4 WiFi at 500 samples per second, 14-bit ADC. Raw signal, not smoothed.|
|2 Task|Isometric elbow flexion: forearm laying on horizontal surface under the table, elbow at 90°, palm up, push up against the table edge without moving the arm. Two classes: movement and rest.|
|3 Trial and cue timing|Rest 5 s, warning 3 s, movement 5 s, pause 1 s. 8 trials in one recording. The person follows LED and LCD cues on the Arduino (see cue video).|
|4 Sessions|6 sessions on 2 days, 3 recordings each. New electrodes in every session. Pilot recordings (arm lifting a water bottle) saturated the amplifier and are not in the dataset.|
|5 Cleaning|Offset removal, band-pass 26–433 Hz. We reject a trial if a sample hits the ADC limit, samples are late or missing, or the amplifier saturates. ✏️ N of 144 trials rejected.|
|6 Fair split|Sessions 1 to 4 train the model. Sessions 5 and 6 test it. No session is in both.|

## Team 6 Application of Bioinstrumentation Technologies Roles
|Member|GitHub|In charge of|AI tool|
|-----|-----|-----|-----|
|Yurem|yur029292|code and versions|Visual Studio with Claude|
|Alejandra|a01779350-coder|quality report|Claude|
|Mariana|     |quality report|Claude|
|James|jamesmorrisey123|protocol and cue video|Clause|
