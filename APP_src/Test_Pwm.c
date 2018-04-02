/*
 * Test_Pwm.c
 *
 *  Created on: 2016. 7. 4.
 *      Author: a0795
 */


#include "Test_Pwm.h"
#include "Test_Adc.h"
#include "Camera.h"
#include "ConfigurationIsr.h"


App_GtmTomTimer GtmTomCameraClk;
App_GtmTomTimer GtmTomCameraSi;
App_GtmTomTimer GtmTomCameraClkInterrupt;
App_GtmTomTimer GtmTomCameraSiInterrupt;
App_GtmTomTimer GtmTomMsTimer;
App_GtmTomTimer GtmTomDCmotor;
App_GtmTomTimer GtmTomSteering;
App_GtmTomTimer GtmTomBrake;
App_GtmTomTimer GtmTomLidar;
volatile uint8 CameraFinished;

//unsigned int	Gpt_TimerOverflow;
unsigned int	Gpt_MsTimerOverflow;
static volatile unsigned int timeInTicks = 0;//LCD
unsigned long int startSchool=0;
unsigned long int count1=0;
unsigned long int speed_ms =0;
volatile unsigned int ClkCount1=0;
volatile uint32 CameraResult[2][128];

void Delay_ms(uint32 DelayTime)
{

	Gpt_MsTimerOverflow = 0;
//    IfxGtm_Tom_Timer_run(&GtmTomMsTimer.drivers.timerOneMs);

    while(Gpt_MsTimerOverflow < (DelayTime*10)){

    	__nop();

    }

//    IfxGtm_Tom_Timer_stop(&GtmTomMsTimer.drivers.timerOneMs);

}

void Delay(uint32 t) {//LCD
	Delay_ms(t);
}

IFX_INTERRUPT(Gpt_Notification_SystemTick_1ms, 0, ISR_PRIORITY_TIMER_1ms);
void Gpt_Notification_SystemTick_1ms(void)
{
    IfxCpu_enableInterrupts();

    IfxGtm_Tom_Timer_acknowledgeTimerIrq(&GtmTomMsTimer.drivers.timerOneMs);
    Gpt_MsTimerOverflow++;
   // IfxPort_togglePin(&MODULE_P33, 13);
    timeInTicks++;//LCD
    startSchool++;

    if(Gpt_MsTimerOverflow%10==0)
        	speed_ms++;


}

IFX_INTERRUPT(Interrupt_Si, 0, ISR_PRIORITY_CAMERA_SI);
void Interrupt_Si(void){
    IfxCpu_enableInterrupts();
	IfxGtm_Tom_Timer_acknowledgeTimerIrq(&GtmTomCameraSiInterrupt.drivers.timerOneMs);
	IfxPort_togglePin(&MODULE_P13, 0 );
	ClkCount1=0;
	IfxGtm_Tom_Timer_acknowledgeTimerIrq(&GtmTomCameraClkInterrupt.drivers.timerOneMs);
	GTM_TOM1_CH5_IRQ_EN.B.CCU0TC_IRQ_EN=1;
	GTM_TOM1_CH6_IRQ_EN.B.CCU0TC_IRQ_EN=0;
}

IFX_INTERRUPT(Interrupt_CLK, 0, ISR_PRIORITY_CAMERA_CLK);
void Interrupt_CLK(void){
    IfxCpu_enableInterrupts();
	IfxGtm_Tom_Timer_acknowledgeTimerIrq(&GtmTomCameraClkInterrupt.drivers.timerOneMs);
	Test_VadcAutoScan(IfxVadc_GroupId_0);
	CameraResult[0][ClkCount1]=Adc_Result_Scan[0][0];
	CameraResult[1][ClkCount1]=Adc_Result_Scan[0][1];
	if(ClkCount1>=127) {
		CameraFinished++;
		GTM_TOM1_CH5_IRQ_EN.B.CCU0TC_IRQ_EN=0;
	}
	IfxPort_togglePin(&MODULE_P13, 1 );
	ClkCount1++;
	count1++;
}

