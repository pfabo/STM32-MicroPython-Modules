// DDS modul pre MicroPython
// 8-Bit DDS generator s vyuzitim DMA a double-buffer
// 
// MCU  STM32L476RG
// VER  0.1
// DTM  12.09.2019
 
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"

#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_dac.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_tim.h"

const uint8_t SineTable[]={
0x80,0x81,0x83,0x84,0x86,0x87,0x89,0x8a,0x8c,0x8e,0x8f,0x91,0x92,0x94,0x95,0x97,
0x98,0x9a,0x9b,0x9d,0x9e,0xa0,0xa2,0xa3,0xa5,0xa6,0xa7,0xa9,0xaa,0xac,0xad,0xaf,
0xb0,0xb2,0xb3,0xb5,0xb6,0xb7,0xb9,0xba,0xbc,0xbd,0xbe,0xc0,0xc1,0xc2,0xc4,0xc5,
0xc6,0xc8,0xc9,0xca,0xcb,0xcd,0xce,0xcf,0xd0,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd9,
0xda,0xdb,0xdc,0xdd,0xde,0xdf,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,
0xea,0xea,0xeb,0xec,0xed,0xee,0xee,0xef,0xf0,0xf1,0xf1,0xf2,0xf3,0xf3,0xf4,0xf5,
0xf5,0xf6,0xf6,0xf7,0xf8,0xf8,0xf9,0xf9,0xfa,0xfa,0xfa,0xfb,0xfb,0xfc,0xfc,0xfc,
0xfd,0xfd,0xfd,0xfd,0xfe,0xfe,0xfe,0xfe,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xfe,0xfe,0xfe,0xfe,0xfd,0xfd,0xfd,
0xfd,0xfc,0xfc,0xfc,0xfb,0xfb,0xfa,0xfa,0xfa,0xf9,0xf9,0xf8,0xf8,0xf7,0xf6,0xf6,
0xf5,0xf5,0xf4,0xf3,0xf3,0xf2,0xf1,0xf1,0xf0,0xef,0xee,0xee,0xed,0xec,0xeb,0xea,
0xea,0xe9,0xe8,0xe7,0xe6,0xe5,0xe4,0xe3,0xe2,0xe1,0xe0,0xdf,0xde,0xdd,0xdc,0xdb,
0xda,0xd9,0xd7,0xd6,0xd5,0xd4,0xd3,0xd2,0xd0,0xcf,0xce,0xcd,0xcb,0xca,0xc9,0xc8,
0xc6,0xc5,0xc4,0xc2,0xc1,0xc0,0xbe,0xbd,0xbc,0xba,0xb9,0xb7,0xb6,0xb5,0xb3,0xb2,
0xb0,0xaf,0xad,0xac,0xaa,0xa9,0xa7,0xa6,0xa5,0xa3,0xa2,0xa0,0x9e,0x9d,0x9b,0x9a,
0x98,0x97,0x95,0x94,0x92,0x91,0x8f,0x8e,0x8c,0x8a,0x89,0x87,0x86,0x84,0x83,0x81,
0x80,0x7e,0x7c,0x7b,0x79,0x78,0x76,0x75,0x73,0x71,0x70,0x6e,0x6d,0x6b,0x6a,0x68,
0x67,0x65,0x64,0x62,0x61,0x5f,0x5d,0x5c,0x5a,0x59,0x58,0x56,0x55,0x53,0x52,0x50,
0x4f,0x4d,0x4c,0x4a,0x49,0x48,0x46,0x45,0x43,0x42,0x41,0x3f,0x3e,0x3d,0x3b,0x3a,
0x39,0x37,0x36,0x35,0x34,0x32,0x31,0x30,0x2f,0x2d,0x2c,0x2b,0x2a,0x29,0x28,0x26,
0x25,0x24,0x23,0x22,0x21,0x20,0x1f,0x1e,0x1d,0x1c,0x1b,0x1a,0x19,0x18,0x17,0x16,
0x15,0x15,0x14,0x13,0x12,0x11,0x11,0x10,0xf,0xe,0xe,0xd,0xc,0xc,0xb,0xa,
0xa,0x9,0x9,0x8,0x7,0x7,0x6,0x6,0x5,0x5,0x5,0x4,0x4,0x3,0x3,0x3,
0x2,0x2,0x2,0x2,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x2,0x2,0x2,
0x2,0x3,0x3,0x3,0x4,0x4,0x5,0x5,0x5,0x6,0x6,0x7,0x7,0x8,0x9,0x9,
0xa,0xa,0xb,0xc,0xc,0xd,0xe,0xe,0xf,0x10,0x11,0x11,0x12,0x13,0x14,0x15,
0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,
0x25,0x26,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2f,0x30,0x31,0x32,0x34,0x35,0x36,0x37,
0x39,0x3a,0x3b,0x3d,0x3e,0x3f,0x41,0x42,0x43,0x45,0x46,0x48,0x49,0x4a,0x4c,0x4d,
0x4f,0x50,0x52,0x53,0x55,0x56,0x58,0x59,0x5a,0x5c,0x5d,0x5f,0x61,0x62,0x64,0x65,
0x67,0x68,0x6a,0x6b,0x6d,0x6e,0x70,0x71,0x73,0x75,0x76,0x78,0x79,0x7b,0x7c,0x7e,
};

