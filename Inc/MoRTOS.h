#ifndef __MORTOS_H
#define __MORTOS_H

#include <stddef.h>

// �������֧�ֵ�ģ������
#define MAX_MODULES 32

// ģ��״̬����
typedef enum {
    MODULE_STATE_UNREGISTERED = 0,
    MODULE_STATE_REGISTERED,
    MODULE_STATE_INITIALIZED,
    MODULE_STATE_ERROR
} ModuleState_TypeDef;

// ģ������ṹ�嶨��
typedef struct {
    const char* name;           // ģ������
		const char* introduction; 
    void (*init)(void);        // ��ʼ������
    void (*deinit)(void);      // ����ʼ������
    int (*process)(char *);      // ���ڴ�����
    
} Module_TypeDef;


// ģ�����ṹ��
typedef struct {
    Module_TypeDef* module;
    ModuleState_TypeDef state;
} ModuleManager_TypeDef;

// ȫ�ֱ���
extern ModuleManager_TypeDef g_modules[MAX_MODULES];
extern int g_moduleCount;
extern int g_isRunning;

// MoRTOS API
int MoRTOS_Init(void);
int MoRTOS_Start(void);
int Module_Register(Module_TypeDef* module) ;
int Module_Unregister(Module_TypeDef* module);

#endif /* __MORTOS_H */
