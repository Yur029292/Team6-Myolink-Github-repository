#include <Wire.h>                // [LCD] I2C communication
#include <LiquidCrystal_I2C.h>   // [LCD] Library: "LiquidCrystal I2C" by Frank de Brabander
#include <ArduinoBLE.h>          // [BLE] Library: "ArduinoBLE" by Arduino

const int EMG_PIN = A0;

const int RED_LED = 8;
const int YELLOW_LED = 9;
const int GREEN_LED = 10;
const int SLAVE_SIGNAL_PIN = 7;

const int SAMPLE_RATE = 1000;
const int SAMPLE_PERIOD_US = 1000000 / SAMPLE_RATE;

//RMS window
const int WINDOW_SIZE = 100;

//Calibration
const int CALIBRATION_TIME = 3000;

// Detection settings
float restFactor = 1.5;
float thresholdFactor = 2.5;

float baselineMean = 0;
float baselineRMS = 0;

float restLimit = 0;
float thresholdRMS = 0;

// [LCD] 16x2 LCD at I2C address 0x27 (if nothing shows, try 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// [LCD] -1 = nothing yet, 0 = rest, 1 = activity, 2 = valid contraction
int lcdState = -1;

// [BLE] ===== Bluetooth Low Energy =====
// These UUIDs must match the ones in the web page (emg_fisio.html)
BLEService emgService("6e400001-b5a3-f393-e0a9-e50e24dcca9e");

// Data sent to the page ~10 times per second (17 bytes):
// [0-3] time ms (uint32) | [4-7] RMS (float) | [8-11] rest limit (float)
// [12-15] threshold (float) | [16] state (0 rest, 1 activity, 2 valid, 3 calibrating)
BLECharacteristic dataChar("6e400003-b5a3-f393-e0a9-e50e24dcca9e", BLERead | BLENotify, 17);

// Commands from the page: 'C' = recalibrate
BLEByteCharacteristic cmdChar("6e400002-b5a3-f393-e0a9-e50e24dcca9e", BLEWrite | BLEWriteWithoutResponse);

bool bleOK = false;               // [BLE] false if the Bluetooth module did not start
bool recalibrateRequested = false; // [BLE] set when the physio presses "Recalibrar"


// SETUP
void setup() {

  Serial.begin(115200);

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Output signal for slave Arduino
  pinMode(SLAVE_SIGNAL_PIN, OUTPUT);
  digitalWrite(SLAVE_SIGNAL_PIN, LOW);

  showRest();

  // [LCD] Start the screen
  lcd.init();
  lcd.backlight();
  lcdShow("Sistema EMG", "Iniciando...");

  setupBLE();   // [BLE]
  waitWithBLE(2000);

  // Calibration with patient instructions (same steps as before, now in a
  // function so the physio can repeat it from the web page)
  calibrationSequence();

  delay(1000);
}


// LOOP
void loop() {

  float rms = calculateRMS();

  Serial.print("RMS:");
  Serial.print(rms);

  Serial.print(",Rest:");
  Serial.print(restLimit);

  Serial.print(",Threshold:");
  Serial.println(thresholdRMS);

  byte state;   // [BLE] state to send to the page

  // REST
  if (rms < restLimit) {

    showRest();
    lcdUpdateState(0);   // [LCD]
    state = 0;           // [BLE]
  }


  // ACTIVITY DETECTED
  else if (rms < thresholdRMS) {

    showActivity();
    lcdUpdateState(1);   // [LCD]
    state = 1;           // [BLE]
  }


  // VALID CONTRACTION
  else {

    showValid();
    lcdUpdateState(2);   // [LCD]
    state = 2;           // [BLE]

    Serial.println("***** VALID CONTRACTION *****");
  }

  // [BLE] Send data and check for commands
  sendBLE(rms, state);

  if (recalibrateRequested) {
    recalibrateRequested = false;
    showRest();
    calibrationSequence();
    lcdState = -1;   // [LCD] force the screen to redraw after recalibrating
  }
}


// CALIBRATION SEQUENCE (LCD instructions + calibrateRest + Serial report)
void calibrationSequence() {

  sendBLE(0, 3);   // [BLE] tell the page we are calibrating

  // [LCD] Patient instructions
  lcdShow("Sientese comodo", "y quedese quieto");
  waitWithBLE(3000);
  lcdShow("Relaje la pierna", "NO la mueva");
  waitWithBLE(3000);
  lcdCountdown("Calibrando en", 3);

  delay(1000);

  lcdShow("Calibrando...", "Siga relajado");   // [LCD]

  Serial.println("Keep muscle relaxed...");
  calibrateRest();

  Serial.println("Calibration finished");

  Serial.print("Baseline ADC = ");
  Serial.println(baselineMean);

  Serial.print("Baseline RMS = ");
  Serial.println(baselineRMS);

  Serial.print("Rest limit = ");
  Serial.println(restLimit);

  Serial.print("Threshold RMS = ");
  Serial.println(thresholdRMS);

  // [LCD] Tell the patient calibration is done and what to do next
  lcdShow("Calibracion", "completa!");
  waitWithBLE(2000);
  lcdShow("Listo!", "Contraiga muslo");
}


