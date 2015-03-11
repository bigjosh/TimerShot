// Demo of a technique to generate narrow and precise one shot pulses using a 
// timer module on an AVR. This demo code is writen for an Arduino and uses
// the Timer2 moudle, but this techniquie should would on other AVRs and timers. 

// The demo generates one pulse per second. 
// The 1st pulse is 0 cycles long (no pulse), 
// The 2nd pulse is 1 cycle long (~63ns),
// The 3rd pulse is 2 cycles long (~126ns), 
// ...up to a total of 20 pulses, and then starts over.

// The one shot pulses are output on Digial pin 3
// An ossiliscope trigger signal is output on Ditigal pin 12

// Note that OC2B = GPIO port PD3 = Arduino Digital Pin 3
// Generate a on-shot pulse that is _clocks_ clock counts long

void pulseTimer2ctc(uint8_t clocks) {

	DDRD |= _BV(3);			// Set pin to output
	DDRB |= _BV(3);			// Set pin to output

	DDRB |= _BV(4);			// Set D12 to output for trigger

	TCCR2A = 0;
	TCCR2B = 0;		// Stop counter, start selecting mode 0

	TCNT2 = 0xf0;			// Start counting at bottom. 
	OCR2A = 0x01;			// TOP
	OCR2B = 0xf5;			// Match (and toggle off) here

	TCCR2A = _BV(COM2A1) | _BV(COM2A0) | _BV(COM2B0) | _BV(COM2B1) | _BV(WGM20) | _BV(WGM21);	// Set on Match, Mode 7
	TCCR2B = _BV(WGM22) | _BV(CS20);		// Trigger Compare (toggle), Start counting
	/*
	PORTB |= _BV(4);		// Trigger
	//_delay_us(10);
	PORTB &= ~_BV(4);
	*/
}

#define OSP_SET_WIDTH(cycles) (OCR2B = 0xff-(cycles-1))

// Setup the one-shot pulse generator and initialize with a pulse width that is (cycles) clock counts long

void osp_setup(uint8_t cycles) {


	TCCR2B =  0;			// Halt counter by setting clock select bits to 0 (No clock source).
							// This keeps anyhting from happeneing while we get set up

	TCNT2 = 0x00;			// Start counting at bottom. 
	OCR2A = 0;				// Set TOP to 0. This effectively keeps us from counting becuase the counter just keeps reseting back to 0.
							// We break out of this by manually setting the TCNT higher than 0, in which case it will count all the Way up to MAX and then overflow back to 0 and get locked up again.
	OSP_SET_WIDTH(cycles);	// This also makes new OCR values get loaded frm the buffer on every clock cycle. 

	;	// Match (and set OC) here. For now, set it above TOP so it wil not be accedentally triggered when we start

	TCCR2A = _BV(COM2B0) | _BV(COM2B1) | _BV(WGM20) | _BV(WGM21);	//  OC2B=Set on Match, clear on BOTTOM. Mode 7 Fast PWM.
	TCCR2B = _BV(WGM22)| _BV(CS20);									// Start counting now. WGM22=1 to select Fast PWM mode 7

	DDRD |= _BV(3);			// Set pin to output

}

// Setup the one-shot pulse generator

void osp_setup() {

	osp_setup(1);

}

// Fire a one-shot pulse. Use the most recently set width. 

#define OSP_FIRE() (TCNT2 = OCR2B - 1)

// Test if the last fired pulse is still in progress

#define OSP_INPROGRESS() (TCNT2>0)


// Fire a one-shot pusle with the specififed width. 
// Order of operations in calculating m must avoid overflow of the unint8_t.
// TCNT2 starts one count lower than the match value becuase the chip will block any compare on the cycle after setting a TCNT. 

#define OSP_SET_AND_FIRE(cycles) {uint8_t m=0xff-(cycles-1); OCR2B=m;TCNT2 =m-1;}


void setup()
{

	DDRB |= _BV(4);		// Set Digital Pin 12 to output for ossiliscope trigger
	osp_setup();

}

void loop()
{

	// Step though 0-19 cycle long pulses for demo purposes 

	for (uint8_t o = 0; o < 20; o++) {

		_delay_ms(1000);


		OSP_SET_AND_FIRE(o);

		PORTB |= _BV(4);		// Trigger

		while (OSP_INPROGRESS());

		PORTB &= ~_BV(4);


	}


}
