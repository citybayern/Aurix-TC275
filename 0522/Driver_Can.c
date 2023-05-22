/***********************************************************************/
/*Include*/
/***********************************************************************/
#include "Driver_Can.h"


/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
AppLedType        g_led;                                    /* 전역 LED 구성 및 제어 구조체 */


/***********************************************************************/
/*Define*/
/***********************************************************************/


/***********************************************************************/
/*Typedef*/
/***********************************************************************/


/***********************************************************************/
/*Static Function Prototype*/
/***********************************************************************/




/* 인터럽트 서비스 루틴(ISR)을 정의하기 위한 매크로. */

IFX_INTERRUPT(canIsrTxHandler, 0, ISR_PRIORITY_CAN_TX);

/***********************************************************************/
/*Variable*/
/***********************************************************************/
App_MulticanBasic g_MulticanBasic; /**< \brief 데모 정보 */
volatile CanRxMsg rec;
int a[8]={0,0x1,0x2,0x3,0x4,0x5,0x6,0x7};

/***********************************************************************/
/*Function*/
/***********************************************************************/

void Driver_Can_Init(void)
{
    /* 모듈 구성 생성 */
    IfxMultican_Can_Config canConfig;
    IfxMultican_Can_initModuleConfig(&canConfig, &MODULE_CAN);

    /* 인터럽트 구성 */
    canConfig.nodePointer[TX_INTERRUPT_SRC_ID].priority = ISR_PRIORITY_CAN_TX;

    /* 모듈 초기화 */
    IfxMultican_Can_initModule(&g_MulticanBasic.drivers.can, &canConfig);

    /* CAN 노드 구성 생성 */
    IfxMultican_Can_NodeConfig canNodeConfig;
    IfxMultican_Can_Node_initConfig(&canNodeConfig, &g_MulticanBasic.drivers.can);

    canNodeConfig.baudrate = 500000UL;                                                                      // CAN 통신 속도 500kbps
    {
        canNodeConfig.nodeId    = IfxMultican_NodeId_0;//(IfxMultican_NodeId)((int)IfxMultican_NodeId_0);   // CAN의 Node ID를 0으로 설정
        canNodeConfig.rxPin     = &IfxMultican_RXD0B_P20_7_IN;                                              // 수신 핀
        canNodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
        canNodeConfig.txPin     = &IfxMultican_TXD0_P20_8_OUT;                                              // 송신 핀
        canNodeConfig.txPinMode = IfxPort_OutputMode_pushPull;

        IfxMultican_Can_Node_init(&g_MulticanBasic.drivers.canSrcNode, &canNodeConfig);                     // CAN 노드 초기화
    }

    /* 메시지 객체 구성 생성 */
    IfxMultican_Can_MsgObjConfig canMsgObjConfig;                                                           // CAN 메시지 객체 구성
    IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, &g_MulticanBasic.drivers.canSrcNode);

    canMsgObjConfig.msgObjId              = 0;                                                              //256개의 메시지 객체 중 첫 번째
    canMsgObjConfig.messageId             = 0x100;
    canMsgObjConfig.acceptanceMask        = 0x7FFFFFFFUL;
    canMsgObjConfig.frame                 = IfxMultican_Frame_transmit;                                     // CAN TX로 설정
    canMsgObjConfig.control.messageLen    = IfxMultican_DataLengthCode_8;                                   // 데이터 길이는 8
    canMsgObjConfig.control.extendedFrame = FALSE;                                                          // 확장 ID를 사용하지 않음
    canMsgObjConfig.control.matchingId    = TRUE;

    canMsgObjConfig.txInterrupt.enabled = TRUE;
    canMsgObjConfig.txInterrupt.srcId = TX_INTERRUPT_SRC_ID;

    /* 메시지 객체 초기화 */
    IfxMultican_Can_MsgObj_init(&g_MulticanBasic.drivers.canSrcMsgObj, &canMsgObjConfig);

    /* IO 포트 설정 */
    IfxPort_setPinModeOutput(STB, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    /* CAN 칩의 STB 핀 설정 (active low) */
    IfxPort_setPinLow(STB);
}


