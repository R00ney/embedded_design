/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING 
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT 
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR 
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE 
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software 
* and to discontinue the availability of this software.  By using this software, 
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2013 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_main.c
* Version      : Applilet3 for RL78/G14 V2.01.00.02 [09 Aug 2013]
* Device(s)    : R5F104PJ
* Tool-Chain   : IAR Systems iccrl78
* Description  : This file implements main function.
* Creation Date: 2/19/2014
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_adc.h"
#include "r_cg_it.h"
/* Start user code for include. Do not edit comment generated here */
#include "lcd.h"
#include "RDKRL78_spi.h"
#include "tasks.h"
#include "rtc_sched.h"
#include "queu.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
extern Q_T  rx_q;      //recieve queu
/* End user code. Do not edit comment generated here */

/* Set option bytes */
#pragma location = "OPTBYTE"
__root const uint8_t opbyte0 = 0xEFU;
#pragma location = "OPTBYTE"
__root const uint8_t opbyte1 = 0xFFU;
#pragma location = "OPTBYTE"
__root const uint8_t opbyte2 = 0xF8U;
#pragma location = "OPTBYTE"
__root const uint8_t opbyte3 = 0x85U;

/* Set security ID */
#pragma location = "SECUID"
__root const uint8_t secuid[10] = 
    {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U};
/* Secure trace RAM area */
__no_init __root unsigned char ocdtraceram[1024] @ 0xFA300;

void R_MAIN_UserInit(void);

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
    R_MAIN_UserInit();
    /* Start user code. Do not edit comment generated here */


    R_IT_Start(); 						/* Start interval timer (for RTC_Scheduler) */
    SPI_Init();               /* LCD SPI initialization */
    LCDInit();              	/* LCD controller initialization*/

    LCDFont(FONT_SMALL);
    LCDString("ngohara",1,1);

    //Debug_Decode();
    
    Init_RTC_Scheduler();
    
    //start Queu
    Q_Init( &rx_q);
    ///*  
    //Start Serial Recieve 
    //uint8_t * const rx_buf, uint16_t rx_num
     uint8_t * rx_address;
     uint8_t rx_buff[2];
     rx_address = rx_buff; 
    R_UART1_Receive(rx_address, 2);
    //*/  
    //Add_Task(Task1, RTC_FREQ_TO_TICKS(11), 1);  // Flash LED at 5.5 Hz 
    Add_Task(Task1, 0, 1);
    //Add_Task(Task2, RTC_FREQ_TO_TICKS(4), 2);   // Flash LED at 2 Hz 
    ///*
    Add_Task(Task2, 0, 2);  //decode NMEA 
    Add_Task(Task3, RTC_FREQ_TO_TICKS(10), 3);   // Update LCD at 1 Hz task frequency 

    
    
    R_UART1_Start();
    //*/
    Run_RTC_Scheduler();
    
    
    while (1U)
    {
      ;
    }
    /* End user code. Do not edit comment generated here */
}


/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
