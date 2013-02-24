#define F_CPU 80000000
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#define LED_MAX_FADE 100

#define BREAK  0x1
#define WORK   0x2

#define CLEAR_LEDS() PORTB &= ~(_BV(DDB1) | _BV(DDB2) | _BV(DDB3))
#define LED_ON(BIT)  PORTB |= BIT;
#define LED_OFF(BIT) PORTB &= ~(BIT);

volatile char state;
volatile char running;

// counts at 10 Hz
volatile unsigned long counter;
// counts seconds
volatile unsigned long seconds;

void led_fade_loop(int level) {
    unsigned int i;
    // 0 should be maximum, so set to 1
    level++;

    for (i = 1; i < LED_MAX_FADE; i++) {
	// if running stops during loop, blank LEDs
	if (!running) {
	    CLEAR_LEDS();
	    return;
	}

	if (i % level == 0) {
	    if (state & BREAK)
		PORTB |= (_BV(DDB1));
	    if (state & WORK)
		PORTB |= (_BV(DDB2));
	    if (state & 0)
		PORTB |= (_BV(DDB3));
	}
	else {
	    if (state & BREAK)
		PORTB &= PORTB ^ (_BV(DDB1));
	    if (state & WORK)
		PORTB &= PORTB ^ (_BV(DDB2));
	    if (state & 0)
		PORTB &= PORTB ^ (_BV(DDB3));
	}
    }
}

void fade_led() {
    unsigned int i;
    for (i = 3; i < 30; i++) {
	led_fade_loop(i);
    }
    for (i = 30; i > 3; i--) {
       led_fade_loop(i);
    }
}

void toggle_led() {
    switch (state) {
    case BREAK:
	PORTB ^= _BV(DDB1);
	break;
    case WORK:
	PORTB ^= _BV(DDB2);
    }
}

void init(void) {
    // stop the watchdog
    wdt_disable();

    // configure LED pins (B0, B1, B2 are outputs)
    PORTB = 0;
    DDRB |= (_BV(DDB1) | _BV(DDB2));

    // configure button pin (B3 is an input, interrupt on change)
    // enable pull up
    PORTB |= _BV(DDB0);
    GIMSK |= (1 << PCIE);
    PCMSK |= (1 << PCINT0);

    // configure timer
    TCCR0A |= (1 << WGM01);
    TCCR0B |= (1 << CS02) | (1 << CS00) | (1 << CS00);
    OCR0A = 117;
    TIMSK0 |= (1 << OCIE0A);
    sei();
    //cli();

    // start by working!
    state = WORK;
    running = 0;
}

int main (void)
{
    unsigned long i;
    init();
    for (;;) {
	if (running) {
	    fade_led();
	}
	else {
	    toggle_led();
	    for (i=0; i < 250000; i++);
	}
    }
    return 1;
}

ISR(TIM0_COMPA_vect) {
    if (!running) {return;}
    counter++;
    if (counter % 100 == 0) {
	seconds++;
	switch (state) {
	case WORK:
	    if (seconds % 1 == 0) {
		state = BREAK;
		CLEAR_LEDS();
		seconds = 0;
		running = 0;
	    }
	    break;
	case BREAK:
	    if (seconds % 3 == 0) {
		state = WORK;
		CLEAR_LEDS();
		seconds = 0;
		running = 0;
	    }
	    break;
	}
    }
}

ISR(PCINT0_vect) {
    running = 1;
    return;
}
