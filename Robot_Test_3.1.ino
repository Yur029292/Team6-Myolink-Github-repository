//This code is for testing the emg conection via Wifi, wireless from the arduino master

#include <Servo.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>

// =====================================================
// WIFI SETTINGS
// =====================================================

char ssid[] = "Mariana";
char pass[] = "mar210106";

WiFiUDP udp;

const int UDP_PORT = 4210;

char incomingPacket[10];

bool Contraccion = false;

// =====================================================
// SERVOS
// =====================================================

Servo servo1;
Servo servo2;
Servo servo4;
Servo servo5;
Servo servo6;

// =====================================================
// PINS
// =====================================================

const int PIN_SERVO1 = 3;
const int PIN_SERVO2 = 5;
const int PIN_SERVO4 = 9;
const int PIN_SERVO5 = 10;
const int PIN_SERVO6 = 11;

const int PIN_BOTON = 2;

// =====================================================
// SYSTEM STATE
// =====================================================

bool objetoAgarrado = false;

bool estadoAnteriorBoton = HIGH;

unsigned long ultimoCambio = 0;

const unsigned long debounce = 200;

bool contraccionActiva = false;

int posicionActual = 0;

// 1 = forward
// -1 = backward

int direccion = 1;

const int posicionesServo6[4] = {
  20, 62, 104, 146
};

// =====================================================
// WIFI FUNCTIONS
// =====================================================

void connectWiFi()
{
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED)
{
    Serial.println("Attempting WiFi...");

    WiFi.begin(ssid, pass);

    delay(5000);

    Serial.print("Status = ");
    Serial.println(WiFi.status());
}

  Serial.println();
  Serial.println("Connected!");

  Serial.print("Receiver IP Address: ");
  Serial.println(WiFi.localIP());
}

void readUDP()
{
  int packetSize = udp.parsePacket();

  if (packetSize)
  {
    int len =
      udp.read(
        incomingPacket,
        sizeof(incomingPacket) - 1
      );

    if (len > 0)
    {
      incomingPacket[len] = '\0';
    }

    if (incomingPacket[0] == '1')
    {
      Contraccion = true;
    }
    else
    {
      Contraccion = false;
    }
  }
}

// =====================================================
// POSITION FUNCTIONS
// =====================================================

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

  connectWiFi();

  udp.begin(UDP_PORT);

  pinMode(PIN_BOTON, INPUT_PULLUP);

  servo1.attach(PIN_SERVO1);
  servo2.attach(PIN_SERVO2);
  servo4.attach(PIN_SERVO4);
  servo5.attach(PIN_SERVO5);
  servo6.attach(PIN_SERVO6);

  servo1.write(90);
  servo2.write(35);
  servo4.write(70);
  servo5.write(90);
  servo6.write(posicionesServo6[0]);

  Serial.println("UDP Receiver Ready");

  delay(1000);
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
  // Receive wireless contraction state

  readUDP();

  // ===================================================
  // SERVO 6 CONTROL
  // ===================================================

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

  // ===================================================
  // BUTTON CONTROL
  // ===================================================

  bool estadoBoton =
    digitalRead(PIN_BOTON);

  if (estadoAnteriorBoton == HIGH &&
      estadoBoton == LOW &&
      millis() - ultimoCambio > debounce)
  {
    ultimoCambio = millis();

    // ===============================================
    // PICK UP OBJECT
    // ===============================================

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

    // ===============================================
    // RELEASE OBJECT
    // ===============================================

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