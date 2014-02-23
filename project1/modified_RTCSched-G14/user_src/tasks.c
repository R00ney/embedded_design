#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"

#include "rtc_sched.h"
#include "lcd.h"
#include "r_cg_serial.h"
#include "queu.h"
#include "tasks.h"

extern Q_T  rx_q;      //recieve queu
 var_NMEA gps_message;

//Task_GPS_Update_LCD
void Task1(void) {
	static char led_state=0;
        
        int deg = 0;
        float min = 0;
        int h=0;
        int m=0;
        int s=0;
        
	LED_4_G = led_state;
	led_state ^= 1;
        
        //lat
        deg = (int)(gps_message.lat_num/100);
        min = (float)(gps_message.lat_num - deg*100);
        LCDPrintf(4,0,"%03d %06.3f %c",deg,min,gps_message.lat_dir);
        
        //long
        deg = (int)(gps_message.long_num/100);
        min = (float)(gps_message.long_num - deg*100);
        LCDPrintf(5,0,"%03d %06.3f %c",deg,min,gps_message.long_dir);
        
        //speed
        LCDPrintf(6,0,"%06.2f  kt",gps_message.speed);
        
        //track
        LCDPrintf(7,0,"%06.2f  T",gps_message.angle);
        
        //time
        h = (int)(gps_message.time/10000);
        m = (int)((gps_message.time - h*10000)/100);
        s = (int)(((gps_message.time - h*10000)-m*100)); 
        LCDPrintf(8,0,"%02u:%02u:%02u",h,m,s);
}

//Decode Task
void Task2(void) {
	//static char led_state=0;
        char next_char;
        
	//LED_6_G = led_state;
	//led_state ^= 1;
        
              //ReStart Serial Recieve 
      //uint8_t * const rx_buf, uint16_t rx_num
       uint8_t * rx_address;
       uint8_t rx_buff[2];
       rx_address = rx_buff; 
      R_UART1_Receive(rx_address, 2);
            
        next_char = Q_Dequeue(& rx_q);
        DecodeNMEA(& gps_message,next_char);
        

}

//Update LCD Task
// Task3 runs at 1 Hz, updates elapsed time on LCD
void Task3(void) {
	//static char led_state=0;
	static unsigned char h=0, m=0, s=0, ms=0;

	//LED_8_G = led_state;
	//led_state ^= 1;

        ms++;
        if(ms>=10){
          s++;
          ms = 0;
          if (s>59) {
                  m++;
                  s = 0;
                  if(ms>60){
                    h++;
                    m=0;
                  }
          }
        }
        
	LCDPrintf(3,0, "%02u:%02u:%02u.%01u",h, m, s,ms);
}









