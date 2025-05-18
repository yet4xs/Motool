#ifndef __MORTOS_H
#define __MORTOS_H

#include <stddef.h>

// 定义最大支持的模块数量
#define MAX_MODULES 32

// 模块状态定义
typedef enum {
    MODULE_STATE_UNREGISTERED = 0,
    MODULE_STATE_REGISTERED,
    MODULE_STATE_INITIALIZED,
    MODULE_STATE_ERROR
} ModuleState_TypeDef;

// 模块基础结构体定义
typedef struct {
    const char* name;           // 模块名称
		const char* introduction; 
    void (*init)(void);        // 初始化函数
    void (*deinit)(void);      // 反初始化函数
    int (*process)(char *);      // 周期处理函数
    
} Module_TypeDef;


// 模块管理结构体
typedef struct {
    Module_TypeDef* module;
    ModuleState_TypeDef state;
} ModuleManager_TypeDef;

// 全局变量
extern ModuleManager_TypeDef g_modules[MAX_MODULES];
extern int g_moduleCount;
extern int g_isRunning;

// MoRTOS API
int MoRTOS_Init(void);
int MoRTOS_Start(void);
int Module_Register(Module_TypeDef* module) ;
int Module_Unregister(Module_TypeDef* module);

#endif /* __MORTOS_H */
