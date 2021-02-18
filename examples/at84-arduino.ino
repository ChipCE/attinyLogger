// http://www.gammon.com.au/forum/?id=11488
 byte adcsra_save = ADCSRA;
 byte prr_save = PRR;

 ADCSRA = 0;  // disable ADC
 PRR = 0xFF; // turn off various modules
 set_sleep_mode (SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 attachInterrupt (0, wake, LOW);   // allow grounding pin 2 to wake us
 sleep_mode ();            // now goes to Sleep and waits for the interrupt
 detachInterrupt (0);      // stop LOW interrupt
 
 ADCSRA = adcsra_save; // stop power reduction
 PRR = prr_save;


int analogvalue;

// the setup routine runs once when you press reset:
void setup() {                
 // initialize the digital pin as an output.
 Serial.begin(9600);

}