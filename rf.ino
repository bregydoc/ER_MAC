#include <RH_ASK.h>
#include <SPI.h>  // Dependencia para la librería RadioHead

// Pines
const int pirPin = 2;  // Pin de entrada del sensor PIR
const int txPin = 3;   // Pin de transmisión para el módulo RF 433 MHz

// Crear un objeto RH_ASK para manejar la comunicación
RH_ASK rf_driver(
    2000, 255, txPin);  // 2000 bps, sin pin de recepción, pin de transmisión 3

void setup() {
  pinMode(pirPin, INPUT);  // Configura el pin del PIR como entrada

  // Inicializar el módulo RF 433 MHz
  if (!rf_driver.init()) {
    Serial.println("RF init failed");
    while (1) delay(1000);  // Si la inicialización falla, permanece en un bucle
  }
}

void loop() {
  int pirState = digitalRead(pirPin);  // Lee el estado del PIR

  // Si el PIR detecta movimiento, envía un mensaje
  if (pirState == HIGH) {
    const char *msg = "PIR_DETECTED";
    rf_driver.send((uint8_t *)msg, strlen(msg));
    rf_driver
        .waitPacketSent();  // Espera hasta que el mensaje haya sido enviado
  }

  delay(1000);  // Espera un segundo antes de verificar nuevamente
}
