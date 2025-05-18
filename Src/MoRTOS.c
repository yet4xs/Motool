#include "MoRTOS.h"
#include <string.h>
#include <stdio.h>

// ȫ�ֱ���
ModuleManager_TypeDef g_modules[MAX_MODULES];
int g_moduleCount = 0;
int g_isRunning = 0;

// ��ʼ�� MoRTOS
int MoRTOS_Init(void) {
    memset(g_modules, 0, sizeof(g_modules));
    g_moduleCount = 0;
    return 0;
}

// ע��ģ��
int Module_Register(Module_TypeDef* module) {
    if (module == NULL || g_moduleCount >= MAX_MODULES) {
        return -1;
    }

    // ���ģ���Ƿ��Ѿ�ע��
    for (int i = 0; i < g_moduleCount; i++) {
        if (g_modules[i].module == module) {
            return -2;
        }
    }

    // ע����ģ��
    g_modules[g_moduleCount].module = module;
    g_modules[g_moduleCount].state = MODULE_STATE_REGISTERED;
    g_moduleCount++;

    return 0;
}

// ע��ģ��
int Module_Unregister(Module_TypeDef* module) {
    if (module == NULL) {
        return -1;
    }

    for (int i = 0; i < g_moduleCount; i++) {
        if (g_modules[i].module == module) {
            // ���ģ���ѳ�ʼ�����ȵ��÷���ʼ������
            if (g_modules[i].state == MODULE_STATE_INITIALIZED && 
                g_modules[i].module->deinit != NULL) {
                g_modules[i].module->deinit();
            }

            // �Ƴ�ģ��
            for (int j = i; j < g_moduleCount - 1; j++) {
                g_modules[j] = g_modules[j + 1];
            }
            g_moduleCount--;
            return 0;
        }
    }

    return -2;
}

// ���� MoRTOS
int MoRTOS_Start(void) {
    char input_buffer[256];  // ���ڴ洢�û�����
    
    // ��ʼ������ģ��
    for (int i = 0; i < g_moduleCount; i++) {
        if (g_modules[i].module->init != NULL) {
            g_modules[i].module->init();
            g_modules[i].state = MODULE_STATE_INITIALIZED;
        }
    }

    printf("\r\nMoRTOS Started.\r\nMoRTOS$ ");

    while (1) 
		{  // ��ѭ��

        // ���������ѳ�ʼ��ģ��Ĵ�����
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

    // ϵͳֹͣ����������ģ��ķ���ʼ������
    for (int i = 0; i < g_moduleCount; i++) {
        if (g_modules[i].state == MODULE_STATE_INITIALIZED && 
            g_modules[i].module->deinit != NULL) {
            g_modules[i].module->deinit();
        }
    }

    return 0;
}