//----------------------------------------------------------------------
// Globalne premenne pre obsluhu prenosu double-buffer
#define BUFFER_SIZE 1024            // velkost buffrov pre prenos dat

uint32_t timer_prescaler = 4;       // preddelicka casovaca          
uint32_t timer_reload = 50;         // strop casovaca

uint8_t  buffer_select = 0;         // vyber buffra 0/1 (Low/High)
uint16_t buffer_low[BUFFER_SIZE];   // buffer pre buffer_select = Low             
uint16_t buffer_high[BUFFER_SIZE];  //            buffer_select = High

void DMA_Update(void);              // call back fukcia z prerusenia
                                    // obnovenie DMA prenosu po Transfer Complete
                                    // a inicializacia buffrov novymi hodnotami 
                                    // premenne pre obsluhu DDS
uint32_t phase_acc = 0;             // fazovy akumulator
uint32_t phase_incr = 0x00010000;   // inkrement fazy
uint16_t phase_addr = 0;            // index pola vypocitany z fazoveho akumulatoru 
uint8_t  ampl = 255;                // amplituda signalu

//----------------------------------------------------------------------

void DMA_Config(void){
    NVIC_SetPriority(DMA1_Channel3_IRQn, 1);                // DMA IRQ priority (mensia ako DAC IRQ)
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
     
    LL_DMA_ConfigTransfer(DMA1,
                    LL_DMA_CHANNEL_3,
                    LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
                    LL_DMA_MODE_NORMAL                |  // nie CIRCULAR, po skonceni prenosu menime adresu buffra
                    LL_DMA_PERIPH_NOINCREMENT         |
                    LL_DMA_MEMORY_INCREMENT           |
                    LL_DMA_PDATAALIGN_HALFWORD        |
                    LL_DMA_MDATAALIGN_HALFWORD        |
                    LL_DMA_PRIORITY_HIGH               );
                    
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_3, LL_DMA_REQUEST_6);
                          
    LL_DMA_ConfigAddresses(DMA1,                        // DMA adresa zdroja a spotrebica
                    LL_DMA_CHANNEL_3,
                    (uint32_t) &buffer_low,
                    LL_DAC_DMA_GetRegAddr(DAC1, LL_DAC_CHANNEL_1, LL_DAC_DMA_REG_DATA_8BITS_RIGHT_ALIGNED),
                    LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
                         
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, BUFFER_SIZE);
    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_3);         // povolenie prerusenia pri chybe prenosu
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);         // povolenie prerusenia po ukonceni prenosu
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
}

void DAC_Config(void){
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);    // vystup DAC na PA4
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
    
    NVIC_SetPriority(TIM6_DAC_IRQn, 0);                     // prerusenie od DAC 
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);     // hodiny pre DAC
    LL_DAC_SetTriggerSource(DAC1, LL_DAC_CHANNEL_1, LL_DAC_TRIG_EXT_TIM6_TRGO);
    
    LL_DAC_SetOutputMode(DAC1, LL_DAC_CHANNEL_1, LL_DAC_OUTPUT_MODE_NORMAL);
    LL_DAC_SetOutputBuffer(DAC1, LL_DAC_CHANNEL_1, LL_DAC_OUTPUT_BUFFER_DISABLE);
    LL_DAC_SetOutputConnection(DAC1, LL_DAC_CHANNEL_1, LL_DAC_OUTPUT_CONNECT_GPIO);
    
    LL_DAC_EnableDMAReq(DAC1, LL_DAC_CHANNEL_1);
    LL_DAC_EnableIT_DMAUDR1(DAC1);                          // prerusenie ak DAC nestiha 
    
    LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);
    LL_DAC_EnableTrigger(DAC1, LL_DAC_CHANNEL_1);
}


