/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// UART3发送字符串函数
void UART3_SendString(char* str)
{
    HAL_UART_Transmit(&huart3, (uint8_t*)str, strlen(str), 50);
}


u8 rx_pointer , rx_data;
u8 rx_buff[500];
u32 rx_tick = 0;


# define  false 0
# define  true 1
	
u8 wifi_rx_buff[256];
u8 wifi_rx_pointer = 0;
u8 wifi_connected = false;
char wifi_ssid[32] = {0};
char wifi_password[32] = {0};
char server_ip[16] = "192.168.1.100"; // 默认服务器IP
u16 server_port = 8080;              // 默认端口
u8 tcp_connected = false;
u8 transparent_mode = false;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	  if(huart->Instance == USART1)
    {
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
							// 
							printf("\b \b");
					}
			}
			// 
    }
		
		else if(huart->Instance == USART3)
		{
				wifi_rx_buff[wifi_rx_pointer++] = rx_data;
				
				// 检查是否收到完整响应（以\n结尾）
				if(rx_data == '\n' || wifi_rx_pointer >= sizeof(wifi_rx_buff)-1)
				{
						wifi_rx_buff[wifi_rx_pointer] = '\0';
						printf("\r\nESP8266: %s", wifi_rx_buff);
						
						// 检查连接状态
						if(strstr((char*)wifi_rx_buff, "WIFI CONNECTED") != NULL) {
								wifi_connected = true;
						}
						else if(strstr((char*)wifi_rx_buff, "WIFI DISCONNECT") != NULL) {
								wifi_connected = false;
						}
						else if(strstr((char*)wifi_rx_buff, "CONNECT") != NULL) {
								tcp_connected = true;
						}
						else if(strstr((char*)wifi_rx_buff, "CLOSED") != NULL) {
								tcp_connected = false;
								transparent_mode = false;
						}
						else if(strstr((char*)wifi_rx_buff, "ERROR") != NULL) {
								transparent_mode = false;
						}
						
						wifi_rx_pointer = 0;
				}
				
				HAL_UART_Receive_IT(&huart3, &rx_data, 1);
		}	

		
		
		
}

