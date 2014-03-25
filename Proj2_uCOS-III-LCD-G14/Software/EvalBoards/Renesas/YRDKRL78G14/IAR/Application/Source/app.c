/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                            (c) Copyright 2010-2011; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           EXAMPLE CODE
*
*                                           Renesas RL78
*                                               on the
*                                     YRDKRL78G13 Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
#include <includes.h>
#include <string.h>
#include "accel.h"
#include "lcd.h"

/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/

static  OS_TCB    App_TaskStart_TCB;
static  OS_TCB    App_TaskAccel_TCB;
static  OS_TCB    App_Task1_TCB;

static  CPU_STK   App_TaskStart_Stk[APP_CFG_TASK_START_STK_SIZE];
static  CPU_STK   App_TaskAccel_Stk[APP_CFG_TASKACCEL_STK_SIZE];
static  CPU_STK   App_Task1_Stk[APP_CFG_TASK1_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_TaskCreate(void);
static  void  App_ObjCreate (void);

static  void  App_TaskStart (void  *p_arg);

static  void  App_Task1 (void  *p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : Startup Code.
*
* Note(s)     : none.
*********************************************************************************************************
*/

// AGD: was main, but applilet generates own main function. Added call from main to ucosIII_main.
int  ucosIII_main (void)
{
    OS_ERR   os_err;

    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

    BSP_PreInit();

    OSInit(&os_err);                                            /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB      *)&App_TaskStart_TCB,              /* Create the start task                                */
                 (CPU_CHAR    *)"Start",
                 (OS_TASK_PTR  )App_TaskStart,
                 (void        *)0,
                 (OS_PRIO      )APP_CFG_TASK_START_PRIO,
                 (CPU_STK     *)&App_TaskStart_Stk[0],
                 (CPU_STK_SIZE )APP_CFG_TASK_START_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY   )0u,
                 (OS_TICK      )0u,
                 (void        *)0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&os_err);

	OSStart(&os_err);                                           /* Start multitasking (i.e. give control to uC/OS-III).*/

    return (0);
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
    OS_ERR  err;
	uint8_t x=0;
	char c;

    (void)p_arg;						/* prevent compiler warning */

    BSP_PostInit();                                             /* Initialize BSP functions                             */

#if (OS_CFG_STAT_TASK_EN > 0u)
    OSStatTaskCPUUsageInit(&err);                               /* Determine CPU capacity                               */
#endif

    App_TaskCreate();                                           /* Create Application tasks                             */
    App_ObjCreate();                                            /* Create Application kernel objects                    */

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

#if (OS_CFG_STAT_TASK_EN > 0u)
    OSStatReset(&err);
#endif

	LCDInit();

	InitializeAccelerometer(); 				// Accel init uses LCD/Glyph

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
		LCDClearLine(0);

		if (x&0x08) {
			c = '-';
		} else {
			c = '<';
		}
		LCDChar(c, x++, 0);
		if (x >= LCD_W-8)
			x = 0;

	   	OSTimeDlyHMSM(0u, 0u, 0u, 100u, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}


/*
*********************************************************************************************************
*                                          AppTaskCreate()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_TaskCreate (void)
{
  OS_ERR  os_err;

		OSTaskCreate((OS_TCB 	*)&App_Task1_TCB,              /* Create task 1                               */
                 (CPU_CHAR    *)"Task1",
                 (OS_TASK_PTR  )App_Task1,
                 (void        *)0,
                 (OS_PRIO      )APP_CFG_TASK1_PRIO,
                 (CPU_STK     *)&App_Task1_Stk[0],
                 (CPU_STK_SIZE )APP_CFG_TASK1_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE )APP_CFG_TASK1_STK_SIZE,
                 (OS_MSG_QTY   )0u,
                 (OS_TICK      )0u,
                 (void        *)0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&os_err);

		OSTaskCreate((OS_TCB     *)&App_TaskAccel_TCB,        /* Create accelerometer task                                */
                 (CPU_CHAR    *)"TaskAccel",
                 (OS_TASK_PTR  )App_TaskAccel,
                 (void        *)0,
                 (OS_PRIO      )APP_CFG_TASKACCEL_PRIO,
                 (CPU_STK     *)&App_TaskAccel_Stk[0],
                 (CPU_STK_SIZE )APP_CFG_TASKACCEL_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE )APP_CFG_TASKACCEL_STK_SIZE,
                 (OS_MSG_QTY   )0u,
                 (OS_TICK      )0u,
                 (void        *)0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&os_err);
}


/*
*********************************************************************************************************
*                                          App_ObjCreate()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_ObjCreate (void)
{
}

static void App_Task1 (void * p_arg)
{
	OS_ERR os_err;
  	uint32_t n=0;

	p_arg = p_arg;
	while (1) {
		LCDPrintf(7, 0, "T1:%6ld", n++);
      	OSTimeDlyHMSM(0u, 0u, 0u, 51u,
                     OS_OPT_TIME_HMSM_STRICT,
                      &os_err);

	}
}


