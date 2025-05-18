#include "MoRTOS_cli.h"


static u8 rx_pointer = 0;
static u8 rx_data;
static u8 rx_buff[500];
static u32 rx_tick = 0;

// UART receive callback
void CLI_UARTCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance != USART1) return;
    
    rx_tick = uwTick;
    HAL_UART_Receive_IT(&huart1, &rx_data, 1);
    
    if(rx_data >= 32 && rx_data <= 126 || rx_data == '\r' || rx_data == '\n')
    {
        rx_buff[rx_pointer++] = rx_data;
        HAL_UART_Transmit(&huart1, (u8 *)&rx_data, 1, 50);
    }
    else if(rx_data == 8 || rx_data == 127)
    {
        if(rx_pointer > 0)
        {
            rx_pointer--;
            rx_buff[rx_pointer] = 0;
            printf("\b \b");
        }
    }
}

// CLI command processing
int CLI_Process(char* param)
{
    if(uwTick - rx_tick < 50) return 0 ;
    rx_tick = uwTick;
    
    if(rx_pointer > 0)
    {
        if(rx_buff[rx_pointer-1] == '\r' || rx_buff[rx_pointer-1] == '\n')
        {
            rx_buff[rx_pointer-1] = '\0';
            printf("\r\n");
            
            char *cmd = strtok((char*)rx_buff, " ");
            char *param = strtok(NULL, " ");
            
            if(cmd == NULL) {
                printf("MoRTOS$ ");
                rx_pointer = 0;
                memset(rx_buff, 0, sizeof(rx_buff));
                return 0;
            }

            // Command processing logic here
            if(strcmp(cmd, "help") == 0)
            {
                printf("Available commands:\r\n");
								// 打印所有已注册的模块信息
								for (int i = 1; i < g_moduleCount; i++) {
										printf("%d. %s: %s\n", 
												i, 
												g_modules[i].module->name, 
												g_modules[i].module->introduction ? g_modules[i].module->introduction : "No description"
										);
								}
            }
            else if(strcmp(cmd, "version") == 0)
            {
                printf("MoTool V0.1\r\n");
                printf("Build: " __DATE__ " " __TIME__ "\r\n");
            }
            else if(strcmp(cmd, "clear") == 0)
            {
                printf("\033[2J\033[H");  
                printf("MoTool V0.1 - Type 'help' for available commands\r\n");
            }
            else if(strcmp(cmd, "sysinfo") == 0)
            {
                printf("System Information:\r\n");
                printf("  CPU: STM32F407 @ %dMHz\r\n", HAL_RCC_GetSysClockFreq()/1000000);
                printf("  Uptime: %lu ms\r\n", HAL_GetTick());
            }
            else
            {
							static u8 er = 1;
							for (int i = 1; i < g_moduleCount; i++) 
							{
								
								if(strcmp(cmd, g_modules[i].module->name) == 0)
								{
									er = 0;
									g_modules[i].module->process(param); 
								}
								
							}
							if(er)
							{
								printf("Unknown command: %s\r\n", cmd);
								printf("Type 'help' for available commands\r\n");
							}

            }

            printf("MoRTOS$ ");
            rx_pointer = 0;
            memset(rx_buff, 0, sizeof(rx_buff));
        }
    }
		return 0;
}

void CLI_Init(void)
{
    HAL_UART_Receive_IT(&huart1, &rx_data, 1);
    
		printf("\r\n");
		printf(".----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------. \r\n");
		printf("| .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |\r\n");
		printf("| | ____    ____ | || |     ____     | || |  _________   | || |     ____     | || |     ____     | || |   _____      | |\r\n");
		printf("| ||_   \\  /   _|| || |   .'    `.   | || | |  _   _  |  | || |   .'    `.   | || |   .'    `.   | || |  |_   _|     | |\r\n");
		printf("| |  |   \\/   |  | || |  /  .--.  \\  | || | |_/ | | \\_|  | || |  /  .--.  \\  | || |  /  .--.  \\  | || |    | |       | |\r\n");
		printf("| |  | |\\  /| |  | || |  | |    | |  | || |     | |      | || |  | |    | |  | || |  | |    | |  | || |    | |   _   | |\r\n");
		printf("| | _| |_\\/_| |_ | || |  \\  `--'  /  | || |    _| |_     | || |  \\  `--'  /  | || |  \\  `--'  /  | || |   _| |__/ |  | |\r\n");
		printf("| ||_____||_____|| || |   `.____.'   | || |   |_____|    | || |   `.____.'   | || |   `.____.'   | || |  |________|  | |\r\n");
		printf("| |              | || |              | || |              | || |              | || |              | || |              | |\r\n");
		printf("| '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |\r\n");
		printf(" '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------' \r\n");
		printf("\r\n");
		printf("                                  Welcome to MoTool V0.1\r\n");
		printf("                        Multi-function Debug Assistant for STM32\r\n");
		printf("\r\n");
		printf(">> ");
}

// Define CLI module
Module_TypeDef CLI_Module = {
    .name = "CLI",
    .introduction = "Command Line Interface \r\n",
    .init = CLI_Init,
    .deinit = NULL,
    .process = CLI_Process
};
