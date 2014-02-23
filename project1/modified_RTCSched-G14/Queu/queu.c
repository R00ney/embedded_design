#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "queu.h"
#include "r_cg_serial.h"
#include "rtc_sched.h"
#include "tasks.h"


/* =================== Global variables ======================= */
Q_T  rx_q;      //recieve queu

extern uint8_t * gp_uart1_tx_address;         /* uart1 send buffer address */
extern uint16_t  g_uart1_tx_count;            /* uart1 send data number */
extern uint8_t * gp_uart1_rx_address;         /* uart1 receive buffer address */
extern uint16_t  g_uart1_rx_count;            /* uart1 receive data number */
extern uint16_t  g_uart1_rx_length;           /* uart1 receive data length */

/* ========================================== */


/*#pragma vector = INTST1_vect
__interrupt static void r_uart1_interrupt_send(void)
	if more data to send {
		dequeue data from tx_q
		send it out transmitter
	}
}*/

#pragma vector = INTSR1_vect
__interrupt static void r_uart1_interrupt_receive(void)
  {
    static char led_state = 0;	
    LED_8_G = led_state;
    led_state ^= 1;
    
    
    uint8_t rx_data;
    uint8_t err_type;
    
    err_type = (uint8_t)(SSR03 & 0x0007U);
    SIR03 = (uint16_t)err_type;

    ///if error occurred	handle communication error
    if (err_type != 0U)
    {
        r_uart1_callback_error(err_type);
    }
    //else {                get data from receiver
    rx_data = RXD1; 
    
    //if space is available in rx_q		enqueue data in rx_q
     if(Q_Enqueue(&rx_q , (char)rx_data) );
       //if true, enqueue suceeded
       
    //else		handle buffer overflow error
     else {
        r_uart1_callback_softwareoverrun( rx_data); //currently does nothing,
        //and loses char
     }
      r_uart1_callback_receiveend();
     
        /*
     //handle expected number of bytes, and call recieveend for Decode task
    if(g_uart1_rx_length > g_uart1_rx_count)
    {
        *gp_uart1_rx_address = rx_data;
        gp_uart1_rx_address++;
        g_uart1_rx_count++;

        if (g_uart1_rx_length == g_uart1_rx_count)
        {
            r_uart1_callback_receiveend();
        }
    }
    else
    {
        r_uart1_callback_softwareoverrun(rx_data);
    }
    */

}






void Q_Init(Q_T * q) {
  unsigned int i;
  for (i=0; i<Q_SIZE; i++) 
  q->Data[i] = 0; // to simplify our lives when debugging
  q->Head = 0;
  q->Tail = 0;
  q->Size = 0;
  }
  int Q_Empty(Q_T * q) {
  return q->Size == 0;
  }
  int Q_Full(Q_T * q) {
  return q->Size >= Q_SIZE;
}


int Q_Enqueue(Q_T * q, unsigned char d) {
  // What if queue is full?
  if (!Q_Full(q)) {
  q->Data[q->Tail++] = d;
  q->Tail %= Q_SIZE;
  q->Size++;
  return 1; // success
  } else 
  return 0; // failure
}
unsigned char Q_Dequeue(Q_T * q) {
  // Must check to see if queue is empty before dequeueing
  unsigned char t=0;
  if (!Q_Empty(q)) {
  t = q->Data[q->Head];
  q->Data[q->Head++] = 0; // to simplify debugging
  q->Head %= Q_SIZE;
  q->Size--;
  }
  return t;
}






/***********************************************************************************************************************
* Function Name: r_uart1_callback_error
* Description  : This function is a callback function when UART1 reception error occurs.
* Arguments    : err_type -
*                    error type value
* Return Value : None
***********************************************************************************************************************/
static void r_uart1_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart1_callback_receiveend
* Description  : This function is a callback function when UART1 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart1_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
       
        Run_TaskN(2); //decode task
       
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart1_callback_softwareoverrun
* Description  : This function is a callback function when UART1 receives an overflow data.
* Arguments    : rx_data -
*                    receive data
* Return Value : None
***********************************************************************************************************************/
static void r_uart1_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}



