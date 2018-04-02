/*
 * Cpu0_Main.h
 *
 *  Created on: 2016. 7. 6.
 *      Author: hlCha
 */

#ifndef CPU0_MAIN_H_
#define CPU0_MAIN_H_

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "Configuration.h"
#include "Cpu/Std/Ifx_Types.h"
#include "IfxScuWdt.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------------Type Definitions------------------------------*/
/******************************************************************************/

typedef struct
{
    float32 sysFreq;                /**< \brief Actual SPB frequency */
    float32 cpuFreq;                /**< \brief Actual CPU frequency */
    float32 pllFreq;                /**< \brief Actual PLL frequency */
    float32 stmFreq;                /**< \brief Actual STM frequency */
} AppInfo;

/** \brief Application information */
typedef struct
{
    AppInfo info;                               /**< \brief Info object */
} App_Cpu0;

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/

IFX_EXTERN App_Cpu0 g_AppCpu0;
extern int Right_Lane;
#endif /* 0_SRC_0_APPSW_APP_INC_CPU0_MAIN_H_ */
