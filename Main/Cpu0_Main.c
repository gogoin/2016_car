/**
 * \file Cpu0_Main.c
 * \brief Main function definition for Cpu core 0 .
 *
 * \copyright Copyright (c) 2012 Infineon Technologies AG. All rights reserved.
 *
 *
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */

#include "Tricore/Cpu/Std/Ifx_Types.h"
#include "Tricore/Cpu/Std/IfxCpu_Intrinsics.h"
#include "Tricore/Scu/Std/IfxScuWdt.h"
#include <Stm/Std/IfxStm.h>
#include <Port/Std/IfxPort.h>
#include <Gtm/Tom/Pwm/IfxGtm_Tom_Pwm.h>
#include <stdio.h>


#include "Test_Pwm.h"
#include "glcd.h"
#include "Test_Dio.h"
#include "Test_Adc.h"
#include "Camera.h"
#include "UART.h"

//exinterrupt
#include "Cpu0_Main.h"
#include "ScuEruDemo.h"
#include "SysSe/Bsp/Bsp.h"

int Right_Lane=1;
App_Cpu0 g_AppCpu0;
uint16 duty_speed = 700;
long int current_speed = 0;
long int target_speed = 700;//mm/s
uint32 R_Arr[128]={0};
uint32 R_ArrCenVal=0;
uint32 R_ArrSum=0;
uint32 R_ArrNum=0;
uint16 R_Centr =0;
uint32 L_Arr[128]={0};
uint32 L_ArrCenVal=0;
uint32 L_ArrSum=0;
uint32 L_ArrNum=0;
uint16 L_Centr =0;
uint16 Centr =0;
uint32 CenVal = 624;
int temp1=0;
int temp2=0;
int R_max=0;
int R_min=0;
int L_max=0;
int L_min=0;
int R_t1=0;
int R_t2=0;
int L_t1=0;
int L_t2=0;
int R_past=0;
int L_past=0;

LidarData _Lidar;
enum lost_dir{
	not_lost,
	left,
	right
};


#define MIN_SPEED		700
#define MAX_SPEED		1000
void CarRuning(){

	//Pwm_MotorDutyAndDirectionControl(Speed, Direction);
	long int error =0;
	static long int pre_speed =0;
	long int errorderivative =0;
	static long int duty_temp=700;
	static int pre_target=MIN_SPEED;
	uint16 kp = 10;  // 0.4*10
	uint16 kd = 7;

	if(pre_target>target_speed) {}
	else {
		target_speed=(pre_target*10+target_speed)/11;
	}
	pre_target=target_speed;


	if(IfxPort_getPinState(&MODULE_P02,0) == FALSE){
		Dio_Configuration(&MODULE_P13, 3, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_low);
		Dio_Configuration(&MODULE_P00, 1, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_high);
	}
	if(School_Zone == 1){

		target_speed = 700;
	}
	else {
		if(delta_t!=0)
			current_speed = 62800/delta_t; // 6.28cm/ms = 62.8m/s = (62.8mm/ms)*1000
	}
	if(target_speed>MAX_SPEED) target_speed=MAX_SPEED;
	if(target_speed<MIN_SPEED) target_speed=MIN_SPEED;
	/*controller start*/
	error = target_speed-current_speed;
	errorderivative = current_speed-pre_speed;
	if(pasttime != 0){//current_speed>0
		kp=30;
		duty_temp = (kp*(error)+kd*(current_speed));
		duty_temp= duty_temp/10;
	}
	/*controller end*/
	if(duty_temp <700)
		duty_temp = 700;
	if(duty_temp > 1500)
		duty_temp = 1500;
	duty_speed = (uint16)duty_temp;

	pre_speed=current_speed;

	////////////////////////////////300ms
	if((speed_ms - pasttime) > 300 )
	{
		//current_speed = current_speed/3;
		//pasttime = speed_ms;
		delta_t = speed_ms-pasttime;
	}



	Pwm_DutyUpdate(IfxGtm_TOM0_10_TOUT12_P00_3_OUT, duty_speed);
}





