#include "MoRTOS.h"
#include <string.h>
#include <stdio.h>

// 全局变量
ModuleManager_TypeDef g_modules[MAX_MODULES];
int g_moduleCount = 0;
int g_isRunning = 0;

// 初始化 MoRTOS
int MoRTOS_Init(void) {
    memset(g_modules, 0, sizeof(g_modules));
    g_moduleCount = 0;
    return 0;
}

// 注册模块
int Module_Register(Module_TypeDef* module) {
    if (module == NULL || g_moduleCount >= MAX_MODULES) {
        return -1;
    }

    // 检查模块是否已经注册
    for (int i = 0; i < g_moduleCount; i++) {
        if (g_modules[i].module == module) {
            return -2;
        }
    }

    // 注册新模块
    g_modules[g_moduleCount].module = module;
    g_modules[g_moduleCount].state = MODULE_STATE_REGISTERED;
    g_moduleCount++;

    return 0;
}

// 注销模块
int Module_Unregister(Module_TypeDef* module) {
    if (module == NULL) {
        return -1;
    }

    for (int i = 0; i < g_moduleCount; i++) {
        if (g_modules[i].module == module) {
            // 如果模块已初始化，先调用反初始化函数
            if (g_modules[i].state == MODULE_STATE_INITIALIZED && 
                g_modules[i].module->deinit != NULL) {
                g_modules[i].module->deinit();
            }

            // 移除模块
            for (int j = i; j < g_moduleCount - 1; j++) {
                g_modules[j] = g_modules[j + 1];
            }
            g_moduleCount--;
            return 0;
        }
    }

    return -2;
}

// 启动 MoRTOS
int MoRTOS_Start(void) {
    char input_buffer[256];  // 用于存储用户输入
    
    // 初始化所有模块
    for (int i = 0; i < g_moduleCount; i++) {
        if (g_modules[i].module->init != NULL) {
            g_modules[i].module->init();
            g_modules[i].state = MODULE_STATE_INITIALIZED;
        }
    }

    printf("\r\nMoRTOS Started.\r\nMoRTOS$ ");

    while (1) 
		{  // 主循环

        // 调用所有已初始化模块的处理函数
        for (int i = 0; i < g_moduleCount; i++) 
				{
						if (g_modules[i].state == MODULE_STATE_INITIALIZED && g_modules[i].module->process != NULL) {
								int result = g_modules[i].module->process(input_buffer);
								if (result != 0) 
								{
										printf("Module '%s' reported error: %d\n", 
												g_modules[i].module->name, result);
										g_modules[i].state = MODULE_STATE_ERROR;
								}
						}
					}
    }

    // 系统停止，调用所有模块的反初始化函数
    for (int i = 0; i < g_moduleCount; i++) {
        if (g_modules[i].state == MODULE_STATE_INITIALIZED && 
            g_modules[i].module->deinit != NULL) {
            g_modules[i].module->deinit();
        }
    }

    return 0;
}


