#include <RH_ASK.h>
#include <SPI.h>
#include <Servo.h>

// Pines para los relays
const int relayPins[] = {13, 12, 14, 27, 26, 25, 33, 32};
const int numRelays = sizeof(relayPins) / sizeof(relayPins[0]);

// Pines para los sensores
int pinVeleta = 34;      // Pin para la veleta (dirección del viento)
int pinAnemometro = 35;  // Pin para el anemómetro (velocidad del viento)

Servo servo_flow;  // Servo motor

// Configuración del módulo RF 433 MHz
RH_ASK rf_driver(2000, 21, 22);  // 21: pin de datos, 22: pin de transmisión

// Variables para manejar el tiempo de activación del relay
bool pirDetected = false;
unsigned long pirActivationTime = 0;
const unsigned long relayOnDuration =
    5 * 60 * 1000;  // 5 minutos en milisegundos

void setup() {
  Serial.begin(115200);
  delay(1500);

  // Configuración de los pines de los relays como salida
  for (int i = 0; i < numRelays; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);  // Relays en estado inactivo
  }

  // Configuración del pin del servo motor
  servo_flow.attach(22);

  // Inicialización del módulo RF 433 MHz
  if (!rf_driver.init()) {
    Serial.println("RF init failed");
    while (1) delay(1000);
  } else {
    Serial.println("RF init successful");
  }
}

void loop() {
  // Leer los valores de los sensores
  int veleta = analogRead(pinVeleta);  // Lee el valor de la veleta en el pin 34
  int anemometro =
      analogRead(pinAnemometro);  // Lee el valor del anemómetro en el pin 35

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

  digitalWrite(relayPin, LOW);  // Activa el relay
  delay(map(anemometro, 0, 30, 0,
            3000));              // Control del tiempo de activación del relay
  digitalWrite(relayPin, HIGH);  // Desactiva el relay

  // Control del servo motor basado en los datos del anemómetro
  servo_flow.write(anemometro_lima2);

  // Esperar señal desde el módulo RF 433 MHz
  uint8_t buf[20] = {0};  // Buffer para almacenar la señal recibida
  uint8_t buflen = sizeof(buf);

  if (rf_driver.recv(buf, &buflen)) {
    // Mensaje recibido con checksum válido
    Serial.print("Mensaje Recibido: ");
    Serial.println((char*)buf);

    // Si el mensaje indica que el PIR detectó movimiento
    if (strcmp((char*)buf, "PIR_DETECTED") == 0) {
      pirDetected = true;
      pirActivationTime = millis();     // Registra el tiempo de activación
      digitalWrite(relayPins[0], LOW);  // Activa el relay en el pin 13
    }
  }

  // Verificar si han pasado 5 minutos desde la activación del relay por el PIR
  if (pirDetected && (millis() - pirActivationTime >= relayOnDuration)) {
    digitalWrite(relayPins[0], HIGH);  // Desactiva el relay en el pin 13
    pirDetected = false;               // Resetea la bandera
  }

  delay(1000);  // Espera antes de la próxima actualización
}
