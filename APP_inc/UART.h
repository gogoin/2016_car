/*
 * UART.h
*/


#ifndef UART_H_
#define UART_H_

typedef struct
{
	uint8 R[52];
} READSCAN;
typedef struct
{
	int WIDTH_TOTAL;
	int MIN_DISTANCE;
	int ANGLE_LEFT;
	int ANGLE_RIGHT;
} LidarData;
extern int School_Zone;
extern void SEND(uint8 First, uint8 Second);
extern int READ_BYTE(void);//READSCAN []
extern boolean WAIT(int X);
extern void BUFFER_RESET();
extern void Uart_Initialization(void);
extern void LidarInit(void);
extern void LidarGet(void);
extern void LidarExactlyGet(LidarData *LIDAR);
extern int LidarObjectCheck(LidarData *Lidar);
extern int Object;
#endif /* 0_SRC_0_APPSW_APP_INC_UART_H_ */
