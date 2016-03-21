/*
Name:		TouchlessDimmer.ino
Created:	3/20/2016 7:46:59 PM
Author:	Ruben Ravatsaas
*/

#define ultraSoundTrigPin 2
#define ultraSoundEchoPin 4
#define ledPwmPin 3

#define cycleTime 10
#define waveRange 50
#define dimRange 150
#define minRange 30

short dimRangeCycles = 0;
short waveRangeCycles = 0;

bool ledOn = false;
short ledBrightness = 128;

long lastGoodDistanceReading = 0;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	pinMode(ultraSoundTrigPin, OUTPUT);
	pinMode(ultraSoundEchoPin, INPUT);
}

// the loop function runs over and over again until power down or reset
void loop() {
	float distMm = getDistance();
	//Serial.println(distMm);

	// Detect hold
	if (dimRangeCycles >= 10 && distMm <= dimRange) {
		// Hand has been in front of sensor for more than 20 cycles, and still is.
		//Serial.println(map(constrain(distMm, minRange, dimRange), minRange, dimRange, 100, 0));
		ledBrightness = map(constrain(distMm, minRange, dimRange), minRange, dimRange, 255, 0);
		ledOn = true;
	}
	// Detect wave
	else if (waveRangeCycles > 2 && waveRangeCycles <= 10 && distMm >= waveRange)
	{
		// Hand was in front of sensor for 1-3 cycles, and is now removed
		Serial.println("Detected wave");
		ledOn = !ledOn;
	}

	// Detect hold
	if (dimRangeCycles >= 10 && distMm <= dimRange) {
		// Hand has been in front of sensor for more than 20 cycles, and still is.
		Serial.print("Dimmed to: ");
		Serial.println(map(constrain(distMm, minRange, dimRange), minRange, dimRange, 100, 0));
		ledBrightness = map(constrain(distMm, minRange, dimRange), minRange, dimRange, 255, 0);
		ledOn = true;
	}

	// Update counters
	if (distMm <= waveRange)
		waveRangeCycles = min(waveRangeCycles + 1, 255); // Prevent overflow
	else
		waveRangeCycles = 0; // Reset

	if (distMm <= dimRange)
		dimRangeCycles = min(dimRangeCycles + 1, 255); //Prevent overflow
	else
		dimRangeCycles = 0; // Reset


	analogWrite(ledPwmPin, ledBrightness * ledOn);
	delay(cycleTime);
}

// Returns distance in mm.
long getDistance() {
	long pulseLength;
	long distMm;

	// Trigger pulse
	digitalWrite(ultraSoundTrigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(ultraSoundTrigPin, LOW);

	// Read and interpret return
	pulseLength = pulseIn(ultraSoundEchoPin, HIGH);
	distMm = (pulseLength / 2) / 2.91;

	// Ensure reading is in range
	if (distMm < 2000)
		lastGoodDistanceReading = distMm;

	return lastGoodDistanceReading;
}


