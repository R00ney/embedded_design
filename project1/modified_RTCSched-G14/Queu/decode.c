//decode NMEA functions
//Neal O'Hara

#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"

#include "rtc_sched.h"
#include "lcd.h"
#include "r_cg_serial.h"
#include "queu.h"
#include "tasks.h"


extern Q_T  rx_q;      //recieve queu
extern var_NMEA gps_message;

extern void Debug_Decode(void){
  static char message[100] = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A";
  int i=0;
  char next_char;
  
  for(i=0;message[i]!='\0';i++){
    Q_Enqueue(&rx_q , message[i]);
    
  }
  while(!Q_Empty(&rx_q)){
    next_char = Q_Dequeue(&rx_q );
    DecodeNMEA(& gps_message, next_char);
    i = 0;
  }
  i=0;
  
}


//decode NMEA message like this one:
//$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
//Note, Decode runs on one char from queu rx_q at a time,
//reading sentence from left to right.
void DecodeNMEA(var_NMEA * my, char next_char){
  static char comma = ','; 
  static int temp;

  static char led_state=0;
  LED_6_G = led_state;
  led_state ^= 1;
        
  
 // next_char = Q_Dequeue(& rx_q);
  //determine if check update neccesary
  if( (next_char != '$') && (next_char != '*') && (my->state != CHECK))
    my->check =  my->check ^ next_char; //calculate check of sentence
    
  switch(my->state){
      case(START):
        if(next_char=='$')
        {               
          Init_NMEA(my );
          my->state = SENTENCE;
        }
        else {
          my->state = START;
        }
        break;
      
      //Each NMEA string is a specific sentence type.
      //We only care about RMC sentences
      case(SENTENCE ):
        if(next_char==comma){
          if((my->sentence[2]=='R')&(my->sentence[3]=='M')&(my->sentence[4]=='C'))
            my->state = TIME;
          else
            my->state = START;
        }
        else{
          my->sentence[my->si] = next_char; //store chares between $ and ,
          my->si++;
        }
        break;
       
      //Each RMC sentence has a time fix
      // 123519    =>  Fix taken at 12:35:19 UTC
      case(TIME ):
        if(next_char==comma)
          my->state = STATUS;
        else //store time as int eg. 123519  
          my->time = char2int(my->time, next_char ); 
        break;
      
      //Each RMC sentence has a status flag
      //Status A=active or V=Void.
      case(STATUS) :
        if(next_char==comma)
          my->state = LAT_NUM;
        else if(next_char=='V')
          my->state = START;    //Discard Void sentences
        else
          my->status = next_char; //store single status char
        break;
      
      //Each RMC sentence has a latitude 
      //4807.038,N   Latitude 48 deg 07.038' N
      case(LAT_NUM ):
        if(next_char==comma){
          my->state = LAT_DIR;
          my->lat_dec = FALSE; //reset decimal boolean
        }
        else{
          if(next_char=='.')
            my->lat_dec = TRUE;
          else  //create float of latitude eg. 4807.038
            my->lat_num = char2float(my->lat_num,next_char,&(my->lat_dec));
        }
        break;
        
      case(LAT_DIR ):
        if(next_char==comma)
          my->state = LONG_NUM;
        else
          my->lat_dir = next_char; //store direction char
        break;
        
      //Each RMC sentence has a longitude
      //01131.000,E  Longitude 11 deg 31.000' E
      case(LONG_NUM) :
        if(next_char==comma){
          my->state = LONG_DIR;
          my->long_dec = FALSE; //reset decimal boolean
        }
        else{
          if(next_char=='.')
            my->long_dec = TRUE;
          else  //create float of longitude eg. 01131.000
            my->long_num = char2float(my->long_num,next_char,&(my->long_dec));
        }
        break;
        
      case(LONG_DIR) :
        if(next_char==comma)
          my->state = SPEED;
        else
          my->long_dir = next_char; //store longitude direction char
        break;
      
      //Each RMC sentence has a speed 
        //022.4        Speed over the ground in knots
      case(SPEED ):
        if(next_char==comma){
          my->state = ANGLE;
          my->speed_dec = FALSE; //reset speed_dec
        }
        else{
          if(next_char=='.')
            my->speed_dec = TRUE;
          else  //create float of speed eg. 022.4 
            my->speed = char2float(my->speed,next_char,&(my->speed_dec));
        }  
        break;
      
      //Each RMC sentence has an angle
      //084.4        Track angle in degrees True
      case(ANGLE ):
        if(next_char==comma){
          my->state = DATE;
          my->angle_dec = FALSE; //reset angle_dec
        }
        else{
          if(next_char=='.')
            my->angle_dec = TRUE;
          else  //create float of angle eg. 084.4  
            my->angle = char2float(my->angle,next_char,&(my->angle_dec));
        }  
        break;
      
       //Each RMC sentence has a Date segement
      //230394       Date - 23rd of March 1994
      case(DATE ):
        if(next_char==comma)
          my->state = MAG;
        else //store date as int eg. 230394 
          my->date = char2int(my->date, next_char ); 
        break;
      
      //Each RMC sentence has magnetic strentgh segment
      //003.1,W      Magnetic Variation
      case(MAG) :
        if(next_char==comma){
          my->state = MAG_CHAR;
          my->mag_dec = FALSE; //reset decimal
        }
        else{
          if(next_char=='.')
            my->mag_dec = TRUE;
          else
            my->mag = char2float(my->mag,next_char,&(my->mag_dec));
        }  
        break;
        
      case(MAG_CHAR) :
        if(next_char == '*')
          my->state = CHECK;
        else 
          my->mag_char = next_char;  //store char in mag_char
        break;
        
      case(CHECK ):
        my->checksum[my->ci] = next_char;
        if(my->checksum[my->ci] >= 'A')
          my->checksum[my->ci] = (my->checksum[my->ci]- 55); //convert A-F to 10-15
        else if(my->checksum[my->ci] >= '0')
          my->checksum[my->ci] = (my->checksum[my->ci] - '0');//convert Ox0-9 to 0-9
        my->ci++;
        if(my->ci >= 2){//checksum only has two chars
          my->state = START;
          temp = (my->checksum[0]<<4|my->checksum[1]); //shift checksum chars to match check format
          if(temp == my->check)  //Discard Sentences that fail checksum
            Run_TaskN(1);
        }
        break;
  }

  LED_6_G = led_state;
  led_state ^= 1;
  
  
}//end DecodeNMEA

//shift value by 10s place, and add char's numeric value
double char2int(double value, char c){
  return (value*10 + c - '0'); //convert next char ascii to int
}

//shift whole value left by 10s place, after decimal
// shift fractions right by 10s places
float char2float(float value, char c, int * decimal){

  //decimal portion
   if(*decimal > 0){ //after comma, decimal =1, so *10 for a divede by 10
    *decimal = (*decimal)*10;
    return (value + ((float)(c - '0'))/(*decimal));
   }
  else
    return (value*10 + c - '0'); 
}

//used to setup new NMEA structs
void Init_NMEA(var_NMEA * n ){
  n->state = START;
  // sentence[6];
  n->si = 0;
  n->time = 0;
  // status;
  n->lat_num = 0;
  n->lat_dec = FALSE;
  //lat_dir;
  n->long_num = 0;
  n->long_dec = FALSE;
  //long_dir;
  n->speed = 0;
  n->speed_dec = FALSE;
  n->angle = 0;
  n->angle_dec = FALSE;
  n->date = 0;
  n->mag = 0;
  n->mag_dec = FALSE;
  //mag_char;
  //checksum[2];
  n->ci = 0;
  n->check = 0;    //reset interal check for checksum compare

}
