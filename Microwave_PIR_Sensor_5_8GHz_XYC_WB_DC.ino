#include "Arduino.h"

bool status = LOW;

void setup()
{
	Serial.begin(9600);
	pinMode(11, INPUT);
	pinMode(13,OUTPUT);
	Serial.println("Setup completed.");
}


void loop()
{
	static unsigned int count = 1;

	if (digitalRead(11) == HIGH) {
		if (!status) {
			digitalWrite(13,HIGH);
			status = true;
			Serial.print(count); Serial.println(". Movement detected.");
		}
	}
	else{
		if (status) {
			digitalWrite(13,LOW);
			status = false;
			Serial.print(count++); Serial.println(". Detector waiting for movement\n");
		}
	}
}
