/*
 * Project: Si4463 Radio Library for AVR and Arduino (Ping client example)
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2017 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/si4463-radio-library-avr-arduino/
 */

/*
 * Ping client
 *
 * Time how long it takes to send some data and get a reply
 * Should be around 5-6ms with default settings
 */

#include <main.h>

#define CHANNEL 20
#define MAX_PACKET_SIZE 10
#define TIMEOUT 1000

#define PACKET_NONE		0
#define PACKET_OK		1
#define PACKET_INVALID	2

#define LED_PIN			6

static char data[MAX_PACKET_SIZE] = {0};

static uint32_t replies;
static uint32_t timeouts;
static uint32_t invalids;

const byte ROWS = 4; //four rows
const byte COLS = 3; //fcolumns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A4, A5, 7}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

char swich_key_control(void){
char key = 0;
static	char	customKey;

	customKey = customKeypad.getKey();
	// обнуляем данные
	if (customKey)
	{	
		for (uint8_t i = 0; i < MAX_PACKET_SIZE; i++){	data[i] = 0;} // ОБНУЛЯЕМ БУФЕР
		switch (customKey)
		{
			case '#':
			for (uint8_t i = 0; i < MAX_PACKET_SIZE; i++){	data[i] = RP_SLEP;} /// усыпляем
			key = '#';
			break;
			case '0':
			for (uint8_t i = 0; i < MAX_PACKET_SIZE; i++){	data[i] = RP_WEKUP;} // просыпаемся
			key = '0';
			break;
			case '1':
			data[1] = RP_FIER;
			key = '1';
			break;
			case '2':
			data[2] = RP_FIER;
			key = '2';
			break;
			case '3':
			data[3] = RP_FIER;
			key = '3';
			break;
			case '4':
			data[4] = RP_FIER;
			key = '4';
			break;
			case '5':
			data[5] = RP_FIER;
			key = '5';
			break;
			case '6':
			data[6] = RP_FIER;
			key = '6';
			break;
			case '7':
			data[7] = RP_FIER;
			key = '7';
			break;
			case '8':
			data[8] = RP_FIER;
			key = '8';
			break;
			case '9':
			data[9] = RP_FIER;
			key = '9';
			break;
	
			default:
			break;
		}
		Serial.println(customKey);
	}
return key;
}


typedef struct{
	uint8_t ready;
	uint32_t timestamp;
	int16_t rssi;
	uint8_t length;
	uint8_t buffer[MAX_PACKET_SIZE];
} pingInfo_t;

static volatile pingInfo_t pingInfo;



void setup()
{
	Serial.begin(115200);

	pinMode(LED_PIN, OUTPUT); // LED 

	// Start up
	Si446x_init();
	Si446x_setTxPower(SI446X_MAX_TX_POWER);
}

void loop()
{
char key_ret = 0;
	key_ret = swich_key_control();
	if (key_ret != 0)
	{
		if (key_ret == '0') //пробуждаем раз шлем сигнал пробуждения каждые 2сек (30сек)
		{
			for (size_t i = 0; i < 15 ; i++)
			{
				static uint8_t ledState;

				digitalWrite(LED_PIN, ledState ? HIGH : LOW);
				ledState = !ledState;
				RadioTX(SI446X_STATE_SLEEP);
				Si446x_init();
				Si446x_setTxPower(SI446X_MAX_TX_POWER);
				delay(1950);
			}
		}
		else
		{
				Si446x_init();
				Si446x_setTxPower(SI446X_MAX_TX_POWER);
 				RadioTX(SI446X_STATE_SLEEP);
		}
		
	}

	
	// Make data

	
	// sprintf_P(data, PSTR("test %hhu"), counter);
	// counter++;
	
	// Serial.print(F("Sending data"));
	// Serial.println(data);

	delay(10);	
}

// шлем пакет
uint8_t RadioTX(si446x_state_t state_after_tx)
{
	uint8_t reply_state = 0;
	// Send the data
	Si446x_TX(data, sizeof(data), CHANNEL, state_after_tx);
	//ждем или нет ответа
	if (state_after_tx == SI446X_STATE_RX)
	{
//prt		Serial.println(F("Data sent, waiting for reply..."));
	
		uint8_t success;

		// Wait for reply with timeout
		uint32_t sendStartTime = millis();
		while(1)
		{
			success = pingInfo.ready;
			if(success != PACKET_NONE)
				break;
			else if(millis() - sendStartTime > TIMEOUT) // Timeout // TODO typecast to uint16_t
				break;
		}
		
		pingInfo.ready = PACKET_NONE;

		if(success == PACKET_NONE)
		{
//prt			Serial.println(F("Ping timed out"));
			timeouts++;
		}
		else if(success == PACKET_INVALID)
		{
//prt			Serial.print(F("Invalid packet! Signal: "));
//prt			Serial.print(pingInfo.rssi);
//prt			Serial.println(F("dBm"));
			invalids++;
		}
		else
		{
			static uint8_t ledState;
			digitalWrite(LED_PIN, ledState ? HIGH : LOW);
			ledState = !ledState;

			replies++;

//prt			Serial.print(F("Signal strength: "));
//prt			Serial.print(pingInfo.rssi);
//prt			Serial.println(F("dBm"));

			// Print out ping contents
//prt			Serial.print(F("Data from server: "));
//prt			Serial.write((uint8_t*)pingInfo.buffer, sizeof(pingInfo.buffer));
//prt			Serial.println();
		}
	}
	return reply_state;
}

void SI446X_CB_RXCOMPLETE(uint8_t length, int16_t rssi)
{
	if(length > MAX_PACKET_SIZE)
		length = MAX_PACKET_SIZE;

	pingInfo.ready = PACKET_OK;
	pingInfo.timestamp = millis();
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