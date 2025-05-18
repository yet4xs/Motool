#ifndef __MORTOS_CLI_H
#define __MORTOS_CLI_H

#include "MoRTOS.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

extern Module_TypeDef CLI_Module;


void CLI_UARTCallback(UART_HandleTypeDef *huart);



#endif /* __MORTOS_CLI_H */
