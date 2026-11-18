# Code Files

- `emg_fisio.html` is the HTML code for the Bluetooth data transmission from the emg and displays the data for the physio via chrome. This was created by James.
- `emg_lcd_ble.ino` runs the emg code along with the LED and LCD displays before connecting via BLuetooth to the HTML data display. This was completed by Mariana.
- `robot_ble.ino` is the code to power the robot. It receives the true or false results from the emg via Bluetooth which instructs it to move horizontally. This was completed by Yurem.
- `check_quality.py` applies the rule of QUALITY-REPORT.md to every file in data/raw/. Empty in this example.

Each file starts with one line that says what it does and who wrote it.
