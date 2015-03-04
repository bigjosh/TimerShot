
// Produces a 124ns pulse on OC1A and OC1B when run

// OC1B = PB2 = Arduino Digital Pin 10

void setup()
{

  /* add setup code here */


	// Mode 14, FastPWM TOP=ICR1

	// Set OC1B on Compare Match, Clear at BOTTOM
	TCCR1A = _BV(COM1B1) | _BV(COM1B0) | _BV(COM1A1) | _BV(COM1A0) | _BV(WGM11) ;

	// Mode 14, clk/0
	TCCR1B = _BV(WGM13) |_BV(WGM12) ;	


	TCNT1 = 0xffff;	// Start counting here
	OCR1B = 0xffff;	// Match here (set bit)

	OCR1A = 500;
	ICR1 = 2;		// Stop counting here


	DDRB |= _BV(2) | _BV(1);		// Set PB2 to output mode so OC1B show up on pin.


	// set preclaser to clk/1, which will start it counting
	TCCR1B |=  _BV(CS10);

	DDRB |= _BV(5);		// Enable linking LED on D13

}

void loop()
{

	/* add main program code here */

	PINB |= _BV(5);

	_delay_ms(500);

	//TCNT1 =  1100;

}
