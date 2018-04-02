/*
 * ConfigurationIsr.h
 *
 *  Created on: 2016. 7. 6.
 *      Author: hlCha
 */

#ifndef CONFIGURATIONISR_H_
#define CONFIGURATIONISR_H_

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/** \brief Build the ISR configuration object
 * \param no interrupt priority
 * \param cpu assign CPU number
 */
#define ISR_ASSIGN(no, cpu)  ((no << 8) + cpu)

/** \brief extract the priority out of the ISR object */
#define ISR_PRIORITY(no_cpu) (no_cpu >> 8)

/** \brief extract the service provider  out of the ISR object */
#define ISR_PROVIDER(no_cpu) (no_cpu % 8)

/**
 * \addtogroup IfxLld_Demo_ScuEru_SrcDoc_InterruptConfig
 * \{ */

/**
 * \name Interrupt priority configuration.
 * The interrupt priority range is [1,255]
 * \{ */

#define ISR_PRIORITY_PRINTF_ASC0_TX 5 /**< \brief Define the ASC0 transmit interrupt priority used by printf.c */
#define ISR_PRIORITY_PRINTF_ASC0_EX 6 /**< \brief Define the ASC0 error interrupt priority used by printf.c */

#define ISR_PRIORITY_SCUERU_0       1
#define ISR_PRIORITY_SCUERU_1       2/**< \brief Define the SCU ERU interrupt priority.  */



#define ISR_PRIORITY_TIMER_1ms  149
#define ISR_PRIORITY_DCMOTOR    110
#define ISR_PRIORITY_STEERING   120
#define ISR_PRIORITY_BRAKE 	    130
#define ISR_PRIORITY_CAMERA_SI  100
#define ISR_PRIORITY_CAMERA_CLK 101
#define ISR_PRIORITY_Lidar 115

/** \} */

/**
 * \name Interrupt service provider configuration.
 * \{ */

#define ISR_PROVIDER_PRINTF_ASC0_TX IfxSrc_Tos_cpu0  /**< \brief Define the ASC0 transmit interrupt provider used by printf.c   */
#define ISR_PROVIDER_PRINTF_ASC0_EX IfxSrc_Tos_cpu0  /**< \brief Define the ASC0 error interrupt provider used by printf.c */

#define ISR_PROVIDER_SCUERU_0       IfxSrc_Tos_cpu0  /**< \brief Define the SCU ERU interrupt provider.  */

/** \} */

/**
 * \name Interrupt configuration.
 * \{ */

#define INTERRUPT_PRINTF_ASC0_TX    ISR_ASSIGN(ISR_PRIORITY_PRINTF_ASC0_TX, ISR_PROVIDER_PRINTF_ASC0_TX) /**< \brief Define the ASC0 transmit interrupt priority used by printf.c */
#define INTERRUPT_PRINTF_ASC0_EX    ISR_ASSIGN(ISR_PRIORITY_PRINTF_ASC0_EX, ISR_PROVIDER_PRINTF_ASC0_EX) /**< \brief Define the ASC0 error interrupt priority used by printf.c */

#define INTERRUPT_SCUERU_0          ISR_ASSIGN(ISR_PRIORITY_SCUERU_0, ISR_PROVIDER_SCUERU_0)             /**< \brief Define the SCU ERU interrupt priority.  */

/** \} */

/** \} */
//------------------------------------------------------------------------------


#endif /* 0_SRC_0_APPSW_APP_INC_CONFIGURATIONISR_H_ */
