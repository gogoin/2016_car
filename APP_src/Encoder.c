/*
 * Encoder.c
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

App_ScuEru g_ScuEru; /**< \brief Demo information */

unsigned long int delta_t =0;
unsigned long int pasttime =0;

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

IFX_INTERRUPT(Encoder, 0, ISR_PRIORITY_SCUERU_0);

/** \} */

/** \} */

/** \brief Handle Timer interrupt.
 *
 * \isrProvider \ref ISR_PROVIDER_SCUERU_0
 * \isrPriority \ref ISR_PRIORITY_SCUERU_0
 *
 */
void Encoder(void)
{
    IfxCpu_enableInterrupts();

    if(pasttime!=0){
        		delta_t = speed_ms - pasttime;
    }
    pasttime = speed_ms;


}


/** \brief Demo init API
 *
 * This function is called from main during initialization phase
 */
void Encoder_init(void)
{
    /* disable interrupts */
    boolean        interruptState = IfxCpu_disableInterrupts();

    /* select external request pin */
    IfxScu_Req_In *reqPin = &IfxScu_REQ13_P15_5_IN;

    /* initialize this pin with pull-up enabled */
    /* this function will also configure the input multiplexers of the ERU (EICRm.EXISx) */
    IfxScuEru_initReqPin(reqPin, IfxPort_InputMode_pullUp);

    /* determine input channel EICRm depending on input pin */
    IfxScuEru_InputChannel inputChannel = (IfxScuEru_InputChannel)reqPin->channelId;

    /* signal destination */
    IfxScuEru_InputNodePointer triggerSelect = IfxScuEru_InputNodePointer_0;                           /* TRx0 */
    IfxScuEru_OutputChannel    outputChannel = IfxScuEru_OutputChannel_0;                              /* OGU channel 0 */

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
        IfxSrc_init(src, IfxSrc_Tos_cpu0, ISR_PRIORITY_SCUERU_0);
        IfxSrc_enable(src);
    }

    /* enable interrupts again */
    IfxCpu_restoreInterrupts(interruptState);

}