void Driver_Can_TxTest(void)
{
    const uint32 dataLow  = 0x12340000;
    const uint32 dataHigh = 0x9abc0000;

    /* 데이터 송신 */
    {
        IfxMultican_Message msg;
        IfxMultican_Message_init(&msg, 0x100, dataLow, dataHigh, IfxMultican_DataLengthCode_8);

        while (IfxMultican_Can_MsgObj_sendMessage(&g_MulticanBasic.drivers.canSrcMsgObj, &msg) == IfxMultican_Status_notSentBusy)
        {}
    }
}


void CAN_send(CanRxMsg *message)
{


    IfxMultican_Message msg;

    const unsigned dataLow = message->Data[0]|(message->Data[1]<<8)|(message->Data[2]<<16)|(message->Data[3]<<24);
    const unsigned dataHigh = message->Data[4]|(message->Data[5]<<8)|(message->Data[6]<<16)|(message->Data[7]<<24);


    IfxMultican_Message_init(&msg,message->ID,dataLow,dataHigh,message->DLC);

    while (IfxMultican_Can_MsgObj_sendMessage(&g_MulticanBasic.drivers.canSrcMsgObj, &msg) == IfxMultican_Status_notSentBusy)
    {

    }
}


void CAN_TEST(void)
{
    CanRxMsg MES;
    int i=0;
    MES.ID=0x890;
    MES.IDE=0;
    MES.DLC=8;
    for(i=0; i<8; i++)
    {
        MES.Data[i]=a[i];
    }
    CAN_send(&MES);
}


/* 송신 인터럽트가 발생한 후 호출되는 인터럽트 서비스 루틴(ISR).
 * LED1을 켜고 성공적인 CAN 메시지 송신을 나타냅니다.
 */
void canIsrTxHandler(void)
{
    /* LED1을 켜서 CAN 메시지가 성공적으로 전송되었음을 나타냄 */
    blinkLED1();

    //IfxPort_setPinLow(g_led.led1.port, g_led.led1.pinIndex);
}


void initLed(void)
{
    /* ======================================================================
     * LED에 연결된 핀의 설정:
     * ======================================================================
     *  - GPIO 포트 정의
     *  - LED에 연결된 GPIO 핀 정의
     *  - 일반 GPIO 핀 사용 정의 (대체 기능 없음)
     *  - 패드 드라이버 강도 정의
     * ======================================================================
     */
    g_led.led1.port      = &MODULE_P00;
    g_led.led1.pinIndex  = PIN5;
    g_led.led1.mode      = IfxPort_OutputIdx_general;
    g_led.led1.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;

    g_led.led2.port      = &MODULE_P00;
    g_led.led2.pinIndex  = PIN6;
    g_led.led2.mode      = IfxPort_OutputIdx_general;
    g_led.led2.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;

    /* LED에 연결된 핀을 "HIGH" 레벨로 초기화하여 기본 상태에서 LED가 꺼져 있도록 설정합니다. */
    IfxPort_setPinHigh(g_led.led1.port, g_led.led1.pinIndex);
    IfxPort_setPinHigh(g_led.led2.port, g_led.led2.pinIndex);

    /* LED에 연결된 핀에 대한 입력/출력 모드 설정 */
    IfxPort_setPinModeOutput(g_led.led1.port, g_led.led1.pinIndex, IfxPort_OutputMode_pushPull, g_led.led1.mode);
    IfxPort_setPinModeOutput(g_led.led2.port, g_led.led2.pinIndex, IfxPort_OutputMode_pushPull, g_led.led2.mode);

    /* LED에 연결된 핀에 대한 패드 드라이버 모드 설정 */
    IfxPort_setPinPadDriver(g_led.led1.port, g_led.led1.pinIndex, g_led.led1.padDriver);
    IfxPort_setPinPadDriver(g_led.led2.port, g_led.led2.pinIndex, g_led.led2.padDriver);
}


void blinkLED1(void)
{
    //IfxPort_togglePin(LED1);                                                     /* LED의 상태를 토글합니다. */

    IfxPort_setPinHigh(LED1);
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_1ms));    /* 1밀리초 동안 대기합니다. */
    IfxPort_setPinLow(LED1);
}
void blinkLED2(void)
{
    IfxPort_togglePin(LED2);                                                     /* LED의 상태를 토글합니다. */
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_10ms));    /* 500밀리초 동안 대기합니다. */
}
