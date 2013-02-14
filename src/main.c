#define F_CPU 80000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#define LED_MAX_FADE 1000

#define LED_RED 0x1
#define LED_GREEN 0x2
#define LED_BLUE 0x4

void led_fade(int level, char color) {
    unsigned int i;
    // 0 should be maximum, so set to 1
    level++;
    // prevent levels that cause flashing

    for (i = 1; i < LED_MAX_FADE; i++) {
        if (i % level == 0) {
            if (color & LED_GREEN)
                PORTB |= (_BV(DDB1));
            if (color & LED_BLUE)
                PORTB |= (_BV(DDB0));
            if (color & LED_RED)
                PORTB |= (_BV(DDB2));
        }
        else {
            if (color & LED_GREEN)
                PORTB &= PORTB ^ (_BV(DDB1));
            if (color & LED_BLUE)
                PORTB &= PORTB ^ (_BV(DDB0));
            if (color & LED_RED)
                PORTB &= PORTB ^ (_BV(DDB2));
        }
    }
}

void init(void) {
    // stop the watchdog
    wdt_disable();

    // configure LED pins
    PORTB = 0;
    DDRB |= (_BV(DDB0) | _BV(DDB1) | _BV(DDB2));

    // configure timer
    TCCR1B |= (1 << CS10);
    OCR1A = 15624;
    TCCR1B |= (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);
    sei();

}
int main (void)
{
    init();
    loop();
    return 1;
}

char on;
unsigned int seconds;

void loop() {
    unsigned int i;
    on = LED_RED;

    for (;;) {
        for (i = 15; i < 90; i++) {
            led_fade(i, on);
        }
        for (i = 90; i > 15; i--) {
            led_fade(i, on);
        }
    }
}

ISR(TIMER1_COMPA_vect) {
    seconds++;
    if (seconds % 10 == 0) {
        on ^= LED_GREEN | LED_RED;
    }
}
