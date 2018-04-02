/*
 * SchoolZone.c
 *
 *  Created on: 2016. 7. 6.
 *      Author: hlCha
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include <stdio.h>
#include "ScuEruDemo.h"
#include "Src/Std/IfxSrc.h"
#include "Test_Pwm.h"
#include "UART.h"
#include "Cpu0_Main.h"
/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/

App_ScuEru g_ScuEru1; /**< \brief Demo information */
int Avoiding = 0;
/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

/** \addtogroup IfxLld_Demo_ScuEru_SrcDoc_Main_Interrupt
 * \{ */

/** \name Interrupts for input state change
 * \{ */

IFX_INTERRUPT(SchoolZone, 0, ISR_PRIORITY_SCUERU_1);


void SchoolZone(void)
{
	IfxCpu_enableInterrupts();

	//if(startSchool<10000){
	//	g_ScuEru1.count =0;
	//}


	if(speed_ms > 10000){
		School_Zone++;
		if(School_Zone == 2 ){
			School_Zone = 0;
			if(Right_Lane == 0){
				Avoid();
			}
			IfxScuEru_setInterruptGatingPattern(IfxScuEru_OutputChannel_1, IfxScuEru_InterruptGatingPattern_none);
		}
	}


}


/** \brief Demo init API
 *
 * This function is called from main during initialization phase
 */
void SchoolZone_init(void)
{
	/* disable interrupts */
	boolean        interruptState = IfxCpu_disableInterrupts();

	/* select external request pin */
	IfxScu_Req_In *reqPin = &IfxScu_REQ0_P15_4_IN;

	/* initialize this pin with pull-up enabled */
	/* this function will also configure the input multiplexers of the ERU (EICRm.EXISx) */
	IfxScuEru_initReqPin(reqPin, IfxPort_InputMode_pullUp);

	/* determine input channel EICRm depending on input pin */
	IfxScuEru_InputChannel inputChannel = (IfxScuEru_InputChannel)reqPin->channelId;

	/* signal destination */
	IfxScuEru_InputNodePointer triggerSelect = IfxScuEru_InputNodePointer_1;                           /* TRx0 */
	IfxScuEru_OutputChannel    outputChannel = IfxScuEru_OutputChannel_1;                              /* OGU channel 0 */

	/* input channel configuration */
	IfxScuEru_disableFallingEdgeDetection(inputChannel);                                               /* EICRm.FENn */
	IfxScuEru_enableRisingEdgeDetection(inputChannel);                                                 /* EICRm.RENn */
	IfxScuEru_enableAutoClear(inputChannel);                                                           /* EICRm.LDENn */

	/* Connecting Matrix, Event Trigger Logic ETL block */
	IfxScuEru_enableTriggerPulse(inputChannel);                                                        /* EICRm.EIENn */
	IfxScuEru_connectTrigger(inputChannel, triggerSelect);                                             /* EICRm.INPn */

	/* output channels, OutputGating Unit OGU block */
	IfxScuEru_setFlagPatternDetection(outputChannel, inputChannel, TRUE);                              /* IGCRm.IPENn */
	IfxScuEru_enablePatternDetectionTrigger(outputChannel);                                            /* IGCRm.GEENn */
	IfxScuEru_setInterruptGatingPattern(outputChannel, IfxScuEru_InterruptGatingPattern_patternMatch); /* IGCEm.IGPn */

	/* service request configuration */
	{
		volatile Ifx_SRC_SRCR *src = &MODULE_SRC.SCU.SCU.ERU[(int)outputChannel % 4];
		IfxSrc_init(src, IfxSrc_Tos_cpu0, ISR_PRIORITY_SCUERU_1);
		IfxSrc_enable(src);
	}

	/* enable interrupts again */
	IfxCpu_restoreInterrupts(interruptState);

}