void FrontControl(uint16 Angle){


	Pwm_Steering(IfxGtm_TOM1_0_TOUT32_P33_10_OUT,Angle);

}
/****************modify***************/
#define CUTOFF 15
#define CAMERA_WIDTH 127
#define LEFT_EXTREME    ((uint16)0x15EU) //350
#define LEFT            ((uint16)0x190U) //528 -> 520 ->400
#define CENTER          ((uint16)0x270U) //624
#define RIGHT           ((uint16)0x352U) //720 -> 780 -> 850
#define RIGHT_EXTREME   ((uint16)0x384U) //900
void CarSteering(void){

	uint8 t;
	R_ArrNum=0;
	R_ArrSum=0;
	L_ArrNum=0;
	L_ArrSum=0;
	static int CameraCentr;
	CameraCentr=64;
	temp1=0;
	temp2=0;
	R_max=0;
	R_min=0;
	L_max=0;
	L_min=0;
	R_t1=0;
	//R_t2=0;
	L_t1=0;
	//L_t2=0;
	char text[32];

	static int R_lost=not_lost;
	static int L_lost=not_lost;

	for(t=CUTOFF; t<CAMERA_WIDTH-CUTOFF; t++){
		temp1 = CameraResult[0][t]-CameraResult[0][t+1];//camera (t+1) - t
		temp2 = CameraResult[1][t]-CameraResult[1][t+1];//camera (t+1) - t

		if(temp1 < 0)
		{
			if(temp1 < R_min){
				R_min = temp1;
				R_t1 = t;
				if(R_min > -200)
				{
					R_t1 = 0;//아까는 127
				}

			}
		}
		if(temp2 > 0 )
		{
			if(temp2 > L_max){
				L_max = temp2;
				L_t1 = t;
				if(L_max < 200)
					L_t1 = 127;//이까는 0
			}
		}
	}
	/************* lost **************/
	if (R_lost==not_lost){
		if(R_past!=0&&R_t1==0){//아까는 127,127
			if(R_past>87) R_lost=left;
			else if(R_past<40) R_lost=right;//아까는 else 였음
		}

	}
	if (L_lost==not_lost){
		if(L_past!=127&&L_t1==127){//0,0,
			if(L_past>87) L_lost=left;//아까는 R_PAST
			else if (L_past<40) L_lost=right;//아까는 else 였음
		}
	}
	/************* found ************/
	if(R_past==0&&R_t1!=0){//127,127
		if(R_lost==left) {
			if(R_t1>CAMERA_WIDTH/2)
				R_lost=not_lost;
		}
		else if(R_lost==right){
			if(R_t1<CAMERA_WIDTH/2)
				R_lost=not_lost;
		}
	}
	if(L_past==127&&L_t1!=127){//0,0
		if(L_lost==left) {
			if(L_t1>CAMERA_WIDTH/2)
				L_lost=not_lost;
		}
		else if(L_lost==right){
			if(L_t1<CAMERA_WIDTH/2)
				L_lost=not_lost;
		}
	}
	/***********Lost&Found end**********/
	if((R_lost!=not_lost) && (L_lost!=not_lost)) // lost both
	{
		if((R_lost==right && L_lost==left)||(R_lost==left && L_lost==right)) {
			if(Centr>CENTER) Centr=RIGHT_EXTREME;
			else Centr=LEFT_EXTREME;
		}
		else if (R_lost==right) Centr=RIGHT_EXTREME;
		else if (L_lost==left) Centr=LEFT_EXTREME;
	}

	else if( (L_lost==left) || (R_lost==left)){ // lane gone left
		if(L_lost==left){
			Centr = (CENTER+LEFT)/2 + ((((float)(CAMERA_WIDTH))/2-R_t1)/CAMERA_WIDTH)*(CENTER-LEFT)/2 ;
		} else {
			Centr = (CENTER+RIGHT)/2 + ((((float)(CAMERA_WIDTH))/2-L_t1)/CAMERA_WIDTH)*(RIGHT-CENTER)/2 ;
		}
		//		if(Centr>610){
		//			Centr = 610;
		//		}
	}
	//if( R_t1 >= (R_past-5) && (R_t1-R_past > 20 )
	else if( (R_lost==right) || (L_lost==right)) // lane gone right
	{
		if(L_lost==right){
			Centr = (CENTER+LEFT)/2 + ((((float)(CAMERA_WIDTH))/2-R_t1)/CAMERA_WIDTH)*(CENTER-LEFT)/2;
			//((CAMERA_WIDTH/2-R_t1)*102)/16 ;
		} else {
			Centr = (CENTER+RIGHT)/2 + ((((float)(CAMERA_WIDTH))/2-L_t1)/CAMERA_WIDTH)*(RIGHT-CENTER)/2;
			//((CAMERA_WIDTH/2-L_t1)*102)/16 ;
		}
		//		if(Centr<640){
		//			Centr = 640;
		//		}
	}


	else { // none is lost
		CameraCentr = (((float)(R_t1 + L_t1))/2);
		if ((CameraCentr<(CAMERA_WIDTH/2+12))&&(CameraCentr>(CAMERA_WIDTH/2-12)))
			Centr=CENTER;
		else if(CameraCentr > CAMERA_WIDTH/2)
			Centr = CENTER + ((((float)(CAMERA_WIDTH))/2-CameraCentr)/CAMERA_WIDTH)*(CENTER-LEFT+15);
		else
			Centr = CENTER + ((((float)(CAMERA_WIDTH))/2-CameraCentr)/CAMERA_WIDTH)*(RIGHT-CENTER+15);
	}
	/*
	else {//only one is lost
		if(R_lost==not_lost) CameraCentr=R_t1;
		else CameraCentr=L_t1;
		if(CameraCentr > 63.5)
			Centr = 624 + (((float)63.5-CameraCentr)/32)*200;
		else
			Centr = 624 + (((float)63.5-CameraCentr)/32)*180;
	}
	 */
	if(Centr==RIGHT_EXTREME){}
	else if(Centr>RIGHT)
		Centr = RIGHT;
	if(Centr < LEFT)
		Centr = LEFT;
	if(Centr>=CENTER) target_speed=MAX_SPEED-(MAX_SPEED-MIN_SPEED)*(Centr-CENTER)/(RIGHT-CENTER);
	if(Centr<CENTER) target_speed=MAX_SPEED-(MAX_SPEED-MIN_SPEED)*(CENTER-Centr)/(CENTER-LEFT);
	FrontControl(Centr);
	R_past = R_t1;
	L_past = L_t1;

	usr_sprintf(text,"R_lost: %d  ",R_lost);
	GLCD_displayStringLnOffset(LINE6, text,8);
	usr_sprintf(text,"L_lost: %d  ",L_lost);
	GLCD_displayStringLnOffset(LINE7, text,8);

	//Delay(10);
	/*
	if(Centr >528 && Centr<= 552)
		Centr = 528;         // -30'
	else if(Centr <= 576)
		Centr = 560;		// - 20'
	else if(Centr <= 600)
		Centr = 592;		// -10'
	else if(Centr <= 648 )
		Centr = 624;		// 0'
	else if(Centr <= 672)
		Centr = 656;		//10'
	else if(Centr <= 696)
		Centr = 688;		//20'
	else if(Centr <= 720)
		Centr = 720;		//30'
	 */



}