void Pwm_DutyUpdate(IfxGtm_Tom_ToutMap ChannelInfo, uint16 DutyCycle)
{
    IfxGtm_Tom_Timer *timer = &GtmTomDCmotor.drivers.timerOneMs;

	Ifx_GTM_TOM_CH*   pTomCh;

	IfxGtm_Tom_Timer_disableUpdate(timer);

	pTomCh = (Ifx_GTM_TOM_CH *)(0xF0108000+(0x800*(ChannelInfo.tom)) + 0x40*(ChannelInfo.channel));
	pTomCh->SR0.U = FREQ_20KHZ;
	pTomCh->SR1.U = DutyCycle;

	IfxGtm_Tom_Timer_applyUpdate(timer);

}
void Pwm_Steering(IfxGtm_Tom_ToutMap ChannelInfo,uint16 DutyCycle)
{
    IfxGtm_Tom_Timer *timer = &GtmTomSteering.drivers.timerOneMs;
	Ifx_GTM_TOM_CH*   pTomCh;

	IfxGtm_Tom_Timer_disableUpdate(timer);

	pTomCh = (Ifx_GTM_TOM_CH *)(0xF0108000+(0x800*(ChannelInfo.tom)) + 0x40*(ChannelInfo.channel));
	pTomCh->SR0.U = 0x2048;
	pTomCh->SR1.U = DutyCycle;

	IfxGtm_Tom_Timer_applyUpdate(timer);

}

void Pwm_Brake(IfxGtm_Tom_ToutMap ChannelInfo,uint16 DutyCycle)
{
    IfxGtm_Tom_Timer *timer = &GtmTomBrake.drivers.timerOneMs;
	Ifx_GTM_TOM_CH*   pTomCh;

	IfxGtm_Tom_Timer_disableUpdate(timer);

	pTomCh = (Ifx_GTM_TOM_CH *)(0xF0108000+(0x800*(ChannelInfo.tom)) + 0x40*(ChannelInfo.channel));
	pTomCh->SR0.U = 0x2048;
	pTomCh->SR1.U = DutyCycle;

	IfxGtm_Tom_Timer_applyUpdate(timer);

}

void GtmTomTimer_initMsTimer(void)
{
    {   /* GTM TOM configuration */
        IfxGtm_Tom_Timer_Config MstimerConfig;
        IfxGtm_Tom_Timer_initConfig(&MstimerConfig, &MODULE_GTM);
        MstimerConfig.base.frequency       = 100000;
        MstimerConfig.base.isrPriority     = ISR_PRIORITY_TIMER_1ms; //For Interrupt Enable
        MstimerConfig.base.isrProvider     = IfxSrc_Tos_cpu0;
        MstimerConfig.base.minResolution   = (1.0 / MstimerConfig.base.frequency) / 1000;
        MstimerConfig.base.trigger.enabled = FALSE;

        MstimerConfig.tom                  = IfxGtm_Tom_0;
        MstimerConfig.timerChannel         = IfxGtm_Tom_Ch_14;
        MstimerConfig.clock                = IfxGtm_Cmu_Fxclk_0;

        IfxGtm_Tom_Timer_init(&GtmTomMsTimer.drivers.timerOneMs, &MstimerConfig);
        GTM_TOM0_CH14_SR0.B.SR0 = 	10000; //1500;			// 1500 -> 1.5ms
        IfxGtm_Tom_Timer_run(&GtmTomMsTimer.drivers.timerOneMs);

    }

}

void GtmTomLidar_init(void)
{

        IfxGtm_Tom_Timer_Config timerConfig;
        IfxGtm_Tom_Timer_initConfig(&timerConfig, &MODULE_GTM);

        timerConfig.base.frequency                  = 2000;  //20Khz
        timerConfig.base.isrPriority                = ISR_PRIORITY_Lidar;
        timerConfig.base.isrProvider                = 0;
        timerConfig.base.minResolution              = (1.0 / timerConfig.base.frequency) / 1000;
        timerConfig.tom                             = IfxGtm_Tom_1;
        timerConfig.timerChannel                    = IfxGtm_Tom_Ch_12;
        timerConfig.clock                           = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk3;

        timerConfig.triggerOut                      = &IfxGtm_TOM1_12_TOUT112_P34_0_OUT;
        timerConfig.base.trigger.outputEnabled      = TRUE;
        timerConfig.base.trigger.enabled            = TRUE;	//For Trigger Signal Check via Oscilloscope
        timerConfig.base.trigger.triggerPoint       = 0;
        timerConfig.base.trigger.risingEdgeAtPeriod = TRUE;

        IfxGtm_Tom_Timer_init(&GtmTomLidar.drivers.timerOneMs, &timerConfig);

        GTM_TOM1_CH12_SR0.B.SR0 = 	2000;
        GTM_TOM1_CH12_SR1.B.SR1 = 	1600;
        timerConfig.base.trigger.triggerPoint       = 0;
        timerConfig.base.trigger.enabled            = TRUE;


        IfxGtm_Tom_Timer_run(&GtmTomLidar.drivers.timerOneMs);

}


