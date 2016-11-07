/*********************************************************************
 IoT Project 2  - BLE Controlled Lighting
 
 This sketch is written to accompany IoT Project 2 
 with Arduino UNO Starter Kit & Adafruit BLE shield
 
 Created 18 Feb 2016
 by Zhong Zheng
*********************************************************************/

#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

#define FACTORYRESET_ENABLE      1
#define		RED		11
#define		GREEN 	10
#define		BLUE	6
#define 	BUTTON_SWITCH	5
#define		BUTTONOUT 12
#define	NUM_COLORS	6
#define	MAX_BRIGHT	10
#define	MIN_BRIGHT	0
/*=========================================================================*/


// Create the bluefruit object, either software serial...uncomment these lines

SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
int ledState = 1;
int pushGND = 0;
float colors[NUM_COLORS][3] = {{1.0,0.0,0.0},{1.0,1.0,0.0},{0.0,1.0,0.0},{0.0,1.0,1.0},{0.0,0.0,1.0},{1.0,0.0,1.0}};
int currentColor=0;
int RGBState=true;
float j = 0;
int brightness = 10;
int RSSI;
int RSSIState = 1;
int MODE = 0;
//int lightmode = 0;

void setup(void)
{
	
  Serial.begin(115200);
	// make LED pin an output
	pinMode(BLUE, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(RED, OUTPUT);
	pinMode(BUTTONOUT,OUTPUT);
	pinMode(BUTTON_SWITCH, INPUT);

	//initial RGB	
  	Serial.println(F("initial light ON"));
  	digitalWrite(BUTTONOUT,0);
	lightUpRGB(0,0,1,MAX_BRIGHT);

  Serial.println(F("Adafruit Bluefruit UART DATA mode Example"));
  Serial.println(F("-------------------------------------"));
  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);
  
  //set device name
  Serial.println(F("Setting device name to 'Zhong RGB lighting': "));
  if (! ble.sendCommandCheckOK(F( "AT+GAPDEVNAME=Zhong RGB lighting" )) ) {
    error(F("Could not set device name?"));
  }
  
  /* disable verbose mode */
  ble.verbose(false);
  
  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  /* Wait for connection */
//  while (! ble.isConnected()) {
//      delay(500);
//  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
//  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));



}

/**************************************************************************/
#define BUFSIZE                        128
void loop(void)
{
//Main switch on/off	
  if (button_switch_Pushed()) {
  	pushGND = 0;
  }
  else {
  	pushGND = 1;
  }
  digitalWrite(BUTTONOUT,pushGND);

// Check for user input
	char n,m,RSSI_INPUT[BUFSIZE+1], inputs[BUFSIZE+1];
// Automatically turn on LED when phone is in proximity. Command: AT+BLEGETRSSI
//  ble.setMode(BLUEFRUIT_MODE_COMMAND); 
  ble.println(F("AT+BLEGETRSSI"));
  n = ble.readline(RSSI_INPUT, BUFSIZE);
  RSSI_INPUT[n] = 0;
  RSSI = atoi(RSSI_INPUT); 
  ble.readline();
  Serial.print("RSSI : "); Serial.println(RSSI);
  ble.waitForOK();
//  ble.setMode(BLUEFRUIT_MODE_DATA);
//  delay(2000);
  
//Send serial input data to BLE host
	if (Serial.available())
	{
   		n = Serial.readBytes(inputs, BUFSIZE);
   		inputs[n] = 0;
   		// display to user
   		Serial.print("Sending: ");
   		Serial.print(inputs);
	    // Send input data to host via Bluefruit
   		ble.print(inputs);
	}
	
// Echo received data

	if (RSSI >= -50){
		lightUpRGB(currentColor,2,ledState,brightness);
		
		if (ble.available()) {
			n = ble.readline(inputs, BUFSIZE);
			m = inputs[0];
			inputs[n] = 0;

			// display to user
			Serial.print("Receiving: ");  Serial.println(inputs);
	//		Serial.print("m: "); 	Serial.println(m);
			
			// light on/off via BLE
			if (m == 'h'){
				ledState = !ledState;
				brightness = 10;
				MODE = 0;
				lightUpRGB(currentColor,MODE,ledState,brightness);
				Serial.print("RGB on/off state : ");Serial.println(ledState);
			}
	
		    if (m == 'c') {
				if (++currentColor == NUM_COLORS) currentColor = 0;
				brightness = 10;
				MODE = 1;
				lightUpRGB(currentColor,MODE,ledState,brightness);
				Serial.print("RGB change color,current color : ");Serial.println(currentColor);
			}
    
	   	    if ((m == 'u')&&(ledState == 1)) {
				if (++brightness >= MAX_BRIGHT) brightness = 10;
				MODE = 2;
				lightUpRGB(currentColor,MODE,ledState,brightness);
				Serial.print("RGB brightness up, current brightness : ");Serial.println(brightness);
			}	
			
			if ((m == 'd')&&(ledState == 1)) {
				if (--brightness <= MIN_BRIGHT) brightness = 0;
				MODE = 2;
				lightUpRGB(currentColor,MODE,ledState,brightness);
				Serial.print("RGB brightness down, current brightness : ");Serial.println(brightness);
			}
		}
	}
	else if (RSSI <= -70)  lightUpRGB(currentColor,MODE,0,brightness);
	
	
}

// Boolean for button_swtich_Pushed
boolean button_switch_Pushed()
{
	static int buttonState0 = 0;
	int state0 = digitalRead(BUTTON_SWITCH);
	
	if (buttonState0 != state0) {
		delay(100);
		state0 = digitalRead(BUTTON_SWITCH);
	}
	
	if (buttonState0 == 0 && state0 == 1) {
		Serial.println("RGB on/off changed from");Serial.print(RGBState);
		RGBState = !RGBState;
		Serial.print(" to ");Serial.println(RGBState);
	}
	
	buttonState0 = state0;
	return RGBState;
}

//Change color RGB LED or photo sensor keep light on/off
void lightUpRGB(int i,int s,int l,int b)
{	
	float R,G,B;
	
	if (s == 0) {
		R = colors[i][0]*l;
		G = colors[i][1]*l;
		B = colors[i][2]*l;
		analogWrite(RED, R*255);
		analogWrite(GREEN, G*255);
		analogWrite(BLUE, B*255);
	}
	// Change light color gradually and smoothly
	else if ( s == 1) {
		j = 0;	
		while (j < 101){
				if (i == 0) {
					R = ((colors[i][0] - colors[5][0])*(j/100) + colors[i][0])*l;
					G = ((colors[i][1] - colors[5][1])*(j/100) + colors[i][1])*l;
					B = ((colors[i][2] - colors[5][2])*(j/100) + colors[i][2])*l;
				}
				else {
					R = ((colors[i][0] - colors[i-1][0])*(j/100) + colors[i-1][0])*l;
					G = ((colors[i][1] - colors[i-1][1])*(j/100) + colors[i-1][1])*l;
					B = ((colors[i][2] - colors[i-1][2])*(j/100) + colors[i-1][2])*l;
				}
		
				analogWrite(RED, R*255);
				analogWrite(GREEN, G*255);
				analogWrite(BLUE, B*255);
				j++;
				delay(20);
			}
	}
	else if (s == 2) {
		j = b;
		R = colors[i][0] * (j/10) * l;
		G = colors[i][1] * (j/10) * l;
		B = colors[i][2] * (j/10) * l;
		analogWrite(RED, R*255);
		analogWrite(GREEN, G*255);
		analogWrite(BLUE, B*255);
	}
}
