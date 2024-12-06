/*
 * callbacks.c
 * Author: Toby Erua
 *
 */

#include "class.h"
#include <stdio.h>

void adcBufCallback(ADCBuf_Handle handle, ADCBuf_Conversion *conversion, void *buffer, uint32_t channel, int_fast16_t status){
    if(buffer != adcBufCtrl.RX_Ping && buffer != adcBufCtrl.RX_Pong){
        Swi_post(Bios.ADCSWI);
        return;
    }

    adcBufCtrl.RX_Completed = buffer;
    adcBufCtrl.callback_count++;

    Semaphore_post(Bios.ADCBufSem);
}

void SWIADC(UArg arg0, UArg arg1){
    return;//error
}

void gpioButtonSW1Callback(uint_least8_t index){
    Swi_post(Bios.SW1SWI);
}

void SWIgpioButtonSW1(UArg arg0, UArg arg1){
    int32_t CBNUM = 1;
    uint32_t gateKey;

    gateKey = GateSwi_enter(Bios.CallbackGate);
    if(CB[CBNUM].count != 0){
            if(CB[CBNUM].count > 0){
                CB[CBNUM].count--;
            }
            AddPayload(CB[CBNUM].payload);
    }
    GateSwi_leave(Bios.CallbackGate, gateKey);
}

void gpioButtonSW2Callback(uint_least8_t index){
    Swi_post(Bios.SW2SWI);
}

void SWIgpioButtonSW2(UArg arg0, UArg arg1){
    int32_t CBNUM = 2;
    uint32_t gateKey;

    gateKey = GateSwi_enter(Bios.CallbackGate);
    if(CB[CBNUM].count != 0){
            if(CB[CBNUM].count > 0){
                CB[CBNUM].count--;
            }
            AddPayload(CB[CBNUM].payload);
    }
    GateSwi_leave(Bios.CallbackGate, gateKey);
}

void Timer0Callback(Timer_Handle myTimerHandle, int_fast16_t status){
    int32_t CBNUM = 0;
    CB[CBNUM].HWI_flag = false;

    if(CB[CBNUM].count != 0){
         if(!strncmp(CB[CBNUM].payload,"-sine",5)){
            Sine(CB[CBNUM].payload);
            CB[CBNUM].HWI_flag = true;
        }
    }
    Swi_post(Bios.Timer0SWI);
}

void Timer1Callback(Timer_Handle myTimerHandle, int_fast16_t status){

    Swi_post(Bios.Timer1SWI);
}

void SWITimer0(UArg arg0, UArg arg1){
    int32_t CBNUM = 0;
    uint32_t gateKey;

    gateKey = GateSwi_enter(Bios.CallbackGate);

    if(CB[CBNUM].count != 0){
        if(CB[CBNUM].count > 0){
            CB[CBNUM].count--;
        }
        if(CB[CBNUM].HWI_flag == false && CB[CBNUM].payload[0] != 0){
            AddPayload(CB[CBNUM].payload);
        }
        //adcBufCtrl.callback0_count++;
    }
    GateSwi_leave(Bios.CallbackGate, gateKey);
}

void SWITimer1(UArg arg0, UArg arg1){
    int32_t i;
    uint32_t gateKey;

    gateKey = GateSwi_enter(Bios.TickerGate);
    for(i = 0; i < TICKER_SIZE; i++){
        if(tic[i].delay > 0){
            tic[i].delay--;
            if(tic[i].delay == 0){
                if(tic[i].count != 0){
                    if(tic[i].count > 0){
                        tic[i].count--;
                    }
                    tic[i].delay = tic[i].period;
                }
                if (tic[i].payload[0] != 0){
                    AddPayload(tic[i].payload);
                }
            }
        }
    }

    GateSwi_leave(Bios.TickerGate, gateKey);
}