void RX_proc()
{
    if(uwTick - rx_tick < 50) return;  // 
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
                printf(">> ");
                rx_pointer = 0;
                memset(rx_buff, 0, sizeof(rx_buff));
                return;
            }


            if(strcmp(cmd, "help") == 0)
            {
                printf("Available commands:\r\n");
                printf("  help                - Show this help message\r\n");
                printf("  version             - Show version information\r\n");
                printf("  clear               - Clear screen\r\n");
                printf("  can [init/send/get] - CAN bus operations\r\n");
                printf("  wifi [scan/connect] - WiFi operations\r\n");
                printf("  rs485 [send/recv]   - RS485 operations\r\n");
                printf("  uart [config/send]  - UART operations\r\n");
                printf("  pwm [start/stop]    - PWM control\r\n");
                printf("  adc [start/read]    - ADC operations\r\n");
                printf("  i2c [scan/read]     - I2C operations\r\n");
                printf("  spi [init/transfer] - SPI operations\r\n");
								printf("  led [number] [on/off] - Led operations\r\n");
                printf("  sysinfo             - Show system information\r\n");
            }
						else if(strcmp(cmd, "led") == 0)
						{
								if(param == NULL) {
										printf("LED usage: led [blue/red] [on/off]\r\n");
								} else {
										char *state = strtok(NULL, " ");
										if(state == NULL) {
												printf("Please specify LED state [on/off]\r\n");
												return;
										}

										if(strcmp(param, "blue") == 0) {
												if(strcmp(state, "on") == 0) {
														HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);  // �͵�ƽ����
														printf("Blue LED turned on\r\n");
												} else if(strcmp(state, "off") == 0) {
														HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);    // �ߵ�ƽϨ��
														printf("Blue LED turned off\r\n");
												} else {
														printf("Invalid state: %s\r\n", state);
												}
										}
										else if(strcmp(param, "red") == 0) {
												if(strcmp(state, "on") == 0) {
														HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);  // �͵�ƽ����
														printf("Red LED turned on\r\n");
												} else if(strcmp(state, "off") == 0) {
														HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);    // �ߵ�ƽϨ��
														printf("Red LED turned off\r\n");
												} else {
														printf("Invalid state: %s\r\n", state);
												}
										}
										else {
												printf("Unknown LED color: %s\r\n", param);
										}
								}
						}
            else if(strcmp(cmd, "can") == 0)
            {
                if(param == NULL) {
                    printf("CAN usage: can [init/send/get]\r\n");
                } else if(strcmp(param, "init") == 0) {
                    printf("Initializing CAN...\r\n");
                    // TODO: CAN
                } else if(strcmp(param, "send") == 0) {
                    printf("Sending CAN message...\r\n");
                    // TODO: CAN
                } else if(strcmp(param, "get") == 0) {
                    printf("Getting CAN message...\r\n");
                    // TODO: CAN
                } else {
                    printf("Unknown CAN command: %s\r\n", param);
                }
            }
						
					else if(strcmp(cmd, "wifi") == 0)
					{
							if(param == NULL) {
									printf("WiFi usage: wifi [scan/connect/disconnect/status/send/at/connect_tcp/disconnect_tcp]\r\n");
							} 
							else if(strcmp(param, "scan") == 0) {
									printf("Scanning WiFi networks...\r\n");
									UART3_SendString("AT+CWLAP\r\n");
							} 
							else if(strcmp(param, "connect") == 0) {
									char* ssid = strtok(NULL, " ");
									char* password = strtok(NULL, " ");
									
									if(ssid != NULL && password != NULL) {
											strncpy(wifi_ssid, ssid, sizeof(wifi_ssid)-1);
											strncpy(wifi_password, password, sizeof(wifi_password)-1);
											
											char cmd[128];
											snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
											UART3_SendString(cmd);
											printf("Connecting to %s...\r\n", ssid);
									} else {
											printf("Usage: wifi connect [SSID] [PASSWORD]\r\n");
									}
							}
							else if(strcmp(param, "disconnect") == 0) {
									UART3_SendString("AT+CWQAP\r\n");
									wifi_connected = false;
									printf("Disconnecting from WiFi...\r\n");
							}
							else if(strcmp(param, "status") == 0) {
									UART3_SendString("AT+CWJAP?\r\n");
							}
							else if(strcmp(param, "send") == 0) {
									if(!tcp_connected) {
											printf("TCP not connected!\r\n");
											return;
									}
									
									char* data = strtok(NULL, "\r\n");
									if(data != NULL) {
											if(transparent_mode) {
													// 透传模式下直接发送数据
													UART3_SendString(data);
													printf("Sent in transparent mode: %s\r\n", data);
											} else {
													// 非透传模式使用AT命令发送
													char cmd[256];
													snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d\r\n", strlen(data));
													UART3_SendString(cmd);
													HAL_Delay(100);
													UART3_SendString(data);
													printf("Sent in normal mode: %s\r\n", data);
											}
									}
							}
							else if(strcmp(param, "at") == 0) {
									char* at_cmd = strtok(NULL, "\r\n");
									if(at_cmd != NULL) {
											char cmd[256];
											snprintf(cmd, sizeof(cmd), "AT+%s\r\n", at_cmd);
											UART3_SendString(cmd);
											printf("Sending AT command: %s\r\n", cmd);
									}
							}
						else if(strcmp(param, "connect_tcp") == 0) {
								char* ip = strtok(NULL, " ");
								char* port_str = strtok(NULL, " ");
								
								if(ip != NULL) strncpy(server_ip, ip, sizeof(server_ip)-1);
								if(port_str != NULL) server_port = atoi(port_str);
								
								char cmd[64];
								snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", 
												 server_ip, server_port);
								UART3_SendString(cmd);
								printf("Connecting to TCP server %s:%d...\r\n", server_ip, server_port);
								
								// 等待连接结果
								uint32_t timeout = HAL_GetTick();
								while(!tcp_connected && (HAL_GetTick() - timeout < 5000)) {
										HAL_Delay(100);
								}
								
								if(!tcp_connected) {
										printf("TCP connection timeout!\n");
								}else
								{
										// TCP连接成功后开启透传模式
										UART3_SendString("AT+CIPMODE=1\r\n");
										HAL_Delay(100);
										UART3_SendString("AT+CIPSEND\r\n");
										transparent_mode = true;
										printf("Entered transparent mode\r\n");
								}
								
								
								
						}
						else if(strcmp(param, "disconnect_tcp") == 0) {
									UART3_SendString("AT+CIPCLOSE\r\n");
									tcp_connected = false;
									printf("Disconnecting TCP...\r\n");
							}
						else if(strcmp(param, "tcp_debug") == 0) {
								printf("TCP Connection Status:\n");
								printf("Server: %s:%d\n", server_ip, server_port);
								printf("Connected: %s\n", tcp_connected ? "Yes" : "No");
								printf("Transparent Mode: %s\n", transparent_mode ? "Yes" : "No");
						}
						else {
									printf("Unknown WiFi command: %s\r\n", param);
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
                printf("Unknown command: %s\r\n", cmd);
                printf("Type 'help' for available commands\r\n");
            }
            

            printf(">> ");
            

            rx_pointer = 0;
            memset(rx_buff, 0, sizeof(rx_buff));
        }
        else if(rx_pointer >= sizeof(rx_buff) - 1)
        {

            printf("\r\nCommand too long!\r\n>> ");
            rx_pointer = 0;
            memset(rx_buff, 0, sizeof(rx_buff));
        }
    }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

	HAL_UART_Receive_IT(&huart1, &rx_data, 1);
	HAL_UART_Receive_IT(&huart3, &rx_data, 1);
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

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		RX_proc();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

#include <stdio.h>
struct __FILE
{
  int handle;
  /* Whatever you require here. If the only file you are using is */
  /* standard output using printf() for debugging, no file handling */
  /* is required. */
};
/* FILE is  in stdio.h. */
FILE __stdout;
int fputc(int ch, FILE *f) 
{
	HAL_UART_Transmit(&huart1,(u8 *)&ch,1,50);
  /* Your implementation of fputc(). */
  return ch;
}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
