/*********************************************************************
 IoT Project 3  - Smart Home
 
 This sketch is written to accompany IoT Project 3 
 with Arduino UNO Starter Kit & Adafruit cc3000 WiFi shield
 
 Created 20 Mar 2016
 by Zhong Zheng
*********************************************************************/

#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <MemoryFree.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif
#include "thingspeak.h"

extern Adafruit_CC3000 cc3000;
extern void wifi_init();

// LED & light sensor
#define	LED_PIN		9
#define	LIGHT_PIN	A0
#define TEMP_PIN A1
#define piezzoPin  6

/**************************************************************************/
/*!
    @brief  Sets up the HW and the CC3000 module (called automatically
            on startup)
*/
/**************************************************************************/
volatile int ledState = 1;
int Lightlow = 2000;
int Lighthigh = 0;

void setup(void)
{
  	Serial.begin(115200);
  	Serial.print("Free RAM: "); Serial.println(freeMemory(), DEC);
	// make LED pin an output
	pinMode(LED_PIN, OUTPUT);
	pinMode(LIGHT_PIN, INPUT);
	pinMode(TEMP_PIN, INPUT);
	attachInterrupt(0, button_Switch_Pushed, FALLING);
	//initial wifi
	wifi_init();
	
	ThingSpeak.init(&cc3000, "api.thingspeak.com", 80, "HZNJU9K0125EODIL", 101741, "BULKXA8YNEG522OR", 7546);

}


void loop(void)
{
    double temp;            
	int LightValue = analogRead(LIGHT_PIN);
    int TempValue =  analogRead(TEMP_PIN);
    temp=Thermister(TempValue);

	char *cmd=ThingSpeak.commandExecute();
	if (strlen(cmd)!=0) {
		Serial.print("Got command to execute: "); Serial.println(cmd);
		if (strcmp(cmd, "on") == 0) ledState = 1;
		else if (strcmp(cmd, "off")== 0) ledState = 0;
	}
// Temperature >=100 then buzzer will buzz and light will be turned on and blink for warning
    if (temp >= 100) {
    	    tone(piezzoPin, 500, 2000);
		    //light will blink and ledState will be set to 1
		    int i = 0;	
		    while (i < 5){
    	    digitalWrite(LED_PIN, HIGH);   
  			delay(200);               
			digitalWrite(LED_PIN, LOW);    
  			delay(200);   
  			i++;
		    }
  			
    	  	ledState = 1;
    }

// LED light on/off
    digitalWrite(LED_PIN, ledState); 
    
  	ThingSpeak.channelInit();
	ThingSpeak.channelSetField("field1", LightValue);
	ThingSpeak.channelSetField("field2", temp);
	if (ledState == 0) 	ThingSpeak.channelSetField("field3", "off");
	else if (ledState == 1) ThingSpeak.channelSetField("field3", "on");
	ThingSpeak.channelUpdate();
		
	Serial.print("LightValue: "); Serial.println(LightValue);
	Serial.print("Temperature : "); Serial.println(temp);
	Serial.print("ledState  : "); Serial.println(ledState);
	
	if (temp >= 100) {
	  	Serial.println("Warning: House on Fire! ");
	   	return;
	}
	else if (temp < 100) delay(5000);
}

// button_swtich_Pushed interrupt function
void button_Switch_Pushed() {
	static long lastTime = 0;
	
	if (millis() - lastTime < 200)return;
	lastTime = millis();

	Serial.println("LED on/off changed from");Serial.print(ledState);
  	ledState = !ledState;
  	Serial.print(" to ");Serial.println(ledState);
  	digitalWrite(LED_PIN, ledState);
  	}

double Thermister(int RawADC) {  //Function to perform the fancy math of the Steinhart-Hart equation
 double Temp;
 Temp = log(((10240000/RawADC) - 10000));
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15;              // Convert Kelvin to Celsius
 Temp = (Temp * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit - comment out this line if you need Celsius
 return Temp;
}

