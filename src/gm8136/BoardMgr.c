#include "BoardMgr.h"
#include "haicam/Config.h"
#include "haicam/Utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "ioctl_isp328.h"

#define GPIO0_BASE 0x91000000
#define GPIO1_BASE 0x91900000

#define GPIODataOut_Offset 0x00
#define GPIODataIn_Offset 0x04
#define PinDir_Offset 0x08
#define PinPullEnable_Offset 0x18
#define PinPullType_Offset 0x1C
#define GPIO_BLOCK 12 // offset 0 to 12

#define IR_PWREN 31
#define IRCUT_1 28
#define IRCUT_2 29

volatile uint32_t *GPIO0_BaseReg = 0;
volatile uint32_t *GPIO0_PinDirReg = 0;
volatile uint32_t *GPIO0_OutReg = 0;
volatile uint32_t *GPIO0_InReg = 0;
volatile uint32_t *GPIO0_PinPullENReg = 0;
volatile uint32_t *GPIO0_PinPullUpReg = 0;

volatile uint32_t *GPIO1_BaseReg = 0;
volatile uint32_t *GPIO1_PinDirReg = 0;
volatile uint32_t *GPIO1_OutReg = 0;
volatile uint32_t *GPIO1_InReg = 0;
volatile uint32_t *GPIO1_PinPullENReg = 0;
volatile uint32_t *GPIO1_PinPullUpReg = 0;

char LED_State_Red = 0;
char LED_State_Blue = 0;
char IRCUT_EN_flag = 0;
unsigned int IR_LED_EN = 1;
unsigned int IR_mode = 1; // 1 day, 0 night

void delayms(int ms);
static void GPIO_READ(void);
static void Set_GPIO0IN(int Pin_num);
static void Set_GPIO1IN(int Pin_num);
static int GPIO0IN_Value(int Pin_num);
static int GPIO1IN_Value(int Pin_num);
static void Set_GPIO0OUT(int Pin_num);
static void Set_GPIO1OUT(int Pin_num);
static void GPIO0OUT_Value(int Pin_num, int Value);
static void GPIO1OUT_Value(int Pin_num, int Value);

void LED_ON_Red(void);
void LED_ON_Blue(void);
void LED_OFF_Red(void);
void LED_OFF_Blue(void);
void LED_TOGGLE_Red(void);
void LED_TOGGLE_Blue(void);

#define SCU_BASE_REG 0x90c00000
#define SCU_BLOCK 0xFF
unsigned int WTD_Reboot = 0;

#if EZHOMELABS_CAMERA

#define BIT_0 1
#define BIT_1 2
#define BIT_2 3
#define PIR_IN 13
#define KEY_IN 16
#define Red_LED1 14
#define ByPass_EN 15
#define Blue_LED2 17
#define WIFI_EN 24
#define ECHO_RST 26
#define SPK_EN 27

unsigned int led_delay = 0;
int echo_en_flag = 0;
int HW_Version(void);
void SPK_PWON(void);
void SPK_SHUT(void);

int uart_fd = 0;
char read_buf[8], send_buf[8];

#else

#define VER1 4

#define Motor0_1 22
#define Motor0_2 21
#define Motor0_3 20
#define Motor0_4 19
#define Motor1_1 30
#define Motor1_2 29
#define Motor1_3 28
#define Motor1_4 23

#define Left_Pin 2
#define Right_Pin 1
#define Up_Pin 4
#define Down_Pin 26
#define Button 16
#define LED_Red 14
#define LED_Blue 15

unsigned int day = 1;
unsigned int night = 1;
int motor_stop = 0;
int HW_VER1 = 1;
int sensor_id = 0;
void sensor_check(void);

#endif

void delayms(int ms)
{
    usleep(1000 * ms);
}

static void GPIO_READ(void)
{
    int m_mfd;
    if ((m_mfd = open(haicam_cfgGetMemDevice(), O_RDWR)) < 0)
    {
        //        printf("cannot open file: /dev/mem \n");
        //		return -1;
    }
    GPIO0_BaseReg = (uint32_t *)mmap(NULL, GPIO_BLOCK, PROT_READ | PROT_WRITE, MAP_SHARED, m_mfd, GPIO0_BASE);
    GPIO1_BaseReg = (uint32_t *)mmap(NULL, GPIO_BLOCK, PROT_READ | PROT_WRITE, MAP_SHARED, m_mfd, GPIO1_BASE);
    unsigned char *reg_tmp = (unsigned char *)mmap(NULL, SCU_BLOCK, PROT_READ | PROT_WRITE, MAP_SHARED, m_mfd, SCU_BASE_REG);
    close(m_mfd);
    if ((GPIO0_BaseReg == MAP_FAILED) || (GPIO1_BaseReg == MAP_FAILED))
    {
        //        printf("mmap failed \n");
        //		return -2;
    }

    GPIO0_PinDirReg = GPIO0_BaseReg + PinDir_Offset / 4;
    GPIO0_PinPullENReg = GPIO0_BaseReg + PinPullEnable_Offset / 4;
    GPIO0_PinPullUpReg = GPIO0_BaseReg + PinPullType_Offset / 4;
    GPIO0_OutReg = GPIO0_BaseReg + GPIODataOut_Offset / 4;
    GPIO0_InReg = GPIO0_BaseReg + GPIODataIn_Offset / 4;

    GPIO1_PinDirReg = GPIO1_BaseReg + PinDir_Offset / 4;
    GPIO1_PinPullENReg = GPIO1_BaseReg + PinPullEnable_Offset / 4;
    GPIO1_PinPullUpReg = GPIO1_BaseReg + PinPullType_Offset / 4;
    GPIO1_OutReg = GPIO1_BaseReg + GPIODataOut_Offset / 4;
    GPIO1_InReg = GPIO1_BaseReg + GPIODataIn_Offset / 4;

    WTD_Reboot = *(volatile unsigned int *)(reg_tmp + 0x20);
}

static void Set_GPIO0IN(int Pin_num)
{
    *GPIO0_PinDirReg &= ~(1l << Pin_num);   // Set PIN Input
    *GPIO0_PinPullENReg |= (1l << Pin_num); // PIN is Pulled
    *GPIO0_PinPullUpReg |= (1l << Pin_num); // Pin Pull Up
}

static void Set_GPIO1IN(int Pin_num)
{
    *GPIO1_PinDirReg &= ~(1l << Pin_num);   // Set PIN Input
    *GPIO1_PinPullENReg |= (1l << Pin_num); // PIN is Pulled
    *GPIO1_PinPullUpReg |= (1l << Pin_num); // Pin Pull Up
}

static int GPIO0IN_Value(int Pin_num)
{
    return (*GPIO0_InReg & (1l << Pin_num)) > 0 ? 1 : 0;
}

static int GPIO1IN_Value(int Pin_num)
{
    return (*GPIO1_InReg & (1l << Pin_num)) > 0 ? 1 : 0;
}

static void Set_GPIO0OUT(int Pin_num)
{
    *GPIO0_PinDirReg |= (1l << Pin_num); // Set PIN Output
}

static void Set_GPIO1OUT(int Pin_num)
{
    *GPIO1_PinDirReg |= (1l << Pin_num); // Set PIN Output
}