void TIM6_Config(void){           
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);     // spustíme clock do TIM6 
    LL_TIM_SetPrescaler(TIM6,  timer_prescaler);            // nastavenie preddelicky
    LL_TIM_SetAutoReload(TIM6, timer_reload);               // nastavenie stropu casovaca
    
    LL_TIM_SetCounterMode(TIM6, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetRepetitionCounter(TIM6, 0);
    LL_TIM_SetTriggerOutput(TIM6, LL_TIM_TRGO_UPDATE);
    LL_TIM_EnableCounter(TIM6);                             // spustime casovac
}

void DMA_Update(void){                                  
    // callback funkcia z DMA prerusenia TC (Transfer Complete)
    // - nastavenie buffra pre prenos dat v DMA
    // - obnovenie prenosu
    // - inicializacia buffra novymi hodnotami z tabulky

    uint16_t index;
    
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
    
    if(buffer_select == 0){                             // vyber buffra
        LL_DMA_SetMemoryAddress (DMA1, LL_DMA_CHANNEL_3, (uint32_t) &buffer_low);
    }
    else{
        LL_DMA_SetMemoryAddress (DMA1, LL_DMA_CHANNEL_3, (uint32_t) &buffer_high);
    }
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, BUFFER_SIZE);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);       // obnovenie DMA 

    
    for(index=0; index<BUFFER_SIZE; index++){           // inicializacia nasledujuceho buffra
        phase_acc = phase_acc + phase_incr;
        phase_addr = (phase_acc & 0x01FF0000) >> 16;    // maska pre pole 512 poloziek tabulky- 9bit
        if(buffer_select == 0){            
            buffer_high[index]= (SineTable[phase_addr] * ampl) >> 8;
        }
        else{
            buffer_low[index] = (SineTable[phase_addr] * ampl) >> 8;
        }
    }
    buffer_select = (buffer_select==0) ? 1 : 0;
}

//----------------------------------------------------------------------
// Interrupt handler

void DMA1_Channel3_IRQHandler(void){

    if( LL_DMA_IsActiveFlag_TC3(DMA1) !=0){                 // Transfer complete
        DMA_Update();
        LL_DMA_ClearFlag_TC3(DMA1);
        
    }
    
    if(LL_DMA_IsActiveFlag_TE3(DMA1) == 1){                 // Transfer error
    }
}

//----------------------------------------------------------------------
// Generovany stub s doplnenym kodom
//
//
//Inicializacia DMA, TIM6, DAC a PA4
//:param prescaler: nastavenie preddelicky pre TIM6
//:param period: hodnota periody TIM6 - kroku update
//:return: None
//
STATIC mp_obj_t dds_init(mp_obj_t prescaler_obj, mp_obj_t period_obj) {
    mp_int_t prescaler = mp_obj_get_int(prescaler_obj);
    mp_int_t period = mp_obj_get_int(period_obj);

    //Your code here
    timer_prescaler = prescaler;        
    timer_reload = period;
    
    DMA_Config();
    TIM6_Config();
    DAC_Config();

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(dds_init_obj, dds_init);
//
//Spustenie fazy
//:param step: inkrement fazy
//:return: None
//
STATIC mp_obj_t dds_set(mp_obj_t step_obj) {
    mp_int_t step = mp_obj_get_int(step_obj);

    //Your code here
    phase_incr = step;

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(dds_set_obj, dds_set);

STATIC const mp_rom_map_elem_t dds_module_globals_table[] = {
	{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_dds) },
	{ MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&dds_init_obj) },
	{ MP_ROM_QSTR(MP_QSTR_set), MP_ROM_PTR(&dds_set_obj) },
};

STATIC MP_DEFINE_CONST_DICT(dds_module_globals, dds_module_globals_table);
const mp_obj_module_t dds_user_cmodule = {
	.base = {&mp_type_module},
	.globals = (mp_obj_dict_t*)&dds_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_dds, dds_user_cmodule, MODULE_DDS_ENABLED);

//----------------------------------------------------------------------
