#include "MoRTOS_led.h"


// CLI command processing
int LED_Process(char* param)
{
	
	if(param == NULL) 
	{
		printf("LED usage: led [blue/red] [on/off]\r\n");
	}
	else 
	{
		char *state = strtok(NULL, " ");
		if(state == NULL) 
		{
			printf("Please specify LED state [on/off]\r\n");
			return 0;
		}
		if(strcmp(param, "blue") == 0) 
		{
			if(strcmp(state, "on") == 0) 
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);  
				printf("Blue LED turned on\r\n");
												
			}
			else if(strcmp(state, "off") == 0) 
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);    
				printf("Blue LED turned off\r\n");
			} 
			else 
			{
				printf("Invalid state: %s\r\n", state);
			}
		}
		else if(strcmp(param, "red") == 0) 
		{
			if(strcmp(state, "on") == 0) 
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);  
				printf("Red LED turned on\r\n");
			} 
			else if(strcmp(state, "off") == 0) 
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);    
				printf("Red LED turned off\r\n");
			} 
			else 
			{
				printf("Invalid state: %s\r\n", state);
			}
		}
		else 
		{
			printf("Unknown LED color: %s\r\n", param);
		}
	}
	return 0;
}

// ¶¨ÒåledÄ£¿é
Module_TypeDef LED_Module = {
    .name = "led",
    .introduction = "led [number] [on/off] - Led operations\r\n",
    .init = NULL,
    .deinit = NULL,
    .process = LED_Process
};
