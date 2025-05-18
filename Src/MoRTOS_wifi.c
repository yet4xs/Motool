#include "MoRTOS_wifi.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>



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


// UART3发送字符串函数
void UART3_SendString(char* str)
{
    HAL_UART_Transmit(&huart3, (uint8_t*)str, strlen(str), 50);
}





// 定义WiFi模块
const Module_TypeDef WiFi_Module = {
    .name = "WiFi",
    .introduction = "WiFi usage: wifi [scan/connect/disconnect/status/send/at/connect_tcp/disconnect_tcp]\r\n",
    .init = NULL,
    .deinit = NULL,
    .process = NULL
};
