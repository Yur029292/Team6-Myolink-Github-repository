#include <Servo.h>
#include <ArduinoBLE.h>   // [BLE] replaces WiFiS3 + WiFiUdp. Library: "ArduinoBLE" by Arduino

// =====================================================
// BLUETOOTH SETTINGS (must match the EMG Arduino)
// =====================================================
const char* EMG_SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
const char* EMG_DATA_UUID    = "6e400003-b5a3-f393-e0a9-e50e24dcca9e";

BLEDevice emg;                    // the EMG Arduino once connected
BLECharacteristic emgData;        // its data characteristic (17 bytes, state in byte 16)

bool bleConectado = false;
unsigned long ultimoDato = 0;

// If no data arrives for this long, assume no contraction (safety)
const unsigned long TIMEOUT_DATOS = 1500;

bool Contraccion = false;

// SERVOS
Servo servo1;
Servo servo4;
Servo servo5;
Servo servo6;

// PINS
const int PIN_SERVO1 = 3;
const int PIN_SERVO4 = 9;
const int PIN_SERVO5 = 10;
const int PIN_SERVO6 = 11;

const int PIN_BOTON = 2;


// SYSTEM STATE

bool objetoAgarrado = false;

bool estadoAnteriorBoton = HIGH;

unsigned long ultimoCambio = 0;

const unsigned long debounce = 200;

bool contraccionActiva = false;

int posicionActual = 0;


// Vector position 
int direccion = 1; // 1 = forward, -1 = backward

const int posicionesServo6[4] = {20, 62, 104, 146};


// BLUETOOTH FUNCTIONS (replace the WiFi/UDP functions)
void startBLE()
{
  if (!BLE.begin())
  {
    Serial.println("BLE failed to start - check the board firmware");
    while (true) {}
  }

  Serial.println("Searching for EMG-Cuadriceps...");
  BLE.scan();
}

// Is this the EMG? (checks the service UUID or the name)
bool esElEMG(BLEDevice d)
{
  if (d.hasAdvertisedServiceUuid())
  {
    for (int i = 0; i < d.advertisedServiceUuidCount(); i++)
    {
      if (d.advertisedServiceUuid(i).equalsIgnoreCase(EMG_SERVICE_UUID)) return true;
    }
  }
  return d.localName().startsWith("EMG");
}

bool connectEMG(BLEDevice d)
{
  Serial.print("Connecting to ");
  Serial.println(d.address());

  if (!d.connect())
  {
    Serial.println("Connection failed");
    return false;
  }

  if (!d.discoverAttributes())
  {
    Serial.println("Could not read the EMG services");
    d.disconnect();
    return false;
  }

  emgData = d.characteristic(EMG_DATA_UUID);

  if (!emgData || !emgData.canSubscribe() || !emgData.subscribe())
  {
    Serial.println("EMG data not found");
    d.disconnect();
    return false;
  }

  emg = d;
  bleConectado = true;
  ultimoDato = millis();

  Serial.println("Connected to EMG!");
  return true;
}

// Replaces readUDP(): updates Contraccion from the EMG state
void readBLE()
{
  BLE.poll();

  // Not connected yet: look for the EMG
  if (!bleConectado)
  {
    BLEDevice d = BLE.available();

    if (d && esElEMG(d))
    {
      BLE.stopScan();

      if (!connectEMG(d))
      {
        BLE.scan();   // try again
      }
    }

    Contraccion = false;
    return;
  }

  // Connection lost: search again
  if (!emg.connected())
  {
    Serial.println("EMG disconnected - searching again...");
    bleConectado = false;
    Contraccion = false;
    BLE.scan();
    return;
  }

  // New data from the EMG
  if (emgData.valueUpdated())
  {
    uint8_t packet[17];
    int n = emgData.readValue(packet, 17);

    if (n >= 17)
    {
      // State: 0 rest, 1 activity, 2 valid contraction, 3 calibrating
      Contraccion = (packet[16] == 2);
      ultimoDato = millis();
    }
  }

  // Safety: no recent data -> no contraction
  if (millis() - ultimoDato > TIMEOUT_DATOS)
  {
    Contraccion = false;
  }
}


// POSITION FUNCTIONS
void posicionEncorvada()
{
  servo4.write(180);
  servo5.write(115);
}

void posicionAgarre()
{
  servo4.write(180);
  servo5.write(55);
}


// =====================================================
// SETUP
// =====================================================
void setup()
{
  Serial.begin(115200);

  startBLE();   // [BLE] replaces connectWiFi() and udp.begin()

  pinMode(PIN_BOTON, INPUT_PULLUP);

  servo1.attach(PIN_SERVO1);
  servo4.attach(PIN_SERVO4);
  servo5.attach(PIN_SERVO5);
  servo6.attach(PIN_SERVO6);

  servo1.write(90);
  servo4.write(70);
  servo5.write(90);
  servo6.write(posicionesServo6[0]);

  Serial.println("BLE Receiver Ready");

  delay(1000);
}


// =====================================================
// LOOP
// =====================================================
void loop()
{
  // Receive wireless contraction state
  readBLE();   // [BLE] replaces readUDP()

  // SERVO 6 CONTROL
  if (Contraccion == true &&
      contraccionActiva == false)
  {
    contraccionActiva = true;

    posicionActual =
      posicionActual + direccion;

    if (posicionActual >= 3)
    {
      posicionActual = 3;
      direccion = -1;
    }
    else if (posicionActual <= 0)
    {
      posicionActual = 0;
      direccion = 1;
    }

    servo6.write(
      posicionesServo6[posicionActual]
    );
  }

  if (Contraccion == false)
  {
    contraccionActiva = false;
  }


  // BUTTON CONTROL
  bool estadoBoton =
    digitalRead(PIN_BOTON);

  if (estadoAnteriorBoton == HIGH &&
      estadoBoton == LOW &&
      millis() - ultimoCambio > debounce)
  {
    ultimoCambio = millis();


    // PICK UP OBJECT
    if (objetoAgarrado == false)
    {
      delay(3000);

      posicionAgarre();

      delay(1000);

      servo1.write(145);

      objetoAgarrado = true;

      delay(1000);

      posicionEncorvada();

      delay(1500);
    }


    // RELEASE OBJECT
    else
    {
      posicionAgarre();

      delay(1000);

      servo1.write(90);

      objetoAgarrado = false;

      delay(1000);

      posicionEncorvada();

      delay(1500);
    }
  }

  estadoAnteriorBoton =
    estadoBoton;

  delay(15);
}