void Camera_Pwm_Init(void){
#if 0
	IfxGtm_Tom_Pwm_Config tomConfig;    //configuration structure
	IfxGtm_Tom_Pwm_Driver tomHandle;
	IfxGtm_Tom_Pwm_initConfig(&tomConfig, &MODULE_GTM);
	tomConfig.clock = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk4;  //DOES NOT WORK!!!!! WHYYYYYYY
	tomConfig.tomChannel  = IfxGtm_Tom_Ch_4;
	tomConfig.period                   = 10000;
	tomConfig.dutyCycle                = 5000;
	tomConfig.interrupt.ccu0Enabled = FALSE;
	tomConfig.interrupt.isrPriority = 0;
	tomConfig.pin.outputPin = &IfxGtm_TOM0_4_TOUT1_P02_1_OUT;

	IfxGtm_Tom_Pwm_init(&tomHandle, &tomConfig);
#else
	{   /* si signal */
		    IfxGtm_Tom_Timer_Config timerConfig;
		    IfxGtm_Tom_Timer_Config timerInterruptConfig;
		    IfxGtm_Tom_Timer_initConfig(&timerConfig, &MODULE_GTM);
		    timerConfig.base.frequency       = 10;
		    timerConfig.base.isrPriority     = 0;
		    timerConfig.base.isrProvider     = IfxSrc_Tos_cpu0;
		    timerConfig.base.minResolution   = (1.0 / timerConfig.base.frequency) / 1000;

		    //IfxGtm_TOM1_1_TOUT31_P33_9_OUT;

		    timerConfig.tom                             = IfxGtm_Tom_1;
		    timerConfig.timerChannel                    = IfxGtm_Tom_Ch_2;
		    timerConfig.clock                           = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk2;
		    timerConfig.triggerOut                      = &IfxGtm_TOM1_2_TOUT28_P33_6_OUT;

		    timerConfig.base.trigger.outputEnabled      = TRUE;
		    timerConfig.base.trigger.enabled            = TRUE;
		    timerConfig.base.trigger.triggerPoint       = 0;
		    timerConfig.base.trigger.risingEdgeAtPeriod = TRUE;

		    IfxGtm_Tom_Timer_init(&GtmTomCameraSi.drivers.timerOneMs, &timerConfig);

		    GTM_TOM1_CH2_SR0.B.SR0 -= GTM_TOM1_CH2_SR0.B.SR0 % 15; // to sync with CLK
		    GTM_TOM1_CH2_SR1.B.SR1 = 	13;
		    timerConfig.base.trigger.triggerPoint       = 0;

		    timerConfig.base.trigger.enabled            = TRUE;
#if 1
	        IfxGtm_Tom_Timer_initConfig(&timerInterruptConfig, &MODULE_GTM);
	        timerInterruptConfig.base.frequency       = 10;
	        timerInterruptConfig.base.isrPriority     = ISR_PRIORITY_CAMERA_SI; //For Interrupt Enable
	        timerInterruptConfig.base.isrProvider     = IfxSrc_Tos_cpu0;
	        timerInterruptConfig.base.minResolution   = (1.0 / timerInterruptConfig.base.frequency) / 1000;
	        timerInterruptConfig.base.trigger.enabled = FALSE;

	        timerInterruptConfig.tom                  = IfxGtm_Tom_1;
	        timerInterruptConfig.timerChannel         = IfxGtm_Tom_Ch_6;
	        timerInterruptConfig.clock                = IfxGtm_Cmu_Fxclk_2;

	        IfxGtm_Tom_Timer_init(&GtmTomCameraSiInterrupt.drivers.timerOneMs, &timerInterruptConfig);

	        GTM_TOM1_CH6_SR0.B.SR0 = GTM_TOM1_CH2_SR0.B.SR0;
		    IfxGtm_Tom_Timer_run(&GtmTomCameraSiInterrupt.drivers.timerOneMs);
#endif
	        IfxGtm_Tom_Timer_run(&GtmTomCameraSi.drivers.timerOneMs);
		}
		{   /* clk signal */
			IfxGtm_Tom_Timer_Config timerConfig;
		    IfxGtm_Tom_Timer_Config timerInterruptConfig;
			IfxGtm_Tom_Timer_initConfig(&timerConfig, &MODULE_GTM);
			timerConfig.base.frequency       = 25000;
			timerConfig.base.isrPriority     = 0;
			timerConfig.base.isrProvider     = IfxSrc_Tos_cpu0;
			timerConfig.base.minResolution   = (1.0 / timerConfig.base.frequency) / 1000;


			//IfxGtm_TOM1_1_TOUT31_P33_9_OUT;

			timerConfig.tom                             = IfxGtm_Tom_1;
			timerConfig.timerChannel                    = IfxGtm_Tom_Ch_3;
			timerConfig.clock                           = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk2;
			timerConfig.triggerOut                      = &IfxGtm_TOM1_3_TOUT21_P00_12_OUT;

			timerConfig.base.trigger.outputEnabled      = TRUE;
			timerConfig.base.trigger.enabled            = TRUE;
			timerConfig.base.trigger.triggerPoint       = 0;
			timerConfig.base.trigger.risingEdgeAtPeriod = FALSE;

			IfxGtm_Tom_Timer_init(&GtmTomCameraClk.drivers.timerOneMs, &timerConfig);
			//IfxGtm_Tom_Timer_run(&GtmTomValve.drivers.timer);

	//	    GTM_TOM1_CH1_SR0.B.SR0 = 	0x2048; //8264;
			GTM_TOM1_CH3_SR1.B.SR1 = 	7;
			timerConfig.base.trigger.triggerPoint       = 0;

			timerConfig.base.trigger.enabled            = TRUE;
#if 1
	        IfxGtm_Tom_Timer_initConfig(&timerInterruptConfig, &MODULE_GTM);
	        timerInterruptConfig.base.frequency       = 25000;
	        timerInterruptConfig.base.isrPriority     = ISR_PRIORITY_CAMERA_CLK; //For Interrupt Enable
	        timerInterruptConfig.base.isrProvider     = IfxSrc_Tos_cpu0;
	        timerInterruptConfig.base.minResolution   = (1.0 / timerInterruptConfig.base.frequency) / 1000;
	        timerInterruptConfig.base.trigger.enabled = FALSE;

	        timerInterruptConfig.tom                  = IfxGtm_Tom_1;
	        timerInterruptConfig.timerChannel         = IfxGtm_Tom_Ch_5;
	        timerInterruptConfig.clock                = IfxGtm_Cmu_Fxclk_2;

	        IfxGtm_Tom_Timer_init(&GtmTomCameraClkInterrupt.drivers.timerOneMs, &timerInterruptConfig);

		    IfxGtm_Tom_Timer_run(&GtmTomCameraClkInterrupt.drivers.timerOneMs);
#endif
			IfxGtm_Tom_Timer_run(&GtmTomCameraClk.drivers.timerOneMs);

		}
		GTM_TOM1_CH6_IRQ_EN.B.CCU0TC_IRQ_EN=0;
		GTM_TOM1_CH5_IRQ_EN.B.CCU0TC_IRQ_EN=0;
#endif
}

