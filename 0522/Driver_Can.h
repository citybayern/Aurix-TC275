#ifndef DRIVER_CAN
#define DRIVER_CAN

/***********************************************************************/
/*Include*/ 
/***********************************************************************/
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxMultican_Can.h"
#include "IfxPort.h"
#include "Bsp.h"


/***********************************************************************/
/*Define*/ 
/***********************************************************************/

#define STB         &MODULE_P20,6                                // STB 핀을 P20.6 핀에 연결

#define TX_INTERRUPT_SRC_ID         IfxMultican_SrcId_0         /* 전송 인터럽트 서비스 요청 ID 정의 /
#define ISR_PRIORITY_CAN_TX         2                           / CAN TX 인터럽트 우선순위 정의 /
#define PIN5                        5                          / TX ISR에서 사용하는 LED1은 이 핀에 연결됨 /
#define PIN6                        6                          / RX ISR에서 사용하는 LED2는 이 핀에 연결됨 */


#define WAIT_TIME_1ms               1
#define WAIT_TIME_5ms               5
#define WAIT_TIME_10ms              10
#define WAIT_TIME_20ms              20
#define WAIT_TIME_50ms              50
#define WAIT_TIME_100ms             100

#define LED1         &MODULE_P00,5                                   // LED1을 P00.5 핀에 연결
#define LED2         &MODULE_P00,6                                   // LED2를 P00.6 핀에 연결
/***********************************************************************/
/*Typedef*/ 
/***********************************************************************/
typedef struct
{
    struct
    {
        IfxMultican_Can        can;         /*< \brief CAN 드라이버 핸들 */
        IfxMultican_Can_Node   canSrcNode;   /*< \brief CAN 소스 노드 */
        IfxMultican_Can_MsgObj canSrcMsgObj; /**< \brief CAN 소스 메시지 객체 */
    }drivers;
} App_MulticanBasic;


typedef struct
{
    unsigned long ID;                                       // CAN 메시지의 식별자(ID)
    unsigned char IDE;                                       // CAN 메시지의 확장 데이터 프레임 여부
    unsigned char DLC;                                       // CAN 데이터 길이 코드(DLC)
    unsigned char Data[8];                                   // CAN 데이터
} CanRxMsg;

typedef struct
{
    IfxPort_Pin_Config              led1;                   /* LED1 설정 구조체 /
    IfxPort_Pin_Config              led2;                  /* LED2 설정 구조체 */
} AppLedType;


/***********************************************************************/
/*External Variable*/ 
/***********************************************************************/
IFX_EXTERN App_MulticanBasic g_MulticanBasic; // 외부 변수로 선언된 MulticanBasic 구조체


/***********************************************************************/
/*Global Function Prototype*/ 
/***********************************************************************/
extern void Driver_Can_Init(void);                      // CAN 초기화 함수
extern void Driver_Can_TxTest(void);                    // CAN 전송 테스트 함수
extern void CAN_send(CanRxMsg *message);                // CAN 메시지 전송 함수
extern void CAN_TEST(void);                             // CAN 테스트 함수
void initLed(void);                                     // LED 초기화 함수
void blinkLED1(void);                                   // LED1 깜빡이는 함수
void blinkLED2(void);                                   // LED2 깜빡이는 함수
#endif /* DRIVER_STM */