void Braking(){

	Dio_Configuration(&MODULE_P00, 1, IfxPort_Mode_outputPushPullGeneral,
			IfxPort_PadDriver_cmosAutomotiveSpeed1, IfxPort_State_low);
	//FrontControl(624);
	while (1) {
		BrakeControl(720);
		Delay(50);

	}
}
void BrakeControl(uint16 Angle){


	Pwm_Brake(IfxGtm_TOM1_1_TOUT31_P33_9_OUT,Angle);

}

void BrakeInt(void){

	BrakeControl(600);
	Delay_ms(1000);

}


void Avoid(void)
{
	IfxScuEru_setInterruptGatingPattern(IfxScuEru_OutputChannel_1, IfxScuEru_InterruptGatingPattern_none);
	static uint32 AvoidStartTime=0;
	const uint32 TURNTIME=1000;
	uint16 SR0;
	SR0=(uint16)GTM_TOM1_CH0_SR0.B.SR0;
	if(Right_Lane) {
		FrontControl(LEFT);
		Delay(TURNTIME);
	}
	else {
		FrontControl(RIGHT_EXTREME);
		Delay(TURNTIME+200);
	}
	if(Right_Lane) {
		FrontControl(RIGHT_EXTREME);
		Delay(TURNTIME-100);
	}
	else {
		FrontControl(LEFT);
		Delay(TURNTIME+200);
	}
	Delay(TURNTIME);
	FrontControl(SR0);
	Right_Lane= !Right_Lane;
	Avoiding = 0;
	Object = 0;
	IfxScuEru_setInterruptGatingPattern(IfxScuEru_OutputChannel_1, IfxScuEru_InterruptGatingPattern_patternMatch);
}
/*
	if(Avoiding==1){
		if(AvoidStartTime==0){
			AvoidStartTime=speed_ms;
			SR0=(uint16)GTM_TOM1_CH0_SR0.B.SR0;
		}
		if(Right_Lane) FrontControl(LEFT);
			else FrontControl(RIGHT);
		if(speed_ms-AvoidStartTime>TURNTIME) Avoiding=2;
	} else if (Avoiding==2) {
		if(Right_Lane) FrontControl(RIGHT);
			else FrontControl(LEFT);
		if(speed_ms-AvoidStartTime>TURNTIME * 2){
			Avoiding=0;
			AvoidStartTime=0;
			FrontControl(SR0);
		}
	} else{

	}
}
 */