void GtmTomDCmotor_init(void)
{

        IfxGtm_Tom_Timer_Config timerConfig;
        IfxGtm_Tom_Timer_initConfig(&timerConfig, &MODULE_GTM);

        timerConfig.base.frequency                  = 5000;  //20Khz
        timerConfig.base.isrPriority                = ISR_PRIORITY_DCMOTOR;
        timerConfig.base.isrProvider                = 0;
        timerConfig.base.minResolution              = (1.0 / timerConfig.base.frequency) / 1000;
        timerConfig.tom                             = IfxGtm_Tom_0;
        timerConfig.timerChannel                    = IfxGtm_Tom_Ch_10;
        timerConfig.clock                           = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk0;

        timerConfig.triggerOut                      = &IfxGtm_TOM0_10_TOUT12_P00_3_OUT;
        timerConfig.base.trigger.outputEnabled      = TRUE;
        timerConfig.base.trigger.enabled            = TRUE;	//For Trigger Signal Check via Oscilloscope
        timerConfig.base.trigger.triggerPoint       = 0;
        timerConfig.base.trigger.risingEdgeAtPeriod = TRUE;

        IfxGtm_Tom_Timer_init(&GtmTomDCmotor.drivers.timerOneMs, &timerConfig);

        GTM_TOM0_CH10_SR0.B.SR0 = 	FREQ_20KHZ;
        GTM_TOM0_CH10_SR1.B.SR1 = 	1500;
        timerConfig.base.trigger.triggerPoint       = 0;
        timerConfig.base.trigger.enabled            = TRUE;


        IfxGtm_Tom_Timer_run(&GtmTomDCmotor.drivers.timerOneMs);

}

