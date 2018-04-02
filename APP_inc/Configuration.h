/*
 * Configuration.h
 *
 *  Created on: 2016. 7. 6.
 *      Author: hlCha
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "Ifx_Cfg.h"
#include "ConfigurationIsr.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/** \addtogroup IfxLld_Demo_ScuEru_SrcDoc_Config
 * \{ */

/*______________________________________________________________________________
** Help Macros
**____________________________________________________________________________*/

/**
 * \name Macros for Regression Runs
 * \{ */

#ifndef REGRESSION_RUN_STOP_PASS
#define REGRESSION_RUN_STOP_PASS
#endif

#ifndef REGRESSION_RUN_STOP_FAIL
#define REGRESSION_RUN_STOP_FAIL
#endif

/** \} */

/** \} */

#endif /* 0_SRC_0_APPSW_APP_INC_CONFIGURATION_H_ */
