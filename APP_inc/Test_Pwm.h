/*
 * Test_Pwm.h
 *
 *  Created on: 2015. 3. 24.
 *      Author: kimwayne
 */

#ifndef TEST_PWM_H_
#define TEST_PWM_H_


/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include <Ifx_Types.h>

#include "SysSe/Bsp/Bsp.h"
#include "IfxGtm_bf.h"

#include "Gtm/Tom/PwmHl/IfxGtm_Tom_PwmHl.h"
#include "Gtm/Tom/Timer/IfxGtm_Tom_Timer.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/** \brief Application information */
typedef struct
{
    struct
    {
        float32 gtmFreq;
        float32 gtmGclkFreq;
    }info;
    struct
    {
        IfxGtm_Tom_Timer timerOneMs;        /**< \brief Timer driver */
    }drivers;
    struct
    {
        uint32 slotOneMs;
    }isrCounter;
} App_GtmTomTimer;

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/


/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/
typedef enum
{
	FREQ_10KHZ		= 10000,	// 100MHz/10000 = 10KHz
	FREQ_20KHZ		= 5000,		// 100MHz/5000 = 20KHz
	FREQ_2KHZ_1Mhz		= 500,		// 1MHz/500 = 2KHz
	FREQ_20KHZ_120MHz	= 3000		// 60MHz/3000 = 20KHz

} GTM_PWM_FREQUENCY_FIXED_CLOCK_0;

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/** \brief Application information */
typedef struct
{
    struct
    {
        float32           gtmFreq;
        float32           gtmGclkFreq;
        float32           gtmCmuClk0Freq; /**< \brief CMU CLK 0 frequency*/
        Ifx_TickTime      stateDeadline;
    }info;
    struct
    {
        IfxGtm_Tom_Timer timer;        /**< \brief Timer driver */
        IfxGtm_Tom_PwmHl pwm;
    }drivers;
    struct
    {
        uint32 slotTimer;
    }isrCounter;
}App_GtmTomPwmHl;

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
extern unsigned long int startschool;
extern unsigned long int count1;
extern unsigned long int speed_ms;

extern void Pwm_Init(void);
extern void Pwm_DutyUpdate(IfxGtm_Tom_ToutMap ChannelInfo,uint16 DutyCycle);
//extern void Delay_us(uint32 DelayTime);
extern void Delay_ms(uint32 DelayTime);
extern void Delay(uint32 t);
extern void GtmTomTimer_initMsTimer(void);
extern void Pwm_Steering(IfxGtm_Tom_ToutMap ChannelInfo,uint16 DutyCycle);
extern void Pwm_Brake(IfxGtm_Tom_ToutMap ChannelInfo,uint16 DutyCycle);

extern volatile uint8 CameraFinished;
extern volatile uint32 CameraResult[2][128];
extern volatile unsigned int ClkCount1;


/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/


#endif /* TEST_PWM_H_ */