// CALIBRATION
void calibrateRest() {

  unsigned long start = millis();

  long sampleCount = 0;

  double sum = 0;
  double sumSquared = 0;

  while (millis() - start < CALIBRATION_TIME) {

    int raw = analogRead(EMG_PIN);

    sum += raw;
    sumSquared += (double)raw * raw;

    sampleCount++;

    delayMicroseconds(SAMPLE_PERIOD_US);
  }

  baselineMean = sum / sampleCount;

  double meanSquare = sumSquared / sampleCount;

  baselineRMS =
      sqrt(meanSquare - baselineMean * baselineMean);

  restLimit = baselineRMS * restFactor;

  thresholdRMS = baselineRMS * thresholdFactor;
}


// RMS
float calculateRMS() {

  double sumSquares = 0;

  for (int i = 0; i < WINDOW_SIZE; i++) {

    int raw = analogRead(EMG_PIN);

    float emg = raw - baselineMean;

    sumSquares += emg * emg;

    delayMicroseconds(SAMPLE_PERIOD_US);
  }

  return sqrt(sumSquares / WINDOW_SIZE);
}


// LED FUNCTIONS
void showRest() {

  digitalWrite(RED_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  // FALSE sent to slave
  digitalWrite(SLAVE_SIGNAL_PIN, LOW);
}


void showActivity() {

  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);

  // FALSE sent to slave
  digitalWrite(SLAVE_SIGNAL_PIN, LOW);
}


void showValid() {

  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);

  // TRUE sent to slave
  digitalWrite(SLAVE_SIGNAL_PIN, HIGH);
}


// [BLE] ===== BLUETOOTH FUNCTIONS (all new) =====

void setupBLE() {

  if (!BLE.begin()) {
    Serial.println("BLE failed to start - continuing without Bluetooth");
    bleOK = false;
    return;
  }

  BLE.setLocalName("EMG-Cuadriceps");
  BLE.setDeviceName("EMG-Cuadriceps");
  BLE.setAdvertisedService(emgService);

  emgService.addCharacteristic(dataChar);
  emgService.addCharacteristic(cmdChar);
  BLE.addService(emgService);

  // Keep advertising after a connection so the robot AND the web page can both connect
  BLE.setEventHandler(BLEConnected, onBLEConnected);
  BLE.setEventHandler(BLEDisconnected, onBLEDisconnected);

  BLE.advertise();
  bleOK = true;
  Serial.println("BLE ready: EMG-Cuadriceps");
}


void onBLEConnected(BLEDevice central) {
  Serial.print("BLE connected: ");
  Serial.println(central.address());
  BLE.advertise();   // stay visible for a second device (robot or page)
}

void onBLEDisconnected(BLEDevice central) {
  Serial.print("BLE disconnected: ");
  Serial.println(central.address());
  BLE.advertise();
}


// Sends one data packet if a device is connected, and reads commands
void sendBLE(float rms, byte state) {

  if (!bleOK) return;

  BLE.poll();

  if (cmdChar.written()) {
    byte cmd = cmdChar.value();
    if (cmd == 'C') recalibrateRequested = true;
  }

  if (!BLE.connected()) return;

  byte packet[17];
  uint32_t t = millis();

  memcpy(packet + 0, &t, 4);
  memcpy(packet + 4, &rms, 4);
  memcpy(packet + 8, &restLimit, 4);
  memcpy(packet + 12, &thresholdRMS, 4);
  packet[16] = state;

  dataChar.writeValue(packet, 17);
}


// Like delay(), but keeps Bluetooth alive while waiting
void waitWithBLE(unsigned long ms) {

  unsigned long start = millis();

  while (millis() - start < ms) {
    if (bleOK) BLE.poll();
    delay(10);
  }
}


// [LCD] ===== LCD FUNCTIONS =====

// Writes two lines, padding with spaces so old text is erased without flicker
void lcdShow(const char* line1, const char* line2) {

  lcd.setCursor(0, 0);
  lcd.print(line1);
  for (int i = strlen(line1); i < 16; i++) lcd.print(' ');

  lcd.setCursor(0, 1);
  lcd.print(line2);
  for (int i = strlen(line2); i < 16; i++) lcd.print(' ');
}


// Shows a countdown on the second line (e.g. 3, 2, 1)
void lcdCountdown(const char* message, int seconds) {

  char buffer[17];

  for (int s = seconds; s > 0; s--) {
    snprintf(buffer, sizeof(buffer), "      %d...", s);
    lcdShow(message, buffer);
    waitWithBLE(1000);
  }
}


// Only redraws the screen when the state changes, so the RMS timing is not slowed down
void lcdUpdateState(int newState) {

  if (newState == lcdState) return;
  lcdState = newState;

  if (newState == 0) {
    lcdShow("Relajado", "Contraiga muslo");
  }
  else if (newState == 1) {
    lcdShow("Actividad...", "Apriete mas!");
  }
  else {
    lcdShow("CONTRACCION", "VALIDA!");
  }
}