static void GPIO0OUT_Value(int Pin_num, int Value)
{
    if (Value == 1)
        *GPIO0_OutReg |= (1l << Pin_num);
    if (Value == 0)
        *GPIO0_OutReg &= ~(1l << Pin_num);
}

static void GPIO1OUT_Value(int Pin_num, int Value)
{
    if (Value == 1)
        *GPIO1_OutReg |= (1l << Pin_num);
    if (Value == 0)
        *GPIO1_OutReg &= ~(1l << Pin_num);
}

void Factory_Default(void)
{
    //TODO
    //resetAccount();
	FILE* fp;
	fp = popen(haicam_cfgGetShellFactoryDefault(), "r");
	pclose(fp);
}

unsigned int get_flag_WTD_reboot(void)
{
    return (WTD_Reboot & (1 << 9)) > 0 ? 1 : 0;
}

#if EZHOMELABS_CAMERA

int HW_Version(void)
{
    int bit0 = 0, bit1 = 0, bit2 = 0, version_num = 0;
    bit0 = GPIO0IN_Value(BIT_0);
    bit1 = GPIO0IN_Value(BIT_1);
    bit2 = GPIO0IN_Value(BIT_2);
    //    if(!bit2)
    //    {
    //        printf("CMOS = 1080P  ");
    //    }
    //    else     printf("CMOS = 720P  ");
    version_num = bit1 * 2 + bit0;
    // printf("HW_Version = %d\n", version_num);
    return version_num;
}

//************************Motor Control***************************
// Motor0
//**********************************
//             Right
// Step  M0_1  M0_2  M0_3  M0_4
//   1     1     0     0     1
//   2     1     1     0     0
//   3     0     1     1     0
//   4     0     0     1     1
//             Left
//   1     0     0     1     1
//   2     0     1     1     0
//   3     1     1     0     0
//   4     1     0     0     1
//**********************************
void Rotate_Left(void)
{
    delayms(1);
}

void Rotate_Right(void)
{
    delayms(1);
}

// Motor1
//**********************************
//               Up
//  Step  M1_1  M1_2  M1_3  M1_4
//    1     1     0     0     1
//    2     1     1     0     0
//    3     0     1     1     0
//    4     0     0     1     1
//              Down
//    1     0     0     1     1
//    2     0     1     1     0
//    3     1     1     0     0
//    4     1     0     0     1
//**********************************

void Rotate_Up(void)
{
    delayms(1);
}

void Rotate_Down(void)
{
    delayms(1);
}

void Motor_Stop(void)
{
    delayms(1);
}

void Motor_Inital(void)
{
    delayms(1);
}

int get_key_value(void)
{
    return GPIO0IN_Value(KEY_IN);
}

int Key_Scan(void)
{
    FILE *fp;
    char buffer[8];
    int Key_Read_Value;
    int Key_Press = 0;
    int Key_Num = 0;
    Key_Read_Value = GPIO0IN_Value(KEY_IN);
    if (!Key_Read_Value)
    {
        delayms(20);
        Key_Read_Value = GPIO0IN_Value(KEY_IN);
        if (!Key_Read_Value)
        {
            Key_Press = 1;
            while (!Key_Read_Value)
            {
                delayms(20);
                Key_Num++;
                if (Key_Num > 390)
                {
                    fp = fopen("/tmp/timeout", "r");
                    if (fp != NULL)
                    {
                        fscanf(fp, "%s", buffer);
                        // printf("%s\n", buffer);
                        if ((strcmp(buffer, "timein")) == 0)
                        {
                            Key_Num = 0;
                            Factory_Default();
                        }
                        else
                        {
                            Key_Num = 0;
                        }
                    }
                    else
                    {
                        // printf("Can not open timeout\n");
                    }
                    fclose(fp);
                }
                Key_Read_Value = GPIO0IN_Value(KEY_IN);
            }
            Key_Num = 0;
        }
    }
    else
        Key_Press = 0;

    return Key_Press;
}

/*void PIR_EN(void)
{
    PIR_EN_flag = 1;
}

void PIR_DIS(void)
{
    PIR_EN_flag = 0;
}*/
int PIR_Scan(void)
{
    int PIR_Read_Value;
    int PIR_Alarm = 0;
    // if(PIR_EN_flag)
    //{
    PIR_Read_Value = GPIO0IN_Value(PIR_IN);
    if (!PIR_Read_Value)
    {
        delayms(50);
        PIR_Read_Value = GPIO0IN_Value(PIR_IN);
        if (!PIR_Read_Value)
        {
            delayms(10);
            PIR_Alarm = 1;
        }
    }
    else
        PIR_Alarm = 0;
    /*}
    else
    {
        delayms(200);
        PIR_Alarm = 0;
    }*/
    return PIR_Alarm;
}

void IR_Light_Blink(unsigned int times, unsigned int timedelay)
{
    unsigned int i;
    for (i = 0; i < times; i++)
    {
        GPIO0OUT_Value(IR_PWREN, 1);
        delayms(timedelay);
        GPIO0OUT_Value(IR_PWREN, 0);
        delayms(timedelay);
    }
}

void set_IR_LED_EN(unsigned int onoff) // 1 IR LED on, 0 IR LED off
{
    if (onoff)
    {
        IR_LED_EN = 1;
        if (!IR_mode)
        {
            GPIO0OUT_Value(IR_PWREN, 1);
        }
    }
    else
    {
        IR_LED_EN = 0;
        GPIO0OUT_Value(IR_PWREN, 0);
    }
}

unsigned int get_IR_LED_EN(void)
{
    return IR_LED_EN;
}

void IRCUT_Night(void)
{
    if (get_IR_LED_EN())
    {
        GPIO0OUT_Value(IR_PWREN, 1);
    }
    GPIO1OUT_Value(IRCUT_2, 1);
    delayms(50);
    // printf("Dimmer light, Filter full penetration\n");
    GPIO1OUT_Value(IRCUT_2, 0);
    IR_mode = 0;
}

void IRCUT_Day(void)
{
    GPIO0OUT_Value(IR_PWREN, 0);
    GPIO1OUT_Value(IRCUT_1, 1);
    delayms(50);
    // printf("Sufficient light, Filter Cut\n");
    GPIO1OUT_Value(IRCUT_1, 0);
    IR_mode = 1;
}

void IRCUT_EN(void)
{
    IRCUT_EN_flag = 1;
}

void IRCUT_DIS(void)
{
    IRCUT_EN_flag = 0;
}

