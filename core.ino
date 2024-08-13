#include "arduino_secrets.h"
#include "thingProperties.h"
#include <Servo.h>

// Pines para los relays
const int relayPins[] = {13, 12, 14, 27, 26, 25, 33, 32};
const int numRelays = sizeof(relayPins) / sizeof(relayPins[0]);

// Pines para los sensores
int pinVeleta = 34;      // Nuevo pin para la veleta (dirección del viento)
int pinAnemometro = 35;  // Nuevo pin para el anemómetro (velocidad del viento)

Servo servo_flow;  // Servo motor

void setup() {
  Serial.begin(9600);
  delay(1500);

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Configuración de los pines de los relays como salida
  for (int i = 0; i < numRelays; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // Relays en estado inactivo
  }

  // Configuración del pin del servo motor
  servo_flow.attach(22);
}

void loop() {
  ArduinoCloud.update();

  // Leer los valores de los sensores
  int veleta = analogRead(pinVeleta);      // Lee el valor de la veleta en el pin 34
  int anemometro = analogRead(pinAnemometro);  // Lee el valor del anemómetro en el pin 35

  // Mapear los valores de los sensores
  veleta = map(veleta, 0, 4095, 0, 360);
  anemometro = map(anemometro, 0, 4095, 0, 30);

  long sectors = 4;
  long M = numRelays * sectors;
  long veleta_lima2 = map(veleta, 0, 360, 0, M);
  int anemometro_lima2 = map(anemometro, 0, 30, 0, 90);

  // Control del relay basado en los datos de la veleta
  int relayIndex = veleta_lima2 % numRelays;
  int relayPin = relayPins[relayIndex];

  digitalWrite(relayPin, LOW); // Activa el relay
  delay(map(anemometro, 0, 30, 0, 3000)); // Control del tiempo de activación del relay
  digitalWrite(relayPin, HIGH); // Desactiva el relay

  // Control del servo motor basado en los datos del anemómetro
  servo_flow.write(anemometro_lima2);

  delay(1000); // Espera antes de la próxima actualización
}

void onVeletaLimaChange() {}
void onAnemometroLimaChange() {}