void GtmTomSteering_init(void){


    IfxGtm_Tom_Timer_Config timerConfig;
    IfxGtm_Tom_Timer_initConfig(&timerConfig, &MODULE_GTM);
    timerConfig.base.frequency                  = 100000;
    timerConfig.base.isrPriority                = ISR_PRIORITY_STEERING;
    timerConfig.base.isrProvider                = 0;
    timerConfig.base.minResolution              = (1.0 / timerConfig.base.frequency) / 1000;
    timerConfig.tom                             = IfxGtm_Tom_1;
    timerConfig.timerChannel                    = IfxGtm_Tom_Ch_0;
    timerConfig.clock                           = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk2;
    timerConfig.triggerOut                      = &IfxGtm_TOM1_0_TOUT32_P33_10_OUT;
    timerConfig.base.trigger.outputEnabled      = TRUE;
    timerConfig.base.trigger.enabled            = TRUE;
    timerConfig.base.trigger.triggerPoint       = 0;
    timerConfig.base.trigger.risingEdgeAtPeriod = TRUE;

    IfxGtm_Tom_Timer_init(&GtmTomSteering.drivers.timerOneMs, &timerConfig);

    GTM_TOM1_CH0_SR0.B.SR0 = 	0x2048; //8264;
    GTM_TOM1_CH0_SR1.B.SR1 = 	0x270; //624;
    timerConfig.base.trigger.triggerPoint       = 0;

    timerConfig.base.trigger.enabled            = TRUE;
    IfxGtm_Tom_Timer_run(&GtmTomSteering.drivers.timerOneMs);

}

void Pwm_Brakeinit(void)
{

	        IfxGtm_Tom_Timer_Config timerConfig;
	        IfxGtm_Tom_Timer_initConfig(&timerConfig, &MODULE_GTM);
	        timerConfig.base.frequency       = 100000;
	        timerConfig.base.isrPriority     = ISR_PRIORITY_BRAKE;
	        timerConfig.base.isrProvider     = 0;
	        timerConfig.base.minResolution   = (1.0 / timerConfig.base.frequency) / 1000;
	        timerConfig.tom                             = IfxGtm_Tom_1;
	        timerConfig.timerChannel                    = IfxGtm_Tom_Ch_1;
	        timerConfig.clock                           = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk2;
	        timerConfig.triggerOut                      = &IfxGtm_TOM1_1_TOUT31_P33_9_OUT;

	        timerConfig.base.trigger.outputEnabled      = TRUE;
	        timerConfig.base.trigger.enabled            = TRUE;
	        timerConfig.base.trigger.triggerPoint       = 0;
	        timerConfig.base.trigger.risingEdgeAtPeriod = TRUE;

	        IfxGtm_Tom_Timer_init(&GtmTomBrake.drivers.timerOneMs, &timerConfig);

	        GTM_TOM1_CH1_SR0.B.SR0 = 	0x2048; //8264;
	        GTM_TOM1_CH1_SR1.B.SR1 = 	0x0; //624;
	        timerConfig.base.trigger.triggerPoint       = 0;

	        timerConfig.base.trigger.enabled            = TRUE;
	        IfxGtm_Tom_Timer_run(&GtmTomBrake.drivers.timerOneMs);
}

void Pwm_Init(void){
    /* disable interrupts */
	boolean  interruptState = IfxCpu_disableInterrupts();

    /** - GTM clocks */
    Ifx_GTM *gtm = &MODULE_GTM;
    IfxGtm_enable(gtm);

    /* Set the global clock frequencies */
//    float32 frequency = IfxGtm_Cmu_getModuleFrequency(gtm);
//    IfxGtm_Cmu_setGclkFrequency(gtm, frequency);

//    IfxGtm_Cmu_setClkFrequency(gtm, IfxGtm_Cmu_Clk_1,2000);

    /*init the timer*/
    Camera_Pwm_Init();
    GtmTomTimer_initMsTimer();
    GtmTomDCmotor_init();
    GtmTomSteering_init();
    Pwm_Brakeinit();
    GtmTomLidar_init();
    IfxCpu_restoreInterrupts(interruptState);
    IfxGtm_Cmu_enableClocks(gtm, IFXGTM_CMU_CLKEN_FXCLK);
}

