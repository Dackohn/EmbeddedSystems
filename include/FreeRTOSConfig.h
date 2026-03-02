#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <Arduino.h>

/* Core scheduler */
#define configUSE_PREEMPTION            1
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0

#define configCPU_CLOCK_HZ              ( F_CPU )
#define configTICK_RATE_HZ              ( ( TickType_t ) 1000 )

/* AVR specifics */
#define configUSE_16_BIT_TICKS          1
#define configMAX_PRIORITIES            3
#define configMINIMAL_STACK_SIZE        85
#define configMAX_TASK_NAME_LEN         8
#define configIDLE_SHOULD_YIELD         1


#define configTOTAL_HEAP_SIZE           ( 500 )

/* Debug / safety */
#define configCHECK_FOR_STACK_OVERFLOW  2
#define configUSE_MALLOC_FAILED_HOOK    1

/* Synchronization */
#define configUSE_MUTEXES               0
#define configQUEUE_REGISTRY_SIZE       0

/* Allocation model */
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configSUPPORT_STATIC_ALLOCATION  0

/* Optional: smaller footprint (no trace) */
#define configUSE_TRACE_FACILITY        0

#endif /* FREERTOS_CONFIG_H */