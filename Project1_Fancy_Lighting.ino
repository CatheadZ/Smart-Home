/*
 IoT Project 1  - Fancy Lighting
 
 This sketch is written to accompany IoT Project 1 
 in the Arduino Starter Kit
 
 Created 4 Feb 2016
 by Zhong Zheng

*/

#define		RED		11
#define		GREEN 	10
#define		BLUE	9

#define 	BUTTON_SWITCH	7
#define		BUTTON_COLOR	8
#define		PHOTO_SENSOR	A0

#define	NUM_COLORS	6
float colors[NUM_COLORS][3] = {{1.0,0.0,0.0},{1.0,1.0,0.0},{0.0,1.0,0.0},{0.0,1.0,1.0},{0.0,0.0,1.0},{1.0,0.0,1.0}};
int currentColor=0;
int RGBState=false;
boolean lighton=false;
// variable to hold sensor value
int sensorValue;
// variable to calibrate low value
int sensorLow = 1023;
// variable to calibrate high value
int sensorHigh = 0;
float j = 0;
	
void setup()
{
	Serial.begin(9600);
	Serial.println("Welcome! Zhong Zheng - Fancy Lighting");
	Serial.println("Press Button Switch to turn on light (highest priority)");
	Serial.println("Press Button Colors to change color (Totally 6 colors)");
	Serial.println("Photo sensor: light dark, turn on LED; light bright, turn off LED");	
	
	// calibrate for the first five seconds after program runs
    while (millis() < 5000) {
    // record the maximum sensor value
    	sensorValue = analogRead(PHOTO_SENSOR);
	 	if (sensorValue > sensorHigh) {
	    sensorHigh = sensorValue;
		  }
    // record the minimum sensor value
		if (sensorValue < sensorLow) {
      	sensorLow = sensorValue;
    	  }
    }
	
	pinMode(BLUE, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(RED, OUTPUT);
	pinMode(BUTTON_SWITCH, INPUT);
	pinMode(BUTTON_COLOR, INPUT);
	pinMode(PHOTO_SENSOR, INPUT);
	
	lightUpRGB(currentColor,0);
}

void loop()
{
	if (button_switch_Pushed()) {
		if (photo_sensor()){
			if (button_color_Pushed()) {
				if (++currentColor == NUM_COLORS) currentColor = 0;
				lightUpRGB(currentColor, 2);
			}
			lightUpRGB(currentColor,1);
		}
		else {
			lightUpRGB(currentColor,0);
		}
	}
	else {
		lightUpRGB(currentColor,0);
	}
}

//Change color RGB LED or photo sensor keep light on/off
void lightUpRGB(int i, int s)
{	
	float R,G,B;
	// Turn off light
	if (s == 0) {
		analogWrite(RED, 0);
		analogWrite(GREEN, 0);
		analogWrite(BLUE, 0);
	}
	// Keep light on
	else if (s==1) {
		analogWrite(RED, colors[i][0]*255);
		analogWrite(GREEN, colors[i][1]*255);
		analogWrite(BLUE, colors[i][2]*255);
	}
	// Change light color gradually and smoothly
	else {
	j = 0;	
	while (j < 101){
			if (i == 0) {
				R = (colors[i][0] - colors[5][0])*(j/100) + colors[i][0];
				G = (colors[i][1] - colors[5][1])*(j/100) + colors[i][1];
				B = (colors[i][2] - colors[5][2])*(j/100) + colors[i][2];
			}
			else {
				R = (colors[i][0] - colors[i-1][0])*(j/100) + colors[i-1][0];
				G = (colors[i][1] - colors[i-1][1])*(j/100) + colors[i-1][1];
				B = (colors[i][2] - colors[i-1][2])*(j/100) + colors[i-1][2];
			}
		
			analogWrite(RED, R*255);
			analogWrite(GREEN, G*255);
			analogWrite(BLUE, B*255);
			j++;
			delay(20);
		}		
		
		
	}
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

// Boolean for button_change_color_Pushed
boolean button_color_Pushed()
{
	static int buttonState1=0;
	boolean pushed1=false;
	
	int state1 = digitalRead(BUTTON_COLOR);
	
	if (buttonState1 != state1) {
		delay(100);
		state1 = digitalRead(BUTTON_COLOR);
	}
	
	if (buttonState1 == 0 && state1 == 1) {
		Serial.println("Change color Button pushed!");
		pushed1=true;
	}
	
	buttonState1 = state1;
	return pushed1;
}
// Boolean for Photo sensor
boolean photo_sensor()
{

	sensorValue = analogRead(PHOTO_SENSOR);
	if (sensorValue < (sensorHigh + sensorLow)/2) {
		return lighton = true;
	}
	else {
		return lighton = false;
	}

}


