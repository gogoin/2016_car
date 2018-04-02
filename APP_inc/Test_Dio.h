/*
 * Test_Dio.h
 *
 *  Created on: 2016. 7. 6.
 *      Author: hlCha
 */

#ifndef TEST_DIO_H_
#define TEST_DIO_H_

extern void Dio_ToggleOutput(void);
extern void Dio_ReadOutput(void);
extern void Dio_ReadInput(void);
extern void Dio_Initialization(void);
extern void Dio_Configuration(Ifx_P *port, uint8 pinIndex, IfxPort_Mode Mode,IfxPort_PadDriver PadType,IfxPort_State State);


#endif /* 0_SRC_0_APPSW_APP_INC_TEST_DIO_H_ */
