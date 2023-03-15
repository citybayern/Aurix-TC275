#ifndef GPT12_TIMER_INTERRUPT_H_
#define GPT12_TIMER_INTERRUPT_H_

/*********************************************************************************************************************/
/*-----------------------------------------------Function Prototypes-------------------------------------------------*/
/*********************************************************************************************************************/
void initGpt12Timer(void);                          /* Function to initialize the GPT12 Module and start the timer  */
void interruptGpt12(void);                         /* Interrupt Service Routine of the GPT12                       */
void interruptGpt12_T3(void);
void interruptGpt12_T2(void);

#endif /* GPT12_TIMER_INTERRUPT_H_ */