void Tests(int ch){
	char text[32];
	char text1[32]={0}; // LCD text
	char text2[32]={0};
	char text3[32]={0};
	char text4[32]={0};
	char text5[32]={0};
	char text6[32]={0};
	char text7[32]={0};
	char text8[32]={0};
	switch(ch){
	case 0:
		break;
	case 1:
		while(1){
			CarRuning();
			usr_sprintf(text1, "Ct : %d   ", speed_ms);
			usr_sprintf(text2, "pt : %d   ", pasttime);
			usr_sprintf(text3, "dt : %d   ", delta_t);
			usr_sprintf(text4, "ds : %d   ", duty_speed);
			usr_sprintf(text5, "cs : %d   ", current_speed);
			//usr_sprintf(text6, "double : %ld", 0.5);



			GLCD_displayStringLn(LINE0, text1);
			GLCD_displayStringLn(LINE1, text2);
			GLCD_displayStringLn(LINE2, text3);
			GLCD_displayStringLn(LINE3, text4);
			GLCD_displayStringLn(LINE4, text5);
			//GLCD_displayStringLn(LINE5, text6);

		}
		break;
	case 2:
		CameraAdj();
		int i =0;
		while(1){
			//if(i%3 ==0)
			{CarRuning();}
			CarSteering();

			Delay(10);
			usr_sprintf(text,"%5d",GTM_TOM1_CH3_SR0.B.SR0);
			usr_sprintf(text1,"Centr: %d  ",Centr);
			usr_sprintf(text2,"Cs: %d  ",current_speed);
			usr_sprintf(text3,"ds: %d  ",duty_speed);
			usr_sprintf(text4,"dt: %d  ",delta_t);
			usr_sprintf(text5,"pt: %d  ",pasttime);


			GLCD_displayStringLnOffset(LINE1,text1,8);
			GLCD_displayStringLnOffset(LINE2,text2,8);
			GLCD_displayStringLnOffset(LINE3,text3,8);
			GLCD_displayStringLnOffset(LINE4,text4,8);
			GLCD_displayStringLnOffset(LINE5,text5,8);



			CameraScan();
			//		CameraShow();

		}
		break;
	case 3:
		Dio_Configuration(&MODULE_P13, 1, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_high);
		LidarInit();
		Delay_ms(1000);
		while(1){
			CarRuning();
			LidarExactlyGet(&_Lidar);
			Object = LidarObjectCheck(&_Lidar);//원래 INPUT parameter는 g_ScuEru1.count
			//if(Object == 3)
			//	Braking();

			usr_sprintf(text,"Object :  %2d",Object);
			GLCD_displayStringLn(LINE1, text);
			IfxPort_togglePin(&MODULE_P13, 1 );

		}
		break;


	case 4:
		while(1){
			usr_sprintf(text,"%5d",GTM_TOM1_CH3_SR0.B.SR0);
			usr_sprintf(text1,"School: %d  ",School_Zone);
			usr_sprintf(text2,"Avoiding: %d  ",Avoiding);
			usr_sprintf(text3,"ds: %d  ",duty_speed);
			usr_sprintf(text4,"dt: %d  ",delta_t);
			usr_sprintf(text5,"pt: %d  ",pasttime);


			GLCD_displayStringLnOffset(LINE1,text1,8);
			GLCD_displayStringLnOffset(LINE2,text2,8);
			GLCD_displayStringLnOffset(LINE3,text3,8);
			GLCD_displayStringLnOffset(LINE4,text4,8);
			GLCD_displayStringLnOffset(LINE5,text5,8);


		}
		break;

	case 5:
		CameraAdj();
		BrakeInt();

		//Dio_Configuration(&MODULE_P13, 1, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_high);


		LidarInit();

		Delay_ms(1000);
		while(1){
			//if(i%3 ==0)
			if(IfxPort_getPinState(&MODULE_P02,2) ){
				{CarRuning();}
				CarSteering();
				//Delay(10);
				usr_sprintf(text,"%5d",GTM_TOM1_CH3_SR0.B.SR0);
				usr_sprintf(text1,"Centr: %d  ",Centr);
				usr_sprintf(text2,"Cs: %d  ",current_speed);
				usr_sprintf(text3,"ds: %d  ",duty_speed);
				usr_sprintf(text4,"dt: %d  ",delta_t);
				usr_sprintf(text5,"pt: %d  ",pasttime);
				usr_sprintf(text6,"school :  %2d",School_Zone);
				usr_sprintf(text8,"State :  1st");

				GLCD_displayStringLnOffset(LINE1,text1,8);
				GLCD_displayStringLnOffset(LINE2,text2,8);
				GLCD_displayStringLnOffset(LINE3,text3,8);
				GLCD_displayStringLnOffset(LINE4,text4,8);
				GLCD_displayStringLnOffset(LINE5,text5,8);
				GLCD_displayStringLnOffset(LINE8,text6,8);
				GLCD_displayStringLnOffset(LINE9,text8,8);

				CameraScan();

			}
			else{
				static int a=0;
				{CarRuning();}
				CarSteering();
				if (a==0){
					LidarExactlyGet(&_Lidar);
					Object = LidarObjectCheck(&_Lidar);
				}
				a++;
				if(a==3) a=0;
				if(Avoiding) Avoid();

				if(Object == 3)
					Braking();
				//Delay(10);
				usr_sprintf(text,"%5d",GTM_TOM1_CH3_SR0.B.SR0);
				usr_sprintf(text1,"Centr: %d  ",Centr);
				usr_sprintf(text2,"school: %d  ",School_Zone);
				usr_sprintf(text3,"ds: %d  ",duty_speed);
				usr_sprintf(text4,"dt: %d  ",delta_t);
				usr_sprintf(text5,"pt: %d  ",pasttime);
				usr_sprintf(text6,"Object :  %2d",Object);
				usr_sprintf(text7,"Right :  %2d",Right_Lane);

				GLCD_displayStringLnOffset(LINE1,text1,8);
				GLCD_displayStringLnOffset(LINE2,text2,8);
				GLCD_displayStringLnOffset(LINE3,text3,8);
				GLCD_displayStringLnOffset(LINE4,text4,8);
				GLCD_displayStringLnOffset(LINE5,text5,8);
				GLCD_displayStringLnOffset(LINE8,text6,8);
				GLCD_displayStringLnOffset(LINE9,text7,8);


				CameraScan();
				//		CameraShow();

			}
		}
		break;

	}

}




void core0_main(void){

	__enable ();
	IfxScuWdt_disableCpuWatchdog (IfxScuWdt_getCpuWatchdogPassword ());
	IfxScuWdt_disableSafetyWatchdog (IfxScuWdt_getSafetyWatchdogPassword ());

	g_AppCpu0.info.pllFreq = IfxScuCcu_getPllFrequency();
	g_AppCpu0.info.cpuFreq = IfxScuCcu_getCpuFrequency(IfxCpu_getCoreIndex());
	g_AppCpu0.info.sysFreq = IfxScuCcu_getSpbFrequency();
	g_AppCpu0.info.stmFreq = IfxStm_getFrequency(&MODULE_STM0);

	IfxCpu_enableInterrupts();

	Adc_Initialization();
	Encoder_init();
	SchoolZone_init();
	Dio_Initialization();
	Pwm_Init();
	Uart_Initialization();

	GLCD_init();
	GLCD_clear(COLOR_WHITE);
	GLCD_setBackColor(COLOR_WHITE);
	GLCD_setTextColor(COLOR_BLACK);

	Tests(5);
}
