#ifndef _BoardMgr_H
#define _BoardMgr_H


//static char IRCUT_EN_flag;
extern unsigned int get_flag_WTD_reboot(void);
extern void Factory_Default(void);

extern void Board_Init(void); 
         
extern void Rotate_Left(void);         
extern void Rotate_Right(void);        
extern void Rotate_Up(void);           
extern void Rotate_Down(void);          
extern void Motor_Stop(void);
extern void Motor_Inital(void);  

extern int Key_Scan(void);        

extern void IRCUT_Night(void);
extern void IRCUT_Day(void);
extern void IRCUT_EN(void);
extern void IRCUT_DIS(void);
extern void IRCUT_Ctrl(void);
extern void set_IR_LED_EN(unsigned int onoff);
 
extern void LED_ON_Red(void);              
extern void LED_ON_Blue(void);
                 
extern void LED_OFF_Red(void);             
extern void LED_OFF_Blue(void); 
                 
extern void LED_BLINK_Red(int times, int delay);          
extern void LED_BLINK_Blue(int times, int delay);
             
extern void LED_TOGGLE_Red(void);          
extern void LED_TOGGLE_Blue(void);  

#if EZHOMELABS_CAMERA

static int HardWare_Version = 0;
//static char PIR_EN_flag;
//extern void PIR_EN(void);
//extern void PIR_DIS(void);
extern int PIR_Scan(void);
extern int get_key_value(void);
extern void IR_Light_Blink(unsigned int times, unsigned int timedelay);
extern void set_led_delay(unsigned int val);
extern unsigned int get_led_delay(void);

extern void SPK_PWON(void);
extern void SPK_SHUT(void);
extern int get_echo_en(void);
extern void reset_echo(void);
extern void set_echo_bypass(int val);

extern int Uart_test(void);
#else

extern void sensor_check(void);
#if 1
extern void Motor0_Stop(void);
extern void Motor1_Stop(void);
extern void Tilt_turn(unsigned int dir);
extern void Pan_turn(unsigned int dir);
#endif
#endif

#if !EZHOMELABS_CAMERA
extern void set_motion_x(int x);
extern void set_motion_y(int y);
extern int get_motion_x(void);
extern int get_motion_y(void);
extern void set_motion_x_en(int x);
extern void set_motion_y_en(int y);
extern int get_motion_x_en(void);
extern int get_motion_y_en(void);
extern void set_turn_x_en(int en);
extern void set_turn_y_en(int en);
extern int get_turn_x_en(void);
extern int get_turn_y_en(void);
extern void set_motion_turn(int en);
extern int get_motion_turn(void);
#endif

extern void initAdvance();
extern void updateAdvance(int iAdvance);

#endif //_BoardMgr_H
