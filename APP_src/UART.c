/*******************************************************************************
 **                      Includes                                              **
 *******************************************************************************/
#include <stdio.h>
#include <Ifx_Types.h>
#include <Asclin/Asc/IfxAsclin_Asc.h>
#include "Uart.h"
#include "glcd.h"
#include "Test_Pwm.h"
#include <math.h>
#include "ScuEruDemo.h"
#define ASC_TX_BUFFER_SIZE 64
#define ASC_RX_BUFFER_SIZE 64
#define UART_TEST_DATA_SIZE		12U
#define ISR_PRIORITY_ASCLIN0_TX		15
#define ISR_PRIORITY_ASCLIN0_RX		14
#define ISR_PRIORITY_ASCLIN0_ER		13
int Object = 0;
int School_Zone = 0;

READSCAN Check;
//****************************************************************************
// @Typedefs
//****************************************************************************
typedef struct
{
	uint8 tx[ASC_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
	uint8 rx[ASC_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
} AppAscBuffer;

/** \brief Asc information */
typedef struct
{
	AppAscBuffer ascBuffer;                     /**< \brief ASC interface buffer */
	struct
	{
		IfxAsclin_Asc asc0;                     /**< \brief ASC interface */
		IfxAsclin_Asc asc1;                     /**< \brief ASC interface */
	}         drivers;

	uint8 Uart_TxData[UART_TEST_DATA_SIZE];
	uint8 Uart_RxData[UART_TEST_DATA_SIZE];

	uint16	TxComplete		:16;
	uint16	RxComplete		:16;
	uint16	ErComplete		:16;

	Ifx_SizeT count;
} App_AsclinAsc;

/*******************************************************************************
 **                      Private Variable Declarations                         **
 *******************************************************************************/

App_AsclinAsc AsclinAsc; /**< \brief Demo information */

/*******************************************************************************
 **                      Global Function Definitions                           **
 *******************************************************************************/
void Uart_Initialization(void);

//******************************************************************************
// @Function	 	void Uart_Initialization(void)
// @Description   	UART initialization for test with StarterKit
// @Returnvalue		None
// @Parameters    	None
//******************************************************************************
void Uart_Initialization(void)
{
	static uint8 i;

	/* disable interrupts */
	boolean              interruptState = IfxCpu_disableInterrupts();
	/* create module config */
	IfxAsclin_Asc_Config Uart_AscLin0;
	IfxAsclin_Asc_initModuleConfig(&Uart_AscLin0, &MODULE_ASCLIN0);

	/* set the desired baudrate */
	Uart_AscLin0.baudrate.prescaler    = 1;
	Uart_AscLin0.baudrate.baudrate     = 115200; /* FDR values will be calculated in initModule */ // 처음에는 1000000 이었음.
	Uart_AscLin0.baudrate.oversampling = IfxAsclin_OversamplingFactor_4;

	/* ISR priorities and interrupt target */
	Uart_AscLin0.interrupt.txPriority    = ISR_PRIORITY_ASCLIN0_TX;
	Uart_AscLin0.interrupt.rxPriority    = ISR_PRIORITY_ASCLIN0_RX;
	Uart_AscLin0.interrupt.erPriority    = ISR_PRIORITY_ASCLIN0_ER;
	Uart_AscLin0.interrupt.typeOfService = IfxSrc_Tos_cpu0;

	/* FIFO configuration */
	Uart_AscLin0.txBuffer     = AsclinAsc.ascBuffer.tx;
	Uart_AscLin0.txBufferSize = ASC_TX_BUFFER_SIZE;

	Uart_AscLin0.rxBuffer     = AsclinAsc.ascBuffer.rx;
	Uart_AscLin0.rxBufferSize = ASC_RX_BUFFER_SIZE;

	const IfxAsclin_Asc_Pins pins = {
			NULL,                     IfxPort_InputMode_pullUp,        /* CTS pin not used */
			&IfxAsclin0_RXB_P15_3_IN, IfxPort_InputMode_pullUp,        /* Rx pin */  // 원래 꺼 &IfxAsclin0_RXB_P15_3_IN  IfxAsclin0_RXC_P00_1_IN
			NULL,                     IfxPort_OutputMode_pushPull,     /* RTS pin not used */
			&IfxAsclin0_TX_P15_2_OUT, IfxPort_OutputMode_pushPull,     /* Tx pin */  // 원래 &IfxAsclin0_TX_P15_2_OUT  IfxAsclin0_TX_P00_0_OUT
			IfxPort_PadDriver_cmosAutomotiveSpeed1
	};
	Uart_AscLin0.pins = &pins;

	/* initialize module */
	IfxAsclin_Asc_initModule(&AsclinAsc.drivers.asc0, &Uart_AscLin0);

	/* enable interrupts again */
	IfxCpu_restoreInterrupts(interruptState);



} /* End of Uart_Initialization */


//******************************************************************************
// @Function	 	void Uart_Test(void)
// @Description   	UART communication test
// @Returnvalue		None
// @Parameters    	None
//******************************************************************************

void SEND(uint8 First, uint8 Second)
{

	AsclinAsc.Uart_TxData[0] = First;
	AsclinAsc.Uart_TxData[1] = Second;
	AsclinAsc.count = 2U;
	IfxAsclin_Asc_write(&AsclinAsc.drivers.asc0,AsclinAsc.Uart_TxData, &AsclinAsc.count, TIME_INFINITE);


}

int READ_BYTE(void)
{
	AsclinAsc.count = 1U;
	IfxAsclin_Asc_read(&AsclinAsc.drivers.asc0, AsclinAsc.Uart_RxData, &AsclinAsc.count, 0xFF);
	//A->R[0] = AsclinAsc.Uart_RxData[0]; //READSCAN [A]
	return AsclinAsc.Uart_RxData[0];

}
boolean WAIT(int X)
{
	AsclinAsc.count = X;
	return IfxAsclin_Asc_canReadCount(&AsclinAsc.drivers.asc0, AsclinAsc.count, 0xFF);
}

void BUFFER_RESET()
{
	IfxAsclin_Asc_clearRx(&AsclinAsc.drivers.asc0);
}


void LidarInit(void){
	int maybe = 0;
	uint32 recv = 0;
	uint8 ReceivedValue;
	SEND(64, 165);
	Delay_ms(2);
	SEND(32, 165);
	while(1) {
		if (WAIT(1)){
			ReceivedValue=READ_BYTE();
			if (ReceivedValue == 165) {
				maybe++;
			} else if (maybe == 1 && ReceivedValue == 90) {
				maybe++;
			} else if (maybe == 2 && ReceivedValue == 5) {
				maybe++;
			} else if (maybe == 3 && ReceivedValue == 0) {
				maybe++;
			} else if (maybe == 4 && ReceivedValue == 0) {
				maybe++;
			} else if (maybe == 5 && ReceivedValue == 64) {
				maybe++;
			} else if (maybe == 6 && ReceivedValue == 129) {
				maybe = 0;
				recv=0;
				break;
			} else {
				maybe = 0;
			}
		}
		recv++;
		if (recv>640000){
			SEND(32,165);
			recv = 0;
		}
	}
}

void LidarGet(void){
	int i = 0;
	int CN = 0;
	int Tmp_Dis = 10000;
	int Tmp_Ang = 0;
	int MIN_Dis = 10000;
	int MAX_Ang = 0;
	int MIN_Ang = 360;
	int CC = 0;
	int WIDTH_LEFT, WIDTH_RIGHT = 0;
	int WIDTH = 0;
	int angle[2000]={0,};
	int distance[2000]={0,};
	boolean checking[2000]={0,};
	int DISTANCE_CHECK[2000]={0,};
	int ANGLE_CHECK[2000] = {0,};
	uint8 B[10000] = {0, };
	char text[32];
	int CHCH = 0;
	int CH =0;
	CH = AsclinAsc.RxComplete;
	for (CN = 0; CN < 2000;CN++) {
		B[CN]=IfxAsclin_Asc_blockingRead(&AsclinAsc.drivers.asc0);
		//Modify
		if( CN % 5 == 4 )
		{
			i = CN/5;
			angle[i]=((B[i*5+1]+256*B[i*5+2])>>1)>>6;
			distance[i] = (B[i*5+3]+256*B[i*5+4])>>2;
			checking[i] = (((B[i*5] >> 1) ^ B[i*5]) & 0x1) && ( (B[(i*5)+1] & (0x1 << 0)) == 1 )  ;
			if(   ( (360> angle[i]) && (angle[i] > 334) ) ||( (0 < angle[i]) && (angle[i] < 26)    )   ){
				DISTANCE_CHECK[CC] = distance[i];
				ANGLE_CHECK[CC] = angle[i];
				CC = CC + 1;
			}
		}
	}
	CHCH = AsclinAsc.RxComplete;

	for(i = 0 ; i <CC ; i++){
		if( (DISTANCE_CHECK[i] > 0) && (DISTANCE_CHECK[i] <2000) ){
			Tmp_Dis = DISTANCE_CHECK[i];
			Tmp_Ang = ANGLE_CHECK[i];
			if( Tmp_Dis < MIN_Dis )
				MIN_Dis = Tmp_Dis;
			if( Tmp_Ang < 200 && (Tmp_Ang > MAX_Ang))
				MAX_Ang = Tmp_Ang;
			if( Tmp_Ang > 200 && (Tmp_Ang < MIN_Ang))
				MIN_Ang = Tmp_Ang;
		}
	}

	WIDTH_RIGHT = tan((MAX_Ang)*3.1415926535/180)*MIN_Dis;
	WIDTH_LEFT = tan((360-MIN_Ang)*3.1415926535/180)*MIN_Dis;
	WIDTH = WIDTH_LEFT + WIDTH_RIGHT;

	//SEND(37, 165);
	//BUFFER_RESET();

	//Delay_ms(1);
	//SEND(64, 165);
	//Delay_ms(2);
	usr_sprintf(text, " WIDTH_LEFT  :  %4d       ", WIDTH_LEFT);
	GLCD_displayStringLn(LINE0, text);
	usr_sprintf(text, " WIDTH_RIGH  :  %4d       ", WIDTH_RIGHT);
	GLCD_displayStringLn(LINE1, text);
	usr_sprintf(text, " WIDTH_TOTL  :  %4d       ", WIDTH);
	GLCD_displayStringLn(LINE2, text);
	usr_sprintf(text, " ANGLE_LEFT  :  %4d       ",360-MIN_Ang);
	GLCD_displayStringLn(LINE3, text);
	usr_sprintf(text, " ANGLE_RIGH  :  %4d       ", MAX_Ang );
	GLCD_displayStringLn(LINE4, text);
	usr_sprintf(text, " MIN_DISTAN  :  %4d       ", MIN_Dis);
	GLCD_displayStringLn(LINE5, text);
	usr_sprintf(text, "%4d      %4d     %4d       ", B[20], B[150],B[300]);
	GLCD_displayStringLn(LINE6, text);
	usr_sprintf(text, "%4d      %4d     %4d       ", B[500], B[600], B[915]);
	GLCD_displayStringLn(LINE7, text);
	usr_sprintf(text, "%4d      %4d     %4d       ", B[101], B[451], B[1011]);
	GLCD_displayStringLn(LINE8, text);
	usr_sprintf(text, "%4d      %4d     %4d       ", B[1221], B[1431], B[1651]);
	GLCD_displayStringLn(LINE8, text);
	AsclinAsc.RxComplete = 0;
}

void LidarExactlyGet(LidarData *LIDAR)
{
	int i = 0;
	int CN = 0;
	int Tmp_Dis = 10000;
	int Tmp_Ang = 0;
	int MIN_Dis = 10000;
	int MAX_Ang = 0;
	int MIN_Ang = 360;
	int CC = 0;
	int WIDTH_LEFT, WIDTH_RIGHT = 0;
	int WIDTH = 0;
	int angle[2000]={0,};
	int distance[2000]={0,};
	boolean checking[2000]={0,};
	int DISTANCE_CHECK[2000]={0,};
	int ANGLE_CHECK[2000] = {0,};
	uint8 B[10000] = {0, };
	char text[32];
	boolean Okay = 0;
	int ReceivedByte = 0;
	int maybe = 0;
	int recv = 0;
	int PROCESS = 0;



	//BUFFER_RESET();
while (1){
	switch(PROCESS){
	case 0:
	while(1) {

		if (WAIT(1)){
			ReceivedByte = READ_BYTE();
			if (  maybe == 0 &&((ReceivedByte + 2*ReceivedByte )>>1)%2 == 1 ) {//해보고 안되면 /2 해보고,
				maybe++;
				B[0] = ReceivedByte;
			} else if ( maybe == 1 && ReceivedByte%2 == 1 ) {
				maybe = 0;
				recv = 0;
				B[1] = ReceivedByte;
				PROCESS=1;
				break;
			}	else {
				maybe = 0;
			}
		}
		recv++;
		if (recv>640000){
			SEND(32,165);
			recv = 0;
		}
	}
	CC = 0;
	break;
	case 1:
	for (CN = 2; CN < 1700;CN++) {
		B[CN]=IfxAsclin_Asc_blockingRead(&AsclinAsc.drivers.asc0);
		//Modify
		if( CN % 5 == 4 )
		{
			i = CN/5;
			if(  ((B[i*5] + 2*B[i*5] )>>1)%2 == 1  && B[(i*5)+1]%2 == 1 ){
				angle[i]=((B[i*5+1]+256*B[i*5+2])>>1)>>6;
				distance[i] = (B[i*5+3]+256*B[i*5+4])>>2;
				//checking[i] = (((B[i*5] >> 1) ^ B[i*5]) & 0x1) && ( (B[(i*5)+1] & (0x1 << 0)) == 1 )  ;
				if(   ( (360> angle[i]) && (angle[i] > 334) ) ||( (0 < angle[i]) && (angle[i] < 26)    )   ){
					DISTANCE_CHECK[CC] = distance[i];
					ANGLE_CHECK[CC] = angle[i];
					CC = CC + 1;
				}
			}
			else {
				PROCESS = 0;

				B[CN]=IfxAsclin_Asc_blockingRead(&AsclinAsc.drivers.asc0);
				break;
			}

		}
	}
	if(PROCESS== 1)
	PROCESS = 3;
	break;
	}
	if(PROCESS==3)
		break;
}
	for(i = 0 ; i <CC ; i++){
		if( (DISTANCE_CHECK[i] > 0) && (DISTANCE_CHECK[i] <1250) ){
			Tmp_Dis = DISTANCE_CHECK[i];
			Tmp_Ang = ANGLE_CHECK[i];
			if( Tmp_Dis < MIN_Dis )
				MIN_Dis = Tmp_Dis;
			if( Tmp_Ang < 200 && (Tmp_Ang > MAX_Ang))
				MAX_Ang = Tmp_Ang;
			if( Tmp_Ang > 200 && (Tmp_Ang < MIN_Ang))
				MIN_Ang = Tmp_Ang;
		}
	}

	WIDTH_RIGHT = tan((MAX_Ang)*3.1415926535/180)*MIN_Dis;
	WIDTH_LEFT = tan((360-MIN_Ang)*3.1415926535/180)*MIN_Dis;
	WIDTH = WIDTH_LEFT + WIDTH_RIGHT;

	//SEND(37, 165);
	//BUFFER_RESET();

	//Delay_ms(1);
	//SEND(64, 165);
	//Delay_ms(2);
	LIDAR->ANGLE_LEFT =360-MIN_Ang;
	LIDAR->ANGLE_RIGHT = MAX_Ang;
	LIDAR->MIN_DISTANCE = MIN_Dis;
	LIDAR->WIDTH_TOTAL = WIDTH;
	/*
	usr_sprintf(text, " WIDTH_LEFT  :  %4d       ", WIDTH_LEFT);
	GLCD_displayStringLn(LINE0, text);
	usr_sprintf(text, " WIDTH_RIGH  :  %4d       ", WIDTH_RIGHT);
	GLCD_displayStringLn(LINE1, text);
	usr_sprintf(text, " WIDTH_TOTL  :  %4d       ", WIDTH);
	GLCD_displayStringLn(LINE2, text);
	usr_sprintf(text, " ANGLE_LEFT  :  %4d       ",360-MIN_Ang);
	GLCD_displayStringLn(LINE3, text);
	usr_sprintf(text, " ANGLE_RIGH  :  %4d       ", MAX_Ang );
	GLCD_displayStringLn(LINE4, text);
	usr_sprintf(text, " MIN_DISTAN  :  %4d       ", MIN_Dis);
	GLCD_displayStringLn(LINE5, text);
	usr_sprintf(text, " B0  :  %4d       ", B[0]);
	GLCD_displayStringLn(LINE6, text);
	usr_sprintf(text, " B1  :  %4d       ", B[1]);
	GLCD_displayStringLn(LINE7, text);
	usr_sprintf(text, " CC  :  %4d       ", CC);
	GLCD_displayStringLn(LINE8, text);
	*/
	//Delay_ms(10);

}


int LidarObjectCheck(LidarData *Lidar){
	if( Object == 0)
	{
		if(  Lidar->MIN_DISTANCE <500)//언덕의 경우 내려올 때, 갑자기 땅이 보임. 그 갑자기 보이는 것은 Object가 0이었을 때 갑자기 MIN_DISTANCE가 몇 이하로 떨어지게 되는 것. 일단은 500으로 둠, //Lidar->WIDTH_TOTAL > 550 && 없어도 될듯.
			Object = 1; // 언덕
		else if(Avoiding == 0 &&School_Zone &&Lidar->MIN_DISTANCE < 900 && Lidar->WIDTH_TOTAL < 500){
			Object = 2;
			Avoiding = 1;//장애물
		}
		else if(School_Zone != 1 && Lidar->MIN_DISTANCE < 1100 && Lidar->WIDTH_TOTAL > 470){//1667로 자르면, 수직거리가 1500. 0.2초마다 돌아가며, 2m/s이기 때문에 최소 40의 차이가 나야함. 따라서 1400
			Object = 3; // 비상제동
		}
		//if( Object == 2 && Lidar->MIN_DISTANCE < 900)
		//Avoiding = 1;
	}
	else if( Object != 0 &&Lidar->MIN_DISTANCE > 3000 )
		Object = 0;
	return Object;
}

IFX_INTERRUPT(Uart_AscLin0_TxIsr, 0, ISR_PRIORITY_ASCLIN0_TX)
{
	IfxAsclin_Asc_isrTransmit(&AsclinAsc.drivers.asc0);
	AsclinAsc.TxComplete++;
}


//name Interrupts for Receive


IFX_INTERRUPT(Uart_AscLin0_RxIsr, 0, ISR_PRIORITY_ASCLIN0_RX)
{
	IfxAsclin_Asc_isrReceive(&AsclinAsc.drivers.asc0);
	AsclinAsc.RxComplete++;
}


//name Interrupts for Error


IFX_INTERRUPT(Uart_AscLin0_ErIsr, 0, ISR_PRIORITY_ASCLIN0_ER)
{
	IfxAsclin_Asc_isrError(&AsclinAsc.drivers.asc0);
	AsclinAsc.ErComplete++;

}