void IRCUT_Ctrl(void)
{
    int key_fd = -1;
    struct timeval tv;
    fd_set rd;
    unsigned int data, num = 0, tmp = 0;
    unsigned int day = 1;
    unsigned int night = 1;

    if ((key_fd = open(haicam_cfgGetIRCtrlDevice(), O_RDONLY | O_NONBLOCK)) > 0)
    {
        // printf("key_fd = %d\n", key_fd);
        while (key_fd)
        {
            if (IRCUT_EN_flag == 1)
            {
                tv.tv_sec = 0;
                tv.tv_usec = 500000; /* timeout 500 ms */
                FD_ZERO(&rd);
                FD_SET(key_fd, &rd);
                select(key_fd + 1, &rd, NULL, NULL, &tv);
                if (read(key_fd, &data, sizeof(unsigned int)) > 0)
                {
                    delayms(10);
                    tmp = tmp + data;
                }
                num++;
                if (num == 12)
                {
                    tmp = tmp / num;
                    // printf("tmp = %d\n", tmp);
                    if (tmp > 149)
                    {
                        if (night == 1)
                        {
                            updateAdvance(1);
                            IRCUT_Night();
                            day = 1;
                            night = 0;
                        }
                    }
                    else if (tmp < 50) // 53
                    {
                        if (day == 1)
                        {
                            updateAdvance(0);
                            IRCUT_Day();
                            day = 0;
                            night = 1;
                        }
                    }
                    num = 0;
                    tmp = 0;
                }
                delayms(350);
            }
            else
            {
                num = 0;
                tmp = 0;
                delayms(3000);
                // printf("DIsable IRCUT\n");
            }
        }
    }
    else
    {
        printf("Can't open sar_adc_drv\n");
        // return -1;
    }
    close(key_fd);
}

void set_led_delay(unsigned int val)
{
    if (val)
        led_delay += val;
    else
        led_delay = 0;
}

unsigned int get_led_delay(void)
{
    return led_delay;
}

void LED_ON_Red(void)
{
    GPIO0OUT_Value(Red_LED1, 1);
    LED_State_Red = 1;
}

void LED_ON_Blue(void)
{
    GPIO0OUT_Value(Blue_LED2, 0);
    LED_State_Blue = 1;
}

void LED_OFF_Red(void)
{
    GPIO0OUT_Value(Red_LED1, 0);
    LED_State_Red = 0;
}

void LED_OFF_Blue(void)
{
    GPIO0OUT_Value(Blue_LED2, 1);
    LED_State_Blue = 0;
}

void LED_TOGGLE_Red(void)
{
    if (LED_State_Red)
        LED_OFF_Red();
    else
        LED_ON_Red();
}

void LED_TOGGLE_Blue(void)
{
    if (LED_State_Blue)
        LED_OFF_Blue();
    else
        LED_ON_Blue();
}

void LED_BLINK_Red(int times, int delay)
{
    int i;
    for (i = 0; i < times; i++)
    {
        LED_TOGGLE_Red();
        delayms(delay);
    }
}

void LED_BLINK_Blue(int times, int delay)
{
    int i;
    for (i = 0; i < times; i++)
    {
        LED_TOGGLE_Blue();
        delayms(delay);
    }
}

void SPK_PWON(void)
{
    GPIO1OUT_Value(SPK_EN, 0);
}

void SPK_SHUT(void)
{
    GPIO1OUT_Value(SPK_EN, 1);
}

void set_echo_en(void)
{
    echo_en_flag = GPIO0IN_Value(ByPass_EN);
    delayms(5);
    Set_GPIO0OUT(ByPass_EN);
}

int get_echo_en(void)
{
    return echo_en_flag;
}

void reset_echo(void)
{
    GPIO1OUT_Value(ECHO_RST, 0);
    delayms(10);
    GPIO1OUT_Value(ECHO_RST, 1);
}

void set_echo_bypass(int val)
{
    if (val)
    {
        GPIO0OUT_Value(ByPass_EN, 1);
        delayms(1);
        GPIO0OUT_Value(ByPass_EN, 0);
        delayms(10);
        GPIO0OUT_Value(ByPass_EN, 1);
    }
    else
        GPIO0OUT_Value(ByPass_EN, 0);
}

void Board_Init(void)
{
    GPIO_READ();
    Set_GPIO0IN(BIT_0);
    Set_GPIO0IN(BIT_1);
    Set_GPIO0IN(BIT_2);
    Set_GPIO0IN(PIR_IN);
    Set_GPIO0IN(KEY_IN);
    Set_GPIO0IN(ByPass_EN);
    Set_GPIO0OUT(Red_LED1);
    Set_GPIO0OUT(Blue_LED2);
    Set_GPIO0OUT(IR_PWREN);
    Set_GPIO1OUT(IRCUT_1);
    Set_GPIO1OUT(IRCUT_2);
    Set_GPIO1OUT(WIFI_EN);
    // Set_GPIO1OUT(ECHO_RST);
    Set_GPIO1OUT(SPK_EN);
    // reset_echo();
    GPIO0OUT_Value(Red_LED1, 0);
    GPIO0OUT_Value(Blue_LED2, 0);
    GPIO1OUT_Value(WIFI_EN, 0);
    // GPIO1OUT_Value(ECHO_RST, 1);
    GPIO1OUT_Value(SPK_EN, 1);
    GPIO0OUT_Value(IR_PWREN, 0);
    GPIO1OUT_Value(IRCUT_1, 1);
    GPIO1OUT_Value(IRCUT_2, 0);
    delayms(50);
    GPIO1OUT_Value(IRCUT_1, 0);
    HardWare_Version = HW_Version();
    set_echo_en();
    LED_State_Red = 0;
    LED_State_Blue = 1;
    IR_LED_EN = 1;
    IR_mode = 1;
    IRCUT_EN_flag = 1;
    // PIR_EN_flag = 1;
}

////////////////////////////////////////////////////////////
//  UART_Init
//  Uart2 Initial
//  @
//////////////////////////////////////////////////////////
int UART_Init(void)
{
    int bits, i;
    struct termios options;

    uart_fd = open(ZWave_Port, O_RDWR | O_NOCTTY | O_NDELAY, 0); // Open Uart2
    if (-1 == uart_fd)
    {
        close(uart_fd);
        printf("Can not open Serial Port0\n");
        return -1;
    }

    bits = 0;
    ioctl(uart_fd, TIOCMSET, &bits);

    bzero(&options, sizeof(options)); // Clear and Reset
    tcgetattr(uart_fd, &options);     // Configure Serial Port options
    cfsetospeed(&options, B115200);   // Baud Rate is 115200
    cfsetispeed(&options, B115200);
    // cfsetspeed( &options, B115200 );
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8 | CREAD | CLOCAL; // Data Length is 8 bit
    options.c_cflag &= ~CSTOPB;              // Stop bit is 1
    options.c_cflag &= ~PARENB;              // No Parity
    options.c_iflag &= ~INPCK;
    options.c_iflag |= IGNBRK;
    options.c_oflag = 0;
    options.c_lflag = 0;
    for (i = 0; i < NCCS; i++)
        options.c_cc[i] = 0;
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 0;

    tcflush(uart_fd, TCIOFLUSH);
    tcsetattr(uart_fd, TCSANOW, &options);

    return uart_fd;
}

