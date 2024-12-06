#ifndef CLASS_H
#define CLASS_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/gates/GateSwi.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/ADCBuf.h>

#include <ctype.h>

#include <time.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/Timer.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/Board.h>

extern void *mainThread(void *arg0);

#define BUFFER_SIZE 120
#define PAYLOAD_SIZE 32
#define CALLBACK_SIZE 3
#define TICKER_SIZE 16
#define REGISTER_SIZE 32
#define SCRIPT_SIZE 64
#define SCRIPT_LIMIT 5
#define LUT_SIZE 256
#define DATABLOCK_SIZE 128
#define DATA_DELAY 8
#define DEFAULT_NET_PORT 1000


UART_Params uartParams;
Timer_Params Timer0Params;
SPI_Params spiParams;
ADCBuf_Params adcBufParams;


int bufferoverload;
int unknownCommands;
int GPIOerror;
int QOverflow;
int CBError;
int TimerError;
int TICError;
int RegError;
int SCRError;
int Ifailure;
int out_of_range;

typedef struct _BiosList{
    Task_Handle UART0ReadTask;
    Task_Handle PayloadTask;
    Task_Handle UART7ReadTask;
    Task_Handle ADCStreamTask;
    Task_Handle FirstTask;
    GateSwi_Handle PayloadWriteGate;
    GateSwi_Handle CallbackGate;
    GateSwi_Handle UDPOutWriteGate;
    GateSwi_Handle TickerGate;
    Semaphore_Handle PayloadSem;
    Semaphore_Handle ADCBufSem;
    Semaphore_Handle UDPOutSem;
    Swi_Handle Timer0SWI;
    Swi_Handle Timer1SWI;
    Swi_Handle SW1SWI;
    Swi_Handle SW2SWI;
    Swi_Handle ADCSWI;

} BiosList;

BiosList Bios;

typedef struct _PayloadQ{
    int32_t payloadWriting;
    int32_t payloadReading;
    char payloads[PAYLOAD_SIZE][BUFFER_SIZE];
    int32_t binaryCount[PAYLOAD_SIZE];
} PQ;

PQ PayloadQ;
PQ NetOutQ;
PQ NetInQ;
PQ UartInQ;

typedef struct _callback{
    int32_t index;
    int32_t count;
    bool HWI_flag;
    char payload[BUFFER_SIZE];
} Callback;

Callback CB[CALLBACK_SIZE];

typedef struct _Globals{
    char inp[BUFFER_SIZE];
    UART_Handle uart0;
    UART_Handle uart7;
    Timer_Handle timer0;
    Timer_Handle timer1;
    SPI_Handle spi3;
    ADCBuf_Handle adcBuf;
} Globals;

Globals Glo;

int32_t Timer0Period;

typedef struct _Ticker{
    int32_t index;
    int32_t delay;
    int32_t period;
    int32_t count;
    char payload[BUFFER_SIZE];
} Ticker;

Ticker tic[TICKER_SIZE];

char scripts[SCRIPT_SIZE][BUFFER_SIZE];

typedef struct _LutCtrl{
    uint16_t sinlut[LUT_SIZE + 1];
    double lutDelta;
    double lutPosition;
} LutCtrl;

LutCtrl LUTCtrl;

typedef struct _adcBufControl{
    ADCBuf_Conversion conversion;
    uint32_t converting;
    uint16_t RX_Ping[DATABLOCK_SIZE];
    uint16_t RX_Pong[DATABLOCK_SIZE];
    uint16_t *RX_Completed;
    uint32_t callback_count;
} ADCBufControl;

ADCBufControl adcBufCtrl;

typedef struct _txBufCtrl{
    uint16_t *completed;
    int32_t index;
    uint32_t delay;
    int32_t correction;
    uint16_t ping[DATABLOCK_SIZE];
    uint16_t pong[DATABLOCK_SIZE];
} TXBufControl;

TXBufControl txBufCtrl;


// reg

int32_t reg[REGISTER_SIZE];

#define FTN_READ 1
#define FTN_WRITE 2
#define FTN_MOV 3
#define FTN_INC 4
#define FTN_DEC 5
#define FTN_XCHG 6
#define FTN_ADD 7
#define FTN_SUB 8
#define FTN_NOT 9
#define FTN_AND 10
#define FTN_IOR 11
#define FTN_XOR 12
#define FTN_MAX 13
#define FTN_MIN 14
#define FTN_MUL 15
#define FTN_DIV 16
#define FTN_REM 17
#define FTN_NEG 18
#define FTN_CLEAR 19


// tasks.c -----------------------------------------------------
void TSKAAAFIRST();
void TSKPayload();
void TSKUart0Read();
void TSKADCStream();

void *UDPxmitFxn(void *arg0);
void *recUDPFxn(void *arg0);
// infrastructure ----------------------------------------------
void AddCallback(int32_t index, int32_t count, char *payload);

int32_t AddPayload(char *payload);

void AddScript(int32_t index, char *input);

void appendStr(char *input, char c);

void AddTicker(int32_t index, int32_t delay, int32_t period, int32_t count, char *payload);

void ClearAudioBuffer();

void Driver_init();

void Global_init();

void MsgAddByte(char input);

char* MsgGetNextStr(char *str, bool printing);

void UARTWriteProtected(char *input);

void SinLUT();

// callbacks.c ------------------------------------------------

void adcBufCallback(ADCBuf_Handle handle, ADCBuf_Conversion *conversion, void *buffer, uint32_t channel, int_fast16_t status);
void SWIADC(UArg arg0, UArg arg1);

void gpioButtonSW1Callback(uint_least8_t index);
void SWIgpioButtonSW1(UArg arg0, UArg arg1);

void gpioButtonSW2Callback(uint_least8_t index);
void SWIgpioButtonSW2(UArg arg0, UArg arg1);

void Timer0Callback(Timer_Handle myTimerHandle, int_fast16_t status);
void SWITimer0(UArg arg0, UArg arg1);

void Timer1Callback(Timer_Handle myTimerHandle, int_fast16_t status);
void SWITimer1(UArg arg0, UArg arg1);

// p100.c -----------------------------------------------------

void about();

void callbacks(char *input);

void error();

void gpio(char *input);

void help(char *input);

void Ifstatement(char *input);

void memr(char *input);

void Print(char *input);

void registers(char *input);

void Rem(char *input);

void Script(char *input);

void Sine(char *input);

void Stream(char *input);

void tickers(char *input);

void timer(char *input);

void uart(char *input);

void commands(char *inp);


#endif
