#include "GPT12_Timer_Interrupt.h"
#include "Ifx_Types.h"
#include "IfxGpt12.h"
#include "IfxPort.h"
#include "Bsp.h"
/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define ISR_PRIORITY_GPT12_TIMER_T3   11
#define ISR_PRIORITY_GPT12_TIMER_T2   10
//#define ISR_PRIORITY_GPT12_TIMER      6                       /* Define the GPT12 Timer interrupt priority            */
#define ISR_PROVIDER_GPT12_TIMER      IfxSrc_Tos_cpu0         /* Interrupt provider                                   */
//#define RELOAD_VALUE                  976u                  /* Reload value to have an interrupt each 500ms         */
#define LED1                           &MODULE_P00, 5          /* LED which toggles in the Interrupt Service Routine   */
#define LED2                           &MODULE_P00, 6
#define TEST_PIN1                     &MODULE_P00, 0
#define TEST_PIN3                     &MODULE_P00, 1
#define RELOAD_VALUE_10ms             977u
#define RELOAD_VALUE_50ms             4883u
#define RELOAD_VALUE                  48828u                  /* Reload value to have an interrupt each 500ms         */
#define WAIT_TIME                     1
/*********************************************************************************************************************/
/*--------------------------------------------Function Implementations-----------------------------------------------*/
/*********************************************************************************************************************/
/* Macro defining the Interrupt Service Routine */
IFX_INTERRUPT(interruptGpt12_T3, 0, ISR_PRIORITY_GPT12_TIMER_T3);
IFX_INTERRUPT(interruptGpt12_T2, 0, ISR_PRIORITY_GPT12_TIMER_T2);


int t3_cnt=0;
/* Function to initialize the GPT12 and start the timer */
void initGpt12Timer(void)
{
    /* Initialize the GPT12 module */
    IfxGpt12_enableModule(&MODULE_GPT120);                                          /* Enable the GPT12 module      */
    IfxGpt12_setGpt1BlockPrescaler(&MODULE_GPT120, IfxGpt12_Gpt1BlockPrescaler_16); /* Set GPT1 block prescaler     */

    /* Initialize the Timer T3 */
    IfxGpt12_T3_setMode(&MODULE_GPT120, IfxGpt12_Mode_timer);                       /* Set T3 to timer mode         */
    IfxGpt12_T3_setTimerDirection(&MODULE_GPT120, IfxGpt12_TimerDirection_down);    /* Set T3 count direction       */
    IfxGpt12_T3_setTimerPrescaler(&MODULE_GPT120, IfxGpt12_TimerInputPrescaler_64); /* Set T3 input prescaler       */
    IfxGpt12_T3_setTimerValue(&MODULE_GPT120, RELOAD_VALUE);                        /* Set T3 start value           */

    /* Initialize the Timer T2 */
    IfxGpt12_T2_setMode(&MODULE_GPT120, IfxGpt12_Mode_timer);                      /* Set T2 to reload mode        */
    //IfxGpt12_T2_setReloadInputMode(&MODULE_GPT120, IfxGpt12_ReloadInputMode_risingEdgeTxIN);   /* Set reload trigger  */
    IfxGpt12_T2_setTimerValue(&MODULE_GPT120, RELOAD_VALUE_10ms);                        /* Set T2 reload value          */

    /* Initialize the interrupt */
    volatile Ifx_SRC_SRCR *src = IfxGpt12_T3_getSrc(&MODULE_GPT120);                /* Get the interrupt address    */
    IfxSrc_init(src, ISR_PROVIDER_GPT12_TIMER, ISR_PRIORITY_GPT12_TIMER_T3);           /* Initialize service request   */
    IfxSrc_enable(src);                                                             /* Enable GPT12 interrupt       */

    volatile Ifx_SRC_SRCR *src_t2 = IfxGpt12_T2_getSrc(&MODULE_GPT120);
    IfxSrc_init(src_t2, ISR_PROVIDER_GPT12_TIMER, ISR_PRIORITY_GPT12_TIMER_T2);
    IfxSrc_enable(src_t2);

    /* Initialize the LED */
    IfxPort_setPinModeOutput(LED1, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED2, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(TEST_PIN1, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(TEST_PIN3, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    IfxGpt12_T3_run(&MODULE_GPT120, IfxGpt12_TimerRun_start);      /* Start the timer              */
    IfxGpt12_T2_run(&MODULE_GPT120, IfxGpt12_TimerRun_start);
}

/* Interrupt Service Routine of the GPT12 */
void interruptGpt12_T3(void)
{
    IfxGpt12_T3_setTimerValue(&MODULE_GPT120, RELOAD_VALUE_50ms);
    //IfxPort_setPin(LED1);
    IfxPort_setPinHigh(TEST_PIN3);
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME));
    IfxPort_setPinLow(TEST_PIN3);
    //IfxPort_togglePin(LED);    /* Toggle LED state             */
}

void interruptGpt12_T2(void)
{
    IfxGpt12_T2_setTimerValue(&MODULE_GPT120, RELOAD_VALUE_10ms);
    //IfxPort_setPin(LED1);
    IfxPort_setPinHigh(TEST_PIN1);
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME));
    IfxPort_setPinLow(TEST_PIN1);
    //IfxPort_togglePin(LED);    /* Toggle LED state             */
}
    /*if(t3_cnt == 29)
    {
        volatile Ifx_SRC_SRCR *src = IfxGpt12_T3_getSrc(&MODULE_GPT120);
        IfxSrc_disable(src);
        //IfxGpt12_T3_run(&MODULE_GPT120, IfxGpt12_TimerRun_stop);
        t3_cnt = -1;
    }
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME));
    IfxPort_setPinLow(LED);
    t3_cnt++;
}*/
