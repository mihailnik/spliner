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
#define TIMEOUT 1000

#define PACKET_NONE		0
#define PACKET_OK		1
#define PACKET_INVALID	2

#define RP_NUM	3		// номер привода
#define LED_PINE	A5		// номер привода

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
static uint8_t ledState = 0;
uint8_t slp = 0;    

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

void RP_SLEEP(void)
{
	digitalWrite(LED_PINE, LOW);
	slp = 1;
	Si446x_sleep();
	delay(50);
	power.sleep(SLEEP_8192MS); // спим ~ 8 секунды (некалиброванный таймаут. Смотри пример с калибрвокой!)
	power.sleep(SLEEP_8192MS); // спим ~ 8 секунды (некалиброванный таймаут. Смотри пример с калибрвокой!)
//	power.sleep(SLEEP_8192MS); // спим ~ 8 секунды (некалиброванный таймаут. Смотри пример с калибрвокой!)
//	power.sleep(SLEEP_8192MS); // спим ~ 8 секунды (некалиброванный таймаут. Смотри пример с калибрвокой!)
//	power.sleep(SLEEP_8192MS); // спим ~ 8 секунды (некалиброванный таймаут. Смотри пример с калибрвокой!)
//	power.sleep(SLEEP_8192MS); // спим ~ 8 секунды (некалиброванный таймаут. Смотри пример с калибрвокой!)
		// Start up
	digitalWrite(LED_PINE, HIGH);
	delay(1000);
	slp = 0;
}

void RX_CMD_EXEC(void)
{
		pingInfo.ready = PACKET_NONE;



		switch (pingInfo.buffer[RP_NUM])
		{
			case RP_FIER:
			Serial.println(F("FIER 1!"));
			myservo.write(0);              // tell servo to go to position in variable 'pos'
			break;
			case RP_NOP:
			Serial.println(F("NOP 1"));
			myservo.write(180);              // tell servo to go to position in variable 'pos'
			break;
			case RP_SLEP:

			Serial.println(F("SLEEP"));
			RP_SLEEP();

			break;
			default:
			break;
		}
}

void setup()
{
//	Serial.begin(115200);
//	 myservo.attach(9);  // attaches the servo on pin 9 to the servo object

	// Start up
//	Si446x_init();
//	Si446x_setTxPower(SI446X_MAX_TX_POWER);
//	myservo.write(180);

	power.setSleepMode(POWERDOWN_SLEEP); // режим сна (по умолчанию POWERDOWN_SLEEP)
	pinMode(LED_PINE, OUTPUT); // LED
}

void loop()
{

while (1)
{
//		Si446x_sleep();
		power.sleepDelay(2000);
		digitalWrite(LED_PINE, HIGH);
		ledState = !ledState;
		delay(2000);
		digitalWrite(LED_PINE, LOW);
		ledState = !ledState;
}

	// Put into receive mode
	Si446x_RX(CHANNEL);
	Serial.println(F("Wait ping."));

	if (slp == 1)// sleep mode
	{
			// Wait for reply with timeout
		uint8_t success;
		uint32_t sendStartTime = millis();
		while(1)// просыпаемся на время что бы послушать
		{
			success = pingInfo.ready;
			if(success != PACKET_NONE)
				break;
			else if(millis() - sendStartTime > TIMEOUT) // Timeout // TODO typecast to uint16_t
				break;
		}
		if (success == PACKET_NONE) //если нет команд то снова спим
		{
			RP_SLEEP();
		}
		else if (success == PACKET_INVALID)
		{
			slp = 0;
			pingInfo.ready = PACKET_NONE;
		}
		else if (success == PACKET_OK)
		{
			slp = 0;
			pingInfo.ready = PACKET_NONE;
			RX_CMD_EXEC();
		}
		
	}
	else // not sleep mode
	{
		while (pingInfo.ready == PACKET_NONE)
		{
			; //ждем пакет /* code */
		}
		
		uint8_t success;
		success = pingInfo.ready;
		if (success == PACKET_INVALID)
		{
			pingInfo.ready = PACKET_NONE;
			Serial.println(F("P invalid"));
		}
		else
		{
			pingInfo.ready = PACKET_NONE;
			RX_CMD_EXEC();
		}
	}

//		Serial.print(F("Data: "));
//		Serial.write((uint8_t*)pingInfo.buffer, sizeof(pingInfo.buffer));
//		Serial.println();

//		Serial.print(F("Signal strength: "));
//		Serial.print(pingInfo.rssi);
//		Serial.println(F("dBm"));

		// Print out ping contents
}
