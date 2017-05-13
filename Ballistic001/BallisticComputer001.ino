/*
 Name:		BallisticComputer001.ino
 Created:	4/2/2017 12:08:21 PM
 Author:	Shaun Stewart
*/

// the setup function runs once when you press reset or power the board
#include <dht.h>
#include <SFE_BMP180.h>
#include <U8x8lib.h>
#include <U8g2lib.h>

U8G2_PCD8544_84X48_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  // Nokia 5110 Display
																													  //Humiture Declarations
dht DHT;
#define DHT11_PIN 6
//

//BMP 180 Declarations
SFE_BMP180 pressure;
//

char dataBuffer[200];

void setup(void) {
	pressure.begin();
	u8g2.begin();
	Serial.begin(9600);
}

void loop(void) {
	bool err = false;
	char status;
	double Temp, Pres, p0, a;
	char tempBuffer[7];
	char presBuffer[8];
	char humBuffer[7];

	u8g2.clearBuffer();					// clear the internal memory
	u8g2.setFont(u8g2_font_6x10_tr);	// choose a suitable font

	int chk = DHT.read11(DHT11_PIN);
	//Serial.write(chk + "\n");
	switch (chk)
	{
		case DHTLIB_OK:
		//_comms.print("OK,\t");
		break;
		case DHTLIB_ERROR_CHECKSUM:
			strcpy(dataBuffer, "TH Checksum");
			err = true;
			break;
		case DHTLIB_ERROR_TIMEOUT:
			strcpy(dataBuffer, "TH Time out");
			err = true;
			break;
		default:
			strcpy(dataBuffer, "TH Unknown");
			err = true;
			break;
	}

	if (!err) {

		status = pressure.startTemperature();
		if (status != 0)
		{
			delay(status);
			status = pressure.getTemperature(Temp);
			if (status != 0)
			{
				status = pressure.startPressure(3);
				if (status != 0)
				{
					// Wait for the measurement to complete:
					delay(status);
					// Retrieve the completed pressure measurement:
					// Note that the measurement is stored in the variable P.
					// Note also that the function requires the previous temperature measurement (T).
					// (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
					// Function returns 1 if successful, 0 if failure.
					status = pressure.getPressure(Pres, Temp);
					if (status != 0)
					{
						dtostrf(Pres, 7, 2, presBuffer);
					}
					else {
						strcpy(dataBuffer, "BMP P meas");
						err = true;
					}
				}
				else {
					strcpy(dataBuffer, "BMP start P meas");
					err = true;
				}
			}
			else {
				strcpy(dataBuffer, "BMP T meas");
				err = true;
			}
		}
		else {
			strcpy(dataBuffer, "BMP start T meas");
			err = true;
		}
	}

	if (!err) {


		dtostrf(DHT.temperature, 6, 2, tempBuffer);
		dtostrf(DHT.humidity, 6, 2, humBuffer);

		u8g2.drawStr(40, 10, presBuffer);	// write something to the internal memory
		u8g2.drawStr(46, 25, humBuffer);	// write something to the internal memory
		u8g2.drawStr(46, 40, tempBuffer);	// write something to the internal memory

		u8g2.drawStr(1, 10, "Baro");	// write something to the internal memory
		u8g2.drawStr(1, 25, "Rh %");	// write something to the internal memory
		u8g2.drawStr(1, 40, "Temp");	// write something to the internal memory

		u8g2.drawFrame(37, 0, 47, 14);
		u8g2.drawFrame(37, 15, 47, 14);
		u8g2.drawFrame(37, 30, 47, 14);
	}
	else {
		u8g2.drawStr(0, 10, dataBuffer);	// write something to the internal memory
	}
	u8g2.sendBuffer();	// transfer internal memory to the display
	delay(1000);
}