// GPS message generation

#include "gps_sim.h"
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_adc.h"
#include "lcd.h"
#include "stdio.h"
#include "string.h"
#include "r_cg_userdefine.h"

extern volatile uint8_t G_UART_SendingData, G_UART_ReceivingData;

char msg[128];

uint8_t checksum(char *s) {
    uint8_t c = 0;

    while(*s)
        c ^= *s++;

    return c;
}

void create_GLL_msg(char * buffer, int lat_deg, float lat_min, int lon_deg, float lon_min, unsigned
hr, unsigned min, unsigned sec) {
	uint8_t cs=0;
	char end[8];

	// form message
	sprintf(buffer, "$GPGLL,%02d%06.3f,N,%03d%06.3f,W,%02d%02d%02d,A,*", lat_deg, lat_min, lon_deg, lon_min,
	hr, min, sec);

	// form checksum
	cs = checksum(buffer);

	// undo effects of exoring in start $ and end *
	cs ^= '$' ^ '*';

	sprintf(end, "%02X\n\r", cs);
	strcat(buffer, end);
}

void create_RMC_msg(char * buffer, int lat_deg, float lat_min, int lon_deg, float lon_min, unsigned
hr, unsigned min, unsigned sec, float speed, float track, uint32_t date, float var) {
	uint8_t cs=0;
	char end[8];

	// form message
	sprintf(buffer, "$GPRMC,%02d%02d%02d,A,%02d%06.3f,N,%03d%06.3f,W,%05.1f,%04.1f,%06ld,%05.1f,W*",
					hr, min, sec, lat_deg, lat_min, lon_deg, lon_min, speed, track, date, var);

	// form checksum
	cs = checksum(buffer);

	// undo effects of exoring in start $ and end *
	cs ^= '$' ^ '*';


	sprintf(end, "%02X\n\r", cs);
	strcat(buffer, end);
}


void inc_time(unsigned * hours, unsigned * minutes, unsigned * seconds) {
	(*seconds)++;
	if (*seconds > 59) {
		*seconds -= 60;
		(*minutes)++;
		if (*minutes > 59) {
			*minutes -= 60;
			(*hours)++;
		}
	}
}


void sim_motion(void) {
	int16_t i, lat_deg = 35, lon_deg = 78;
	uint32_t date=22213;
	float lat_min = 1.1, lon_min = 2.6, lat_step=0.011, lon_step=0.025;
	float spd=1.2, trk=180.0, var=0.0;

	unsigned h=0, m=0, s=0, ctr=0;
	volatile uint32_t dly;

	LCDPrintf(1,0, "GPSSimulator");
	LCDPrintf(2,0, "deg minutes");

	LCDPrintf(3,0, "%03d %06.3f", lat_deg, lat_min);
	LCDPrintf(4,0, "%03d %06.3f", lon_deg, lon_min);
	LCDPrintf(5,0, "%02d:%02d:%02d", h, m, s);
	LCDPrintf(6,0, "%4d msgs", ctr);


	while (1) {
		for (i=0; i<NUM_STEPS; i++) {
			create_RMC_msg(msg, lat_deg, lat_min, lon_deg, lon_min, h, m, s, spd, trk, date, var);
			G_UART_SendingData = 1;
			R_UART0_Send((uint8_t *) msg, strlen(msg));

			//	wait for tx to finish
			while (G_UART_SendingData)
				;

			// update variables and LCD
			lat_min += lat_step;
			lon_min += lon_step;
			ctr += 1;
			inc_time(&h, &m, &s);

			LCDPrintf(3,0, "%03d %06.3f", lat_deg, lat_min);
			LCDPrintf(4,0, "%03d %06.3f", lon_deg, lon_min);
			LCDPrintf(5,0, "%02d:%02d:%02d", h, m, s);
			LCDPrintf(6,0, "%4d msgs", ctr);

			// delay between sentences unless user is pressing switch 3
			if (SW_3 == 1) {
				for (dly=0; dly<600000; dly++) {
					if (SW_3 == 0) // abandon delay immediately
						break;
					;
				}
			}
		}
		lat_step *= -1.0;
		lon_step *= -1.0;
	}
}

