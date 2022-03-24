#include <Arduino.h>

#define LED_PINE	22		// номер привода
static uint8_t ledState = 0;

void setup() {
    // put your setup code here, to run once:
    digitalWrite(LED_PINE, LOW);
}

void loop() {
  		// Toggle LED
		digitalWrite(LED_PINE,ledState? LOW : HIGH);
		ledState = !ledState;
  // put your main code here, to run repeatedly:
}