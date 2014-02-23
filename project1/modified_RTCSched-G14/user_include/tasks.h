#ifndef TASKS_H
#define TASKS_H

typedef struct {
  int state;
  char sentence[6];
  int si ;
  double time ;
  char status;
  float lat_num ;
  int lat_dec ;
  char lat_dir;
  float long_num ;
  int long_dec;
  char long_dir;
  float speed ;
  int speed_dec ;
  float angle ;
  int angle_dec ;
  double date;
  float mag ;
  int mag_dec ;
  char mag_char;
  char checksum[2];
  int ci ;
  char check;
  
} var_NMEA;

extern void Task1(void);
extern void Task2(void);
extern void Task3(void);
extern void DecodeNMEA(var_NMEA * my,char next_char);
extern double char2int(double value, char c);
extern float char2float(float value, char c, int * decimal);

//State Defines
#define START 0
#define SENTENCE 1
#define TIME 2
#define STATUS 3
#define LAT_NUM 4
#define LAT_DIR 5
#define LONG_NUM 6
#define LONG_DIR 7
#define SPEED 8
#define ANGLE 9
#define DATE 10
#define MAG 11
#define MAG_CHAR 12
#define CHECK 13
#define TRUE 1
#define FALSE 0

extern void Init_NMEA(var_NMEA * n);
extern void Debug_Decode(void);

  
#endif // TASKS_H