/////////////////////////////////////////////////////////////
//  UART_Read
//  Read data from the Serial Port
//  @
////////////////////////////////////////////////////////////
int UART_Read()
{
    int i, byteread, retval;
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(uart_fd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 5000;
    while (FD_ISSET(uart_fd, &rfds))
    {
        FD_ZERO(&rfds);
        FD_SET(uart_fd, &rfds);
        retval = select(uart_fd + 1, &rfds, NULL, NULL, &tv);
        if (retval)
        {
            byteread = read(uart_fd, read_buf, sizeof(read_buf));
            if (byteread > 0)
            {
                for (i = 0; i < byteread; i++)
                    printf("%02X ", read_buf[i]);
                printf("\n");
            }
        }
    }
    return byteread;
}

/////////////////////////////////////////////////////////////
//  UART_Send
//  Serial Port sent data
//  @DataBuff: data waiting for Transmission
//  @DataLen: data length
////////////////////////////////////////////////////////////
void UART_Send(char *buf, int buf_len)
{
    int j, ret;

    ret = write(uart_fd, buf, buf_len);
    if (buf_len == ret)
    {
        for (j = 0; j < buf_len; j++)
            printf("%02X ", *(buf + j));
        printf("\n");
    }
    else
    {
        tcflush(uart_fd, TCOFLUSH);
        printf("Send Error");
    }
}

int Uart_test(void)
{
    int i, result = 0;
    sprintf(read_buf, "");
    send_buf[0] = 0x01;
    send_buf[1] = 0x23;
    send_buf[2] = 0x45;
    send_buf[3] = 0x67;
    send_buf[4] = 0x89;
    send_buf[5] = 0xAB;
    send_buf[6] = 0xCD;
    send_buf[7] = 0xEF;
    UART_Init();
    for (i = 0; i < 10; i++)
    {
        printf("Send: ");
        UART_Send(send_buf, 8);
        usleep(10000);
        printf("Read: ");
        UART_Read();
        if ((memcmp(read_buf, send_buf, 8)) == 0)
        {
            result = 1;
            printf("------------PASS------------\n");
        }
        usleep(200000);
    }
    close(uart_fd);

    return result;
}

#else

void sensor_check(void)
{
    FILE *fp;
    char buffer[8];
    fp = fopen("/tmp/sensor_id", "r");
    if (fp != NULL)
    {
        fscanf(fp, "%s", buffer);
        // printf("%s\n", buffer);
        if ((strcmp(buffer, "ov9732")) == 0)
        {
            sensor_id = 1;
        }
        else if ((strcmp(buffer, "ps5230")) == 0)
        {
            sensor_id = 2;
        }
        else if ((strcmp(buffer, "ov9715")) == 0)
        {
            sensor_id = 3;
        }
    }
    else
    {
        // printf("Can not open sensor_id\n");
    }
    fclose(fp);
}
// sensor_id = ov9732->1/ps5230->2

//************************Motor Control***************************
// Motor0
//**********************************
//             Right
// Step  M0_1  M0_2  M0_3  M0_4
//   1     1     0     0     1
//   2     1     1     0     0
//   3     0     1     1     0
//   4     0     0     1     1
//             Left
//   1     0     0     1     1
//   2     0     1     1     0
//   3     1     1     0     0
//   4     1     0     0     1
//**********************************
#if 1
void Tilt_turn(unsigned int dir)
{
    unsigned int step = 0;
    if (dir)
    { // turn left
        while (step < 28)
        {
            GPIO0OUT_Value(Motor1_1, 0);
            GPIO0OUT_Value(Motor1_2, 0);
            GPIO0OUT_Value(Motor1_3, 1);
            GPIO1OUT_Value(Motor1_4, 1);
            delayms(2);
            GPIO0OUT_Value(Motor1_1, 0);
            GPIO0OUT_Value(Motor1_2, 1);
            GPIO0OUT_Value(Motor1_3, 1);
            GPIO1OUT_Value(Motor1_4, 0);
            delayms(2);
            GPIO0OUT_Value(Motor1_1, 1);
            GPIO0OUT_Value(Motor1_2, 1);
            GPIO0OUT_Value(Motor1_3, 0);
            GPIO1OUT_Value(Motor1_4, 0);
            delayms(2);
            GPIO0OUT_Value(Motor1_1, 1);
            GPIO0OUT_Value(Motor1_2, 0);
            GPIO0OUT_Value(Motor1_3, 0);
            GPIO1OUT_Value(Motor1_4, 1);
            delayms(2);
            step++;
        }
    }
    else
    { // turn right
        while (step < 28)
        {
            GPIO0OUT_Value(Motor1_1, 1);
            GPIO0OUT_Value(Motor1_2, 0);
            GPIO0OUT_Value(Motor1_3, 0);
            GPIO1OUT_Value(Motor1_4, 1);
            delayms(2);
            GPIO0OUT_Value(Motor1_1, 1);
            GPIO0OUT_Value(Motor1_2, 1);
            GPIO0OUT_Value(Motor1_3, 0);
            GPIO1OUT_Value(Motor1_4, 0);
            delayms(2);
            GPIO0OUT_Value(Motor1_1, 0);
            GPIO0OUT_Value(Motor1_2, 1);
            GPIO0OUT_Value(Motor1_3, 1);
            GPIO1OUT_Value(Motor1_4, 0);
            delayms(2);
            GPIO0OUT_Value(Motor1_1, 0);
            GPIO0OUT_Value(Motor1_2, 0);
            GPIO0OUT_Value(Motor1_3, 1);
            GPIO1OUT_Value(Motor1_4, 1);
            delayms(2);
            step++;
        }
    }
}
void Pan_turn(unsigned int dir)
{
    unsigned int step = 0;
    if (dir)
    { // turn up
        while (step < 8)
        {
            GPIO1OUT_Value(Motor0_1, 0);
            GPIO1OUT_Value(Motor0_2, 0);
            GPIO1OUT_Value(Motor0_3, 1);
            GPIO1OUT_Value(Motor0_4, 1);
            delayms(4);
            GPIO1OUT_Value(Motor0_1, 0);
            GPIO1OUT_Value(Motor0_2, 1);
            GPIO1OUT_Value(Motor0_3, 1);
            GPIO1OUT_Value(Motor0_4, 0);
            delayms(4);
            GPIO1OUT_Value(Motor0_1, 1);
            GPIO1OUT_Value(Motor0_2, 1);
            GPIO1OUT_Value(Motor0_3, 0);
            GPIO1OUT_Value(Motor0_4, 0);
            delayms(4);
            GPIO1OUT_Value(Motor0_1, 1);
            GPIO1OUT_Value(Motor0_2, 0);
            GPIO1OUT_Value(Motor0_3, 0);
            GPIO1OUT_Value(Motor0_4, 1);
            delayms(4);
            step++;
        }
    }
    else
    { // turn down
        while (step < 8)
        {
            GPIO1OUT_Value(Motor0_1, 1);
            GPIO1OUT_Value(Motor0_2, 0);
            GPIO1OUT_Value(Motor0_3, 0);
            GPIO1OUT_Value(Motor0_4, 1);
            delayms(4);
            GPIO1OUT_Value(Motor0_1, 1);
            GPIO1OUT_Value(Motor0_2, 1);
            GPIO1OUT_Value(Motor0_3, 0);
            GPIO1OUT_Value(Motor0_4, 0);
            delayms(4);
            GPIO1OUT_Value(Motor0_1, 0);
            GPIO1OUT_Value(Motor0_2, 1);
            GPIO1OUT_Value(Motor0_3, 1);
            GPIO1OUT_Value(Motor0_4, 0);
            delayms(4);
            GPIO1OUT_Value(Motor0_1, 0);
            GPIO1OUT_Value(Motor0_2, 0);
            GPIO1OUT_Value(Motor0_3, 1);
            GPIO1OUT_Value(Motor0_4, 1);
            delayms(4);
            step++;
        }
    }
}
#endif
void Motor0_Stop(void)
{
    if (HW_VER1 == 0)
    {
        GPIO1OUT_Value(Motor1_1, 0);
        GPIO1OUT_Value(Motor1_2, 0);
        GPIO1OUT_Value(Motor1_3, 0);
        GPIO1OUT_Value(Motor1_4, 0);
    }
    else
    {
        GPIO1OUT_Value(Motor0_1, 0);
        GPIO1OUT_Value(Motor0_2, 0);
        GPIO1OUT_Value(Motor0_3, 0);
        GPIO1OUT_Value(Motor0_4, 0);
    }
}

void Rotate_Left(void)
{
    int Left_Value;
    unsigned int step = 0;
    set_turn_x_en(0);
    set_turn_y_en(0);
    if (HW_VER1 == 0)
    {
        while (step < 46) // 36
        {
            GPIO0OUT_Value(Motor1_1, 0);
            GPIO0OUT_Value(Motor1_2, 0);
            GPIO0OUT_Value(Motor1_3, 1);
            GPIO1OUT_Value(Motor1_4, 1);
            delayms(4);
            GPIO0OUT_Value(Motor1_1, 0);
            GPIO0OUT_Value(Motor1_2, 1);
            GPIO0OUT_Value(Motor1_3, 1);
            GPIO1OUT_Value(Motor1_4, 0);
            delayms(4);
            GPIO0OUT_Value(Motor1_1, 1);
            GPIO0OUT_Value(Motor1_2, 1);
            GPIO0OUT_Value(Motor1_3, 0);
            GPIO1OUT_Value(Motor1_4, 0);
            delayms(4);
            GPIO0OUT_Value(Motor1_1, 1);
            GPIO0OUT_Value(Motor1_2, 0);
            GPIO0OUT_Value(Motor1_3, 0);
            GPIO1OUT_Value(Motor1_4, 1);
            delayms(4);
            step++;
        }
    }
    else
    {
        Left_Value = GPIO0IN_Value(Left_Pin);
        if (Left_Value)
        {
            while (step < 20)
            {
                GPIO1OUT_Value(Motor0_1, 0);
                GPIO1OUT_Value(Motor0_2, 0);
                GPIO1OUT_Value(Motor0_3, 1);
                GPIO1OUT_Value(Motor0_4, 1);
                delayms(4);
                GPIO1OUT_Value(Motor0_1, 0);
                GPIO1OUT_Value(Motor0_2, 1);
                GPIO1OUT_Value(Motor0_3, 1);
                GPIO1OUT_Value(Motor0_4, 0);
                delayms(4);
                GPIO1OUT_Value(Motor0_1, 1);
                GPIO1OUT_Value(Motor0_2, 1);
                GPIO1OUT_Value(Motor0_3, 0);
                GPIO1OUT_Value(Motor0_4, 0);
                delayms(4);
                GPIO1OUT_Value(Motor0_1, 1);
                GPIO1OUT_Value(Motor0_2, 0);
                GPIO1OUT_Value(Motor0_3, 0);
                GPIO1OUT_Value(Motor0_4, 1);
                delayms(4);
                step++;
            }
        }
        else
        {
            Motor0_Stop();
        }
    }
    if (1 == get_motion_turn())
    {
        set_turn_x_en(1);
    }
    else if (2 == get_motion_turn())
    {
        set_turn_y_en(1);
    }
}

void Rotate_Right(void)
{
    int Right_Value;
    unsigned int step = 0;
    set_turn_x_en(0);
    set_turn_y_en(0);
    if (HW_VER1 == 0)
    {
        while (step < 46) // 36
        {
            GPIO0OUT_Value(Motor1_1, 1);
            GPIO0OUT_Value(Motor1_2, 0);
            GPIO0OUT_Value(Motor1_3, 0);
            GPIO1OUT_Value(Motor1_4, 1);
            delayms(4);
            GPIO0OUT_Value(Motor1_1, 1);
            GPIO0OUT_Value(Motor1_2, 1);
            GPIO0OUT_Value(Motor1_3, 0);
            GPIO1OUT_Value(Motor1_4, 0);
            delayms(4);
            GPIO0OUT_Value(Motor1_1, 0);
            GPIO0OUT_Value(Motor1_2, 1);
            GPIO0OUT_Value(Motor1_3, 1);
            GPIO1OUT_Value(Motor1_4, 0);
            delayms(4);
            GPIO0OUT_Value(Motor1_1, 0);
            GPIO0OUT_Value(Motor1_2, 0);
            GPIO0OUT_Value(Motor1_3, 1);
            GPIO1OUT_Value(Motor1_4, 1);
            delayms(4);
            step++;
        }
    }
    else
    {
        Right_Value = GPIO0IN_Value(Right_Pin);
        if (Right_Value)
        {
            while (step < 20)
            {
                GPIO1OUT_Value(Motor0_1, 1);
                GPIO1OUT_Value(Motor0_2, 0);
                GPIO1OUT_Value(Motor0_3, 0);
                GPIO1OUT_Value(Motor0_4, 1);
                delayms(4);
                GPIO1OUT_Value(Motor0_1, 1);
                GPIO1OUT_Value(Motor0_2, 1);
                GPIO1OUT_Value(Motor0_3, 0);
                GPIO1OUT_Value(Motor0_4, 0);
                delayms(4);
                GPIO1OUT_Value(Motor0_1, 0);
                GPIO1OUT_Value(Motor0_2, 1);
                GPIO1OUT_Value(Motor0_3, 1);
                GPIO1OUT_Value(Motor0_4, 0);
                delayms(4);
                GPIO1OUT_Value(Motor0_1, 0);
                GPIO1OUT_Value(Motor0_2, 0);
                GPIO1OUT_Value(Motor0_3, 1);
                GPIO1OUT_Value(Motor0_4, 1);
                delayms(4);
                step++;
            }
        }
        else
        {
            Motor0_Stop();
        }
    }
    if (1 == get_motion_turn())
    {
        set_turn_x_en(1);
    }
    else if (2 == get_motion_turn())
    {
        set_turn_y_en(1);
    }
}

// Motor1
//**********************************
//               Up
//  Step  M1_1  M1_2  M1_3  M1_4
//    1     1     0     0     1
//    2     1     1     0     0
//    3     0     1     1     0
//    4     0     0     1     1
//              Down
//    1     0     0     1     1
//    2     0     1     1     0
//    3     1     1     0     0
//    4     1     0     0     1
//**********************************
void Motor1_Stop(void)
{
    if (HW_VER1 == 0)
    {
        GPIO1OUT_Value(Motor0_1, 0);
        GPIO1OUT_Value(Motor0_2, 0);
        GPIO1OUT_Value(Motor0_3, 0);
        GPIO1OUT_Value(Motor0_4, 0);
    }
    else
    {
        GPIO1OUT_Value(Motor1_1, 0);
        GPIO1OUT_Value(Motor1_2, 0);
        GPIO1OUT_Value(Motor1_3, 0);
        GPIO1OUT_Value(Motor1_4, 0);
    }
}

void Rotate_Up(void)
{
    int Up_Value;
    unsigned int step = 0;
    set_turn_x_en(0);
    set_turn_y_en(0);
    if (HW_VER1 == 0)
    {
        while (step < 8)
        {
            GPIO1OUT_Value(Motor0_1, 0);
            GPIO1OUT_Value(Motor0_2, 0);
            GPIO1OUT_Value(Motor0_3, 1);
            GPIO1OUT_Value(Motor0_4, 1);
            delayms(9);
            GPIO1OUT_Value(Motor0_1, 0);
            GPIO1OUT_Value(Motor0_2, 1);
            GPIO1OUT_Value(Motor0_3, 1);
            GPIO1OUT_Value(Motor0_4, 0);
            delayms(9);
            GPIO1OUT_Value(Motor0_1, 1);
            GPIO1OUT_Value(Motor0_2, 1);
            GPIO1OUT_Value(Motor0_3, 0);
            GPIO1OUT_Value(Motor0_4, 0);
            delayms(9);
            GPIO1OUT_Value(Motor0_1, 1);
            GPIO1OUT_Value(Motor0_2, 0);
            GPIO1OUT_Value(Motor0_3, 0);
            GPIO1OUT_Value(Motor0_4, 1);
            delayms(9);
            step++;
        }
    }
    else
    {
        Up_Value = GPIO0IN_Value(Up_Pin);
        if (Up_Value)
        {
            while (step < 10)
            {
                GPIO0OUT_Value(Motor1_1, 1);
                GPIO0OUT_Value(Motor1_2, 0);
                GPIO0OUT_Value(Motor1_3, 0);
                GPIO1OUT_Value(Motor1_4, 1);
                delayms(8);
                GPIO0OUT_Value(Motor1_1, 1);
                GPIO0OUT_Value(Motor1_2, 1);
                GPIO0OUT_Value(Motor1_3, 0);
                GPIO1OUT_Value(Motor1_4, 0);
                delayms(8);
                GPIO0OUT_Value(Motor1_1, 0);
                GPIO0OUT_Value(Motor1_2, 1);
                GPIO0OUT_Value(Motor1_3, 1);
                GPIO1OUT_Value(Motor1_4, 0);
                delayms(8);
                GPIO0OUT_Value(Motor1_1, 0);
                GPIO0OUT_Value(Motor1_2, 0);
                GPIO0OUT_Value(Motor1_3, 1);
                GPIO1OUT_Value(Motor1_4, 1);
                delayms(8);
                step++;
            }
        }
        else
        {
            Motor1_Stop();
        }
    }
    if (1 == get_motion_turn())
    {
        set_turn_x_en(1);
    }
    else if (2 == get_motion_turn())
    {
        set_turn_y_en(1);
    }
}

void Rotate_Down(void)
{
    int Down_Value;
    unsigned int step = 0;
    set_turn_x_en(0);
    set_turn_y_en(0);
    if (HW_VER1 == 0)
    {
        while (step < 8)
        {
            GPIO1OUT_Value(Motor0_1, 1);
            GPIO1OUT_Value(Motor0_2, 0);
            GPIO1OUT_Value(Motor0_3, 0);
            GPIO1OUT_Value(Motor0_4, 1);
            delayms(9);
            GPIO1OUT_Value(Motor0_1, 1);
            GPIO1OUT_Value(Motor0_2, 1);
            GPIO1OUT_Value(Motor0_3, 0);
            GPIO1OUT_Value(Motor0_4, 0);
            delayms(9);
            GPIO1OUT_Value(Motor0_1, 0);
            GPIO1OUT_Value(Motor0_2, 1);
            GPIO1OUT_Value(Motor0_3, 1);
            GPIO1OUT_Value(Motor0_4, 0);
            delayms(9);
            GPIO1OUT_Value(Motor0_1, 0);
            GPIO1OUT_Value(Motor0_2, 0);
            GPIO1OUT_Value(Motor0_3, 1);
            GPIO1OUT_Value(Motor0_4, 1);
            delayms(9);
            step++;
        }
    }
    else
    {
        Down_Value = GPIO1IN_Value(Down_Pin);
        if (Down_Value)
        {
            while (step < 10)
            {
                GPIO0OUT_Value(Motor1_1, 0);
                GPIO0OUT_Value(Motor1_2, 0);
                GPIO0OUT_Value(Motor1_3, 1);
                GPIO1OUT_Value(Motor1_4, 1);
                delayms(8);
                GPIO0OUT_Value(Motor1_1, 0);
                GPIO0OUT_Value(Motor1_2, 1);
                GPIO0OUT_Value(Motor1_3, 1);
                GPIO1OUT_Value(Motor1_4, 0);
                delayms(8);
                GPIO0OUT_Value(Motor1_1, 1);
                GPIO0OUT_Value(Motor1_2, 1);
                GPIO0OUT_Value(Motor1_3, 0);
                GPIO1OUT_Value(Motor1_4, 0);
                delayms(8);
                GPIO0OUT_Value(Motor1_1, 1);
                GPIO0OUT_Value(Motor1_2, 0);
                GPIO0OUT_Value(Motor1_3, 0);
                GPIO1OUT_Value(Motor1_4, 1);
                delayms(8);
                step++;
            }
        }
        else
        {
            Motor1_Stop();
        }
    }
    if (1 == get_motion_turn())
    {
        set_turn_x_en(1);
    }
    else if (2 == get_motion_turn())
    {
        set_turn_y_en(1);
    }
}

void Motor_Stop(void)
{
    Motor0_Stop();
    Motor1_Stop();
    motor_stop = 1;
}

void Motor_Inital(void)
{
    unsigned int PAN, TILT;
    int Right_Read_Value, Left_Read_Value;
    int Up_Read_Value, Down_Read_Value;
    if (HW_VER1 == 0)
    {
        PAN = 0;
        while (PAN < 11) // 14
        {
            if (motor_stop)
            {
                goto EXIT;
            }
            Rotate_Right();
            PAN++;
        }
        Motor0_Stop();
        delayms(40);
        PAN = 0;
        while (PAN < 22) // 28
        {
            if (motor_stop)
            {
                goto EXIT;
            }
            Rotate_Left();
            PAN++;
        }
        Motor0_Stop();
        delayms(40);
        PAN = 0;
        while (PAN < 11) // 11
        {
            if (motor_stop)
            {
                goto EXIT;
            }
            Rotate_Right();
            PAN++;
        }
        Motor0_Stop();
        delayms(40);
        TILT = 0;
        while (TILT < 6)
        {
            if (motor_stop)
            {
                goto EXIT;
            }
            Rotate_Up();
            TILT++;
        }
        Motor1_Stop();
        delayms(40);
        TILT = 0;
        while (TILT < 15)
        {
            if (motor_stop)
            {
                goto EXIT;
            }
            Rotate_Down();
            TILT++;
        }
        Motor1_Stop();
        delayms(40);
        TILT = 0;
        while (TILT < 9)
        {
            if (motor_stop)
            {
                goto EXIT;
            }
            Rotate_Up();
            TILT++;
        }
        Motor1_Stop();
        delayms(40);
    }
    else
    {
        do
        {
            if (motor_stop)
            {
                goto EXIT;
            }
            Rotate_Right();
            Right_Read_Value = GPIO0IN_Value(Right_Pin);
        } while (Right_Read_Value);
        Motor0_Stop();
        delayms(50);
        do
        {
            if (motor_stop)
            {
                goto EXIT;
            }
            Rotate_Left();
            Left_Read_Value = GPIO0IN_Value(Left_Pin);
        } while (Left_Read_Value);
        Motor0_Stop();
        delayms(50);
        PAN = 0;
        while (PAN < 12)
        {
            if (motor_stop)
            {
                goto EXIT;
            }
            Rotate_Right();
            PAN++;
        }
        Motor0_Stop();
        delayms(50);
        do
        {
            if (motor_stop)
            {
                goto EXIT;
            }
            Rotate_Up();
            Up_Read_Value = GPIO0IN_Value(Up_Pin);
        } while (Up_Read_Value);
        Motor1_Stop();
        delayms(50);
        do
        {
            if (motor_stop)
            {
                goto EXIT;
            }
            Rotate_Down();
            Down_Read_Value = GPIO1IN_Value(Down_Pin);
        } while (Down_Read_Value);
        Motor1_Stop();
        delayms(50);
        TILT = 0;
        while (TILT < 6)
        {
            if (motor_stop)
            {
                goto EXIT;
            }
            Rotate_Up();
            TILT++;
        }
        Motor1_Stop();
        delayms(50);
    }
EXIT:
    delayms(10);
    motor_stop = 0;
}

//***********************Key Scan**********************
int Key_Scan(void)
{
    int Key_Read_Value;
    int Key_Press = 0;
    int Key_Num = 0;
    Key_Read_Value = GPIO0IN_Value(Button);
    if (!Key_Read_Value)
    {
        delayms(10);
        Key_Read_Value = GPIO0IN_Value(Button);
        if (!Key_Read_Value)
        {
            Key_Press = 1;
            while (!Key_Read_Value)
            {
                delayms(20);
                Key_Num++;
                if (Key_Num > 400)
                {
                    Key_Num = 0;
                    Factory_Default();
                }
                Key_Read_Value = GPIO0IN_Value(Button);
            }
            Key_Num = 0;
        }
    }
    else
        Key_Press = 0;
    return Key_Press;
}

unsigned int get_IR_LED_EN(void)
{
    return IR_LED_EN;
}

void IRCUT_Night(void)
{
    if (HW_VER1 == 0)
    {
        if (get_IR_LED_EN())
        {
            GPIO0OUT_Value(IR_PWREN, 1);
        }
    }
    GPIO1OUT_Value(IRCUT_2, 1);
    delayms(50);
    // printf("Dimmer light, Filter full penetration\n");
    GPIO1OUT_Value(IRCUT_2, 0);
    IR_mode = 0;
}

void IRCUT_Day(void)
{
    if (HW_VER1 == 0)
    {
        GPIO0OUT_Value(IR_PWREN, 0);
    }
    GPIO1OUT_Value(IRCUT_1, 1);
    delayms(50);
    // printf("Sufficient light, Filter Cut\n");
    GPIO1OUT_Value(IRCUT_1, 0);
    IR_mode = 1;
}

void IRCUT_EN(void)
{
    GPIO0OUT_Value(IR_PWREN, 0);
    IRCUT_EN_flag = 1;
}

void IRCUT_DIS(void)
{
    GPIO0OUT_Value(IR_PWREN, 1);
    IRCUT_EN_flag = 0;
}

void set_IR_LED_EN(unsigned int onoff) // 1 IR LED on, 0 IR LED off
{
    if (onoff)
    {
        IR_LED_EN = 1;
        if (HW_VER1 == 0)
        {
            if (!IR_mode)
            {
                GPIO0OUT_Value(IR_PWREN, 1);
            }
        }
        else
        {
            GPIO0OUT_Value(IR_PWREN, 0);
            IRCUT_EN_flag = 1;
        }
    }
    else
    {
        IR_LED_EN = 0;
        if (HW_VER1 == 0)
        {
            GPIO0OUT_Value(IR_PWREN, 0);
        }
        else
        {
            GPIO0OUT_Value(IR_PWREN, 1);
            IRCUT_EN_flag = 0;
            sleep(2);
            updateAdvance(0);
            IRCUT_Day();
            day = 1;
            night = 1;
        }
    }
}

void IRCUT_Ctrl(void)
{
    int key_fd = -1;
    struct timeval tv;
    fd_set rd;
    unsigned int data, num = 0, tmp = 0;
    unsigned int day_thr, night_thr;

    if (sensor_id == 1) // E21
    {
        day_thr = 60;
        night_thr = 142;
    }
    else if (sensor_id == 2)
    {
        if (HW_VER1 == 0) // E23
        {
            day_thr = 60;
            night_thr = 142;
        }
        else // E22
        {
            day_thr = 49; // 64
            night_thr = 162;
        }
    }
    if ((key_fd = open(haicam_cfgGetIRCtrlDevice(), O_RDONLY | O_NONBLOCK)) > 0)
    {
        // printf("key_fd = %d\n", key_fd);
        while (key_fd)
        {
            if (IRCUT_EN_flag == 1)
            {
                tv.tv_sec = 0;
                tv.tv_usec = 500000; /* timeout 500 ms */
                FD_ZERO(&rd);
                FD_SET(key_fd, &rd);
                select(key_fd + 1, &rd, NULL, NULL, &tv);
                if (read(key_fd, &data, sizeof(unsigned int)) > 0)
                {
                    delayms(10);
                    tmp = tmp + data;
                }
                num++;
                if (num == 12)
                {
                    tmp = tmp / num;
                    // printf("tmp = %d\n", tmp);
                    if (tmp > night_thr)
                    {
                        if (night == 1)
                        {
                            updateAdvance(1);
                            IRCUT_Night();
                            day = 1;
                            night = 0;
                        }
                    }
                    else if (tmp < day_thr)
                    {
                        if (day == 1)
                        {
                            updateAdvance(0);
                            IRCUT_Day();
                            day = 0;
                            night = 1;
                        }
                    }
                    num = 0;
                    tmp = 0;
                }
                delayms(350);
            }
            else
            {
                num = 0;
                tmp = 0;
                delayms(3000);
                // printf("DIsable IRCUT\n");
            }
        }
    }
    else
    {
        printf("Can't open sar_adc_drv\n");
        // return -1;
    }
    close(key_fd);
}

void LED_ON_Red(void)
{
    GPIO0OUT_Value(LED_Red, 1);
    LED_State_Red = 1;
}

void LED_ON_Blue(void)
{
    GPIO0OUT_Value(LED_Blue, 0);
    LED_State_Blue = 1;
}

void LED_OFF_Red(void)
{
    GPIO0OUT_Value(LED_Red, 0);
    LED_State_Red = 0;
}

void LED_OFF_Blue(void)
{
    GPIO0OUT_Value(LED_Blue, 1);
    LED_State_Blue = 0;
}

void LED_TOGGLE_Red(void)
{
    if (LED_State_Red)
        LED_OFF_Red();
    else
        LED_ON_Red();
}

void LED_TOGGLE_Blue(void)
{
    if (LED_State_Blue)
        LED_OFF_Blue();
    else
        LED_ON_Blue();
}

void LED_BLINK_Red(int times, int delay)
{
    int i;
    for (i = 0; i < times; i++)
    {
        LED_TOGGLE_Red();
        delayms(delay);
    }
}

void LED_BLINK_Blue(int times, int delay)
{
    int i;
    for (i = 0; i < times; i++)
    {
        LED_TOGGLE_Blue();
        delayms(delay);
    }
}

//********************Board Inital***********************
void Board_Init(void)
{
    GPIO_READ();
    sensor_check();
    if (sensor_id == 2)
    {
        Set_GPIO1IN(VER1);
        HW_VER1 = GPIO1IN_Value(VER1);
    }
    Set_GPIO1OUT(Motor0_1);
    Set_GPIO1OUT(Motor0_2);
    Set_GPIO1OUT(Motor0_3);
    Set_GPIO1OUT(Motor0_4);
    Set_GPIO0IN(Left_Pin);
    Set_GPIO0IN(Right_Pin);
    Set_GPIO0OUT(Motor1_1);
    Set_GPIO0OUT(Motor1_2);
    Set_GPIO0OUT(Motor1_3);
    Set_GPIO1OUT(Motor1_4);
    Set_GPIO0IN(Up_Pin);
    Set_GPIO1IN(Down_Pin);
    Motor0_Stop();
    Motor1_Stop();
    Set_GPIO0IN(Button);
    Set_GPIO0OUT(LED_Red);
    Set_GPIO0OUT(LED_Blue);
    Set_GPIO0OUT(IR_PWREN);
    Set_GPIO1OUT(IRCUT_1);
    Set_GPIO1OUT(IRCUT_2);
    GPIO0OUT_Value(LED_Red, 1);
    GPIO0OUT_Value(LED_Blue, 1);
    GPIO0OUT_Value(IR_PWREN, 0);
    GPIO1OUT_Value(IRCUT_1, 1);
    GPIO1OUT_Value(IRCUT_2, 0);
    delayms(50);
    GPIO1OUT_Value(IRCUT_1, 0);
    day = 1;
    night = 1;
    motor_stop = 0;
    IR_LED_EN = 1;
    IR_mode = 1;
    IRCUT_EN_flag = 1;
}

#endif


#if !EZHOMELABS_CAMERA
static int motion_x = 0;
static int motion_y = 0;
static int motion_x_en = 0;
static int motion_y_en = 0;
static int turn_x_en = 0;
static int turn_y_en = 0;
static int motion_turn = 0;
void set_motion_x(int x)
{
    motion_x = x;
}

void set_motion_y(int y)
{
    motion_y = y;
}

int get_motion_x(void)
{
    return motion_x;
}

int get_motion_y(void)
{
    return motion_y;
}
void set_motion_x_en(int x)
{
    motion_x_en = x;
}

void set_motion_y_en(int y)
{
    motion_y_en = y;
}

int get_motion_x_en(void)
{
    return motion_x_en;
}

int get_motion_y_en(void)
{
    return motion_y_en;
}

void set_turn_x_en(int en)
{
    turn_x_en = en;
}

int get_turn_x_en(void)
{
    return turn_x_en;
}

void set_turn_y_en(int en)
{
    turn_y_en = en;
}

int get_turn_y_en(void)
{
    return turn_y_en;
}

void set_motion_turn(int en)
{
    switch(en)
    {
	case 0: set_turn_x_en(0);   //x disable, y disable
		set_turn_y_en(0);
		break;
	case 1: set_turn_x_en(1);   //x enable, y disable
		set_turn_y_en(0);
		break;
	case 2: set_turn_x_en(0);   //x disable, y enable
		set_turn_y_en(1);
		break;
	default : break;
    }
    motion_turn = en;
}

int get_motion_turn(void)
{
    return motion_turn;
}
#endif

static int isp_fd = 0;

void initAdvance(){
    int ret;
    int awb_en, sensor_awb_en, ae_en, sensor_ae_en, mrnr_en, tmnr_en, sp_en;   
    int scene_mode, denoise, contrast, brightness, shapness;
    if (isp_fd == 0){
        isp_fd = open(haicam_cfgGetISPDevice(), O_RDWR);
        if (isp_fd < 0) {
            haicam_utils_log("Open ISP dev fail\n");
            return;
        }
    }
    ret = ioctl(isp_fd, ISP_IOC_GET_SENSOR_AWB_EN, &sensor_awb_en);
    ret = ioctl(isp_fd, AWB_IOC_GET_ENABLE, &awb_en);
    sensor_awb_en = 1;
    awb_en = 1;
    ret = ioctl(isp_fd, ISP_IOC_SET_SENSOR_AWB_EN, &sensor_awb_en);
    ret = ioctl(isp_fd, AWB_IOC_SET_ENABLE, &awb_en);

    ret = ioctl(isp_fd, ISP_IOC_GET_SENSOR_AE_EN, &sensor_ae_en);
    ret = ioctl(isp_fd, AE_IOC_GET_ENABLE, &ae_en);
    sensor_ae_en = 1;
    ae_en = 1;
    ret = ioctl(isp_fd, ISP_IOC_SET_SENSOR_AE_EN, &sensor_ae_en);
    ret = ioctl(isp_fd, AE_IOC_SET_ENABLE, &ae_en);

    mrnr_en = 1;
    ret = ioctl(isp_fd, ISP_IOC_SET_MRNR_EN, &mrnr_en);
    tmnr_en = 1;
    ret = ioctl(isp_fd, ISP_IOC_SET_TMNR_EN, &tmnr_en);
    sp_en = 1;
    ret = ioctl(isp_fd, ISP_IOC_SET_SP_EN, &sp_en);

    ret = ioctl(isp_fd, AWB_IOC_GET_SCENE_MODE, &scene_mode);
    scene_mode = 0;
    ret = ioctl(isp_fd, AWB_IOC_SET_SCENE_MODE, &scene_mode);

    ret = ioctl(isp_fd, ISP_IOC_GET_DENOISE, &denoise);
    ret = ioctl(isp_fd, ISP_IOC_GET_CONTRAST, &contrast);
    ret = ioctl(isp_fd, ISP_IOC_GET_BRIGHTNESS, &brightness);
    ret = ioctl(isp_fd, ISP_IOC_GET_SHARPNESS, &shapness);
    denoise = 178;
    contrast = 134; 
    brightness = 131;
    shapness = 134;
    ret = ioctl(isp_fd, ISP_IOC_SET_DENOISE, &denoise);
    ret = ioctl(isp_fd, ISP_IOC_SET_CONTRAST, &contrast);
    ret = ioctl(isp_fd, ISP_IOC_SET_BRIGHTNESS, &brightness);
    ret = ioctl(isp_fd, ISP_IOC_SET_SHARPNESS, &shapness);
}

void updateAdvance(int iAdvance){
    int ret;
    int saturation;
    ret = ioctl(isp_fd, ISP_IOC_GET_SATURATION, &saturation);
    //TODO
    //setNightVision(iAdvance);
    if (iAdvance == 0) {
        saturation = 128;
    }
    else if (iAdvance == 1) {
        saturation = 0;
    }
    ret = ioctl(isp_fd, ISP_IOC_SET_SATURATION, &saturation);
    //TODO
    //disMotionByTime(2);
}