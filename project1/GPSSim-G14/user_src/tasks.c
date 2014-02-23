#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"

#include "rtc_sched.h"
#include "lcd.h"

void Task1(void) {
	static char led_state=0;

	LED_4_G = led_state;
	led_state ^= 1;
}

void Task2(void) {
	static char led_state=0;

	LED_6_G = led_state;
	led_state ^= 1;
}

// Task3 runs at 1 Hz, updates elapsed time on LCD
void Task3(void) {
	static char led_state=0;
	static unsigned char m=0, s=0;

	LED_8_G = led_state;
	led_state ^= 1;

	s++;
	if (s>59) {
		m++;
		s = 0;
	}

	LCDPrintf(2,0, "%02u:%02u", m, s);
}
