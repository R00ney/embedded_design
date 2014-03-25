#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
#include "r_cg_userdefine.h"
#include <includes.h>
#include <string.h>
#include "accel.h"
#include "lcd.h"


void InitializeAccelerometer(void) {
	unsigned char i2cbuf[2];/* Buffer for I2C transactions */

	LCDStringPosLine("Init Accel", 0, 1);

	// initialize accelerometer
	// Set the 'measure' bit to turn on accelerometer
  	i2cbuf[0] = ACCEL_REG_PWRCTL;
  	i2cbuf[1] = ACCEL_CTL_MEASURE;
  	R_IICA0_Master_Send(ACCEL_WADDR, i2cbuf, 2, 32);

  	while (IICBSY0)
		;			/* Busy-wait until I2C TX is done */

	LCDStringPosLine("Done", 0, 2);
}

void App_TaskAccel (void * p_arg)
{
	OS_ERR err;
	unsigned char i2cbuf[6]; /* Buffer for I2C transactions */
  	signed int datax = 0;   /* x axis acceleration */
  	signed int datay = 0;   /* y axis acceleration */
  	signed int dataz = 0;   /* z axis acceleration */

	p_arg = p_arg;

	// clear initialization message from LCD
	LCDClearLine(1);
	LCDClearLine(2);

	while (1) {
  		/*** ACCELEROMETER HANDLING ***/

		/* Inform accelerometer of starting address at which to read next */
		i2cbuf[0] = ACCEL_REG_DATASTART;
		R_IICA0_Master_Send(ACCEL_WADDR, i2cbuf, 1, 32);
		while (IICBSY0)
			;    /* Busy-wait until I2C TX is done */

		/* Read axis data (three at 2 bytes each) from the accelerometer */
		R_IICA0_Master_Receive(ACCEL_RADDR, i2cbuf, 6, 32);
		while (IICBSY0)
			;    /* Busy-wait until I2C RX is done */

		/* Convert the returned x and y data bytes into signed data */
		datax = ((signed int)i2cbuf[1] << 8) | i2cbuf[0];
		datay = ((signed int)i2cbuf[3] << 8) | i2cbuf[2];
		dataz = ((signed int)i2cbuf[5] << 8) | i2cbuf[4];

		/*** LCD HANDLING ***/
		LCDPrintf(1, 0, "X:%5d", datax);
		LCDPrintf(2, 0, "Y:%5d", datay);
		LCDPrintf(3, 0, "Z:%5d", dataz);

	   	OSTimeDlyHMSM(0u, 0u, 0u, 250u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}
