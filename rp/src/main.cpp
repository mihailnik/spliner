/*
 * Project: Si4463 Radio Library for AVR and Arduino (Ping server example)
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2017 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/si4463-radio-library-avr-arduino/
 */

/*
 * Ping server
 *
 * Listen for packets and send them back
 */

#include <main.h>

#define CHANNEL 20
#define MAX_PACKET_SIZE 10

#define PACKET_NONE		0
#define PACKET_OK		1
#define PACKET_INVALID	2

#define RP_NUM	1		// номер привода

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

typedef struct{
	uint8_t ready;
	int16_t rssi;
	uint8_t length;
	uint8_t buffer[MAX_PACKET_SIZE];
} pingInfo_t;

static volatile pingInfo_t pingInfo;

void SI446X_CB_RXCOMPLETE(uint8_t length, int16_t rssi)
{
	if(length > MAX_PACKET_SIZE)
		length = MAX_PACKET_SIZE;

	pingInfo.ready = PACKET_OK;
	pingInfo.rssi = rssi;
	pingInfo.length = length;

	Si446x_read((uint8_t*)pingInfo.buffer, length);

	// Radio will now be in idle mode
}

void SI446X_CB_RXINVALID(int16_t rssi)
{
	pingInfo.ready = PACKET_INVALID;
	pingInfo.rssi = rssi;
}

void setup()
{
	Serial.begin(115200);
	 myservo.attach(9);  // attaches the servo on pin 9 to the servo object
	pinMode(A5, OUTPUT); // LED

	// Start up
	Si446x_init();
	Si446x_setTxPower(SI446X_MAX_TX_POWER);
}

void loop()
{
	static uint32_t pings;
	static uint32_t invalids;

	// Put into receive mode
	Si446x_RX(CHANNEL);

	Serial.println(F("Wait ping."));

	// Wait for data
	while(pingInfo.ready == PACKET_NONE);
		
	if(pingInfo.ready != PACKET_OK)
	{
		invalids++;
		pingInfo.ready = PACKET_NONE;
		Serial.print(F("Invalid packet! Signal: "));
		Serial.print(pingInfo.rssi);
		Serial.println(F("dBm"));
		Si446x_RX(CHANNEL);
	}
	else
	{
		pings++;
		pingInfo.ready = PACKET_NONE;

		// Toggle LED
		static uint8_t ledState;
		
		static uint8_t servoState=0;

		digitalWrite(A5, ledState ? HIGH : LOW);
		ledState = !ledState;
		switch (pingInfo.buffer[RP_NUM])
		{
			case RP_FIER:
			Serial.println(F("FIER 1!"));
			myservo.write(180);              // tell servo to go to position in variable 'pos'
			break;
			case RP_NOP:
			Serial.println(F("NOP 1"));
			myservo.write(0);              // tell servo to go to position in variable 'pos'
			break;

			default:
			break;
		}


//		Serial.print(F("Signal strength: "));
		Serial.print(pingInfo.rssi);
		Serial.println(F("dBm"));

		// Print out ping contents
		Serial.print(F("Data: "));
		Serial.write((uint8_t*)pingInfo.buffer, sizeof(pingInfo.buffer));
		Serial.println();
	}

	// Serial.print(F("Totals: "));
	// Serial.print(pings);
	// Serial.print(F("Pings, "));
	// Serial.print(invalids);
	// Serial.println(F("Invalid"));
	// Serial.println(F("------"));
}
