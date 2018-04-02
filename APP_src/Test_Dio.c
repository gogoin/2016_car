/*
 * Test_Dio.c
 *
 *  Created on: 2016. 7. 6.
 *      Author: hlCha
 */

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/

#include "Tricore/Cpu/Std/Ifx_Types.h"
#include "Tricore/Cpu/Std/IfxCpu_Intrinsics.h"
#include "Tricore/Scu/Std/IfxScuWdt.h"
#include "IfxPort_reg.h"
#include "IfxPort.h"

/*******************************************************************************
**            			Private Macro Definitions                             **
*******************************************************************************/

/*******************************************************************************
**                      Private Type Definitions                              **
*******************************************************************************/

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/

/*******************************************************************************
**                      Global Constant Definitions                           **
*******************************************************************************/

/*******************************************************************************
**                      Global Variable Definitions                           **
*******************************************************************************/
uint8	Dio_LevelInput[4] = {0};
uint8	Dio_LevelOutput[4] ={0};

/*******************************************************************************
**                      Private  Constant Definitions                         **
*******************************************************************************/
#define LED_TEST	STD_ON
/*******************************************************************************
**                     Private  Variable Definitions                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
void Dio_ToggleOutput(void);
void Dio_ReadOutput(void);
void Dio_ReadInput(void);
void Dio_Initialization(void);
void Dio_Configuration(Ifx_P *port, uint8 pinIndex, IfxPort_Mode Mode,IfxPort_PadDriver PadType,IfxPort_State State);

void Dio_Initialization(void){


	//Initialization LED Port Pin on TC237 TFT-Board
	Dio_Configuration(&MODULE_P13, 0, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_high);
	Dio_Configuration(&MODULE_P13, 1, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_high);
	Dio_Configuration(&MODULE_P13, 2, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_high);
	Dio_Configuration(&MODULE_P13, 3, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_high);



	//Dio_Configuration(&MODULE_P00, 0, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_high);
	Dio_Configuration(&MODULE_P00, 1, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_low);
	Dio_Configuration(&MODULE_P02, 0, IfxPort_Mode_inputPullUp  ,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_notChanged);
	Dio_Configuration(&MODULE_P02, 2, IfxPort_Mode_inputPullUp  ,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_notChanged);
	//Dio_Configuration(&MODULE_P33, 3, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_high);
	//Dio_Configuration(&MODULE_P33, 4, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_high);

	Dio_Configuration(&MODULE_P33, 10, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_low);
	Dio_Configuration(&MODULE_P33, 9, IfxPort_Mode_outputPushPullGeneral,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_low);


	Dio_Configuration(&MODULE_P15, 5, IfxPort_Mode_inputNoPullDevice,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_low);
	Dio_Configuration(&MODULE_P15, 4, IfxPort_Mode_inputNoPullDevice,IfxPort_PadDriver_cmosAutomotiveSpeed1,IfxPort_State_low);

}




//******************************************************************************
// @Function	 	void Dio_ReadOutput(void)
// @Description   	Read digital output
// @Returnvalue		None
// @Parameters    	None
//******************************************************************************
void Dio_ReadOutput(void)
{
	// Read level of all the Digital Output pins
	Dio_LevelOutput[0]= IfxPort_getPinState(&MODULE_P13, 0 );
	Dio_LevelOutput[1]= IfxPort_getPinState(&MODULE_P13, 1 );
	Dio_LevelOutput[2]= IfxPort_getPinState(&MODULE_P13, 2 );
	Dio_LevelOutput[3]= IfxPort_getPinState(&MODULE_P13, 3 );


} /* Dio_ReadOutput */

//******************************************************************************
// @Function	 	void Dio_ReadInput(void)
// @Description   	Read digital input
// @Returnvalue		None
// @Parameters    	None
//******************************************************************************
void Dio_ReadInput(void)
{
	// Read Digital Input pins
	Dio_LevelInput[0]= IfxPort_getPinState(&MODULE_P33,8       );

} /* Dio_ReadInput */


//******************************************************************************
// @Function	 	void Dio_ToggleOutput(void)
// @Description   	Toggle digital input
// @Returnvalue		None
// @Parameters    	None
//******************************************************************************

void Dio_ToggleOutput(void)
{
	IfxPort_togglePin(&MODULE_P13, 0 );
	IfxPort_togglePin(&MODULE_P13, 1 );
	IfxPort_togglePin(&MODULE_P13, 2 );
	IfxPort_togglePin(&MODULE_P13, 3 );

} /* Dio_ToggleOutput */

void Dio_Configuration(Ifx_P *port, uint8 pinIndex, IfxPort_Mode Mode,IfxPort_PadDriver PadType,IfxPort_State State)
{

	IfxPort_setPinMode(port, pinIndex, Mode);
	IfxPort_setPinPadDriver(port, pinIndex, PadType);
	IfxPort_setPinState(port, pinIndex, State);

}


