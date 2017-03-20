/* Example by TMRh20 2016
 *
 * Overview:
 * Standard frequency counting libraries for Arduino typically involve edge detection and interrupts, which are good for processing
 * digital signals, but not so great for weaker analog signals.
 *
 * Use of the Arduino on-board ADC can provide not only the frequency of the waveform, but its amplitude as well.
 *
 * The HB-100 Microwave Doppler Radar modules are fairly cheap, but require a pre-amp circuit to boost the IF signal. I used an LM358N
 * and the circuit at https://hackaday.io/project/371-hb100-radar-shield. The output sits at about 2.5v when the HB100 is inactive.
 * The pre-amp is sensitive to changes in components if you are breadboarding. The more stable the signal when motionless, the better
 * this code will work.
 *
 * Problem:
 * The HB-100 output indicates both frequency and amplitude of the received signal. Tests using the radar shield with standard frequency
 * counters/libraries showed that using interrupts and edge-detection has limited usefullness.
 *
 * Goal:
 * Detect the frequency and amplitude of low frequency (0-4khz) signals from HB-100 modules, while potentially increasing the sensitivity
 * of waveform detection using HB-100 & circuit as linked above.
 *
 * Result:
 * A simple process uses the Arduino ADC to sample incoming signals using analog peak-to-peak detection, above and below a set
 * "midPoint" which represents the 0v value of the AC waveform. Typically, the midpoint will be ADC_RESOLUTION / 2 or 2.5v, but in this
 * application, a value slightly lower has provided the best results
 *
 * Resolution / Voltage == Volts per Increment ( 1023/5v = 0.005v per Increment )
 * Sensitivity of 10 == peak-to-peak detection of +/- 0.05v signals
 */

//******** USER CONFIG *********************
//Define the "zero" point of the AC waveform as a DC value between 0 and 1023. This needs to be correct when the sensitivity is low ( <100 )
#define MIDPOINT 22
// Define the sensitivity of analog detection. With a very clean & consistent waveform, the sensitivity can be raised significantly.
// This value is important in determining if detected peak-to-peak values constitute a 'wave'
#define SENSITIVITY 75
// The analog pin to use
#define ADCPin A1

// Incoming data is summed, so fetching the results every second
// will indicate speed over the previous second
// How often in mS to display the results ( 0 = print all results if possible)
#define printDelay 250

//*****************************************/

#include "AnalogFrequency.h"

uint32_t displayTimer = 0;
uint32_t amplitude = 0, samples = 0;

void setup() {
	Serial.begin(115200);
	setupADC(ADCPin);
	Serial.println("Ready...");
}

void loop() {

	static long int cnt = 0;

	if (fAvailable() && millis() - displayTimer > printDelay) {
		displayTimer = millis();
		uint32_t frequency = getFreq(&amplitude, &samples);

		//Filter out low amplitude signals with a low count
		//if (amplitude < 540 && samples < 2) {
		if ((amplitude < 700 && samples < 2) || (frequency > 45)) {
			return;
		}
		float speedKM = frequency / 19.49;
		Serial.print(++cnt);
		Serial.print("\t\tKM/h\t");
		Serial.print(speedKM);
		Serial.print("\t\tHz\t");
		Serial.print(frequency);
		Serial.print("\t\tAmplitude\t");
		Serial.print(amplitude);
		Serial.print("\t\tSamples\t");
		Serial.println(samples);
	}
}
