#include <VirtualWire.h>

const int pirPin = 2;
const int txPin = 3;

void setup() {
  pinMode(pirPin, INPUT);
  vw_set_tx_pin(txPin);
  vw_setup(2000);  // Bits per second
}

void loop() {
  int pirState = digitalRead(pirPin);
  if (pirState == HIGH) {
    const char *msg = "ON";
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx();  // Wait until the whole message is gone
  }
  delay(1000);
}
