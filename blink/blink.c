/* BLINK
 * Simple LED blinking - minimal example
 */
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"

#include <stdlib.h>

#include "stm32l4xx.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_utils.h"

//LED Blinking
//:param n: no. of blink
//:param b: delay
//:return: None
STATIC mp_obj_t blink_blink(mp_obj_t n_obj, mp_obj_t delay_obj) {
    mp_int_t n = mp_obj_get_int(n_obj);
    mp_int_t delay = mp_obj_get_int(delay_obj);
    int i=0;

    //Your code here
    // -----------------------------------------------------------------
    for(i=0; i<n; i++){
        LL_mDelay(delay);
        LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_3);
    }
    // -----------------------------------------------------------------
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(blink_blink_obj, blink_blink);

//
// On-board LED init
//
STATIC mp_obj_t blink_init() {

    //Your code here
    // -----------------------------------------------------------------
     LL_Init1msTick(SystemCoreClock);
     
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    LL_GPIO_StructInit(&GPIO_InitStruct);                   // init the struct with some sensible defaults 
    GPIO_InitStruct.Pin = LL_GPIO_PIN_3;                    // GPIO pin 5; on Nucleo there is an LED
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;         // output speed
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;             // set as output 
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;   // make it a push pull
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);  
    // -----------------------------------------------------------------
    //LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_3);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(blink_init_obj, blink_init);

STATIC const mp_rom_map_elem_t blink_module_globals_table[] = {
	{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_blink) },
	{ MP_ROM_QSTR(MP_QSTR_blink), MP_ROM_PTR(&blink_blink_obj) },
	{ MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&blink_init_obj) },
};

STATIC MP_DEFINE_CONST_DICT(blink_module_globals, blink_module_globals_table);
const mp_obj_module_t blink_user_cmodule = {
	.base = {&mp_type_module},
	.globals = (mp_obj_dict_t*)&blink_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_blink, blink_user_cmodule, MODULE_BLINK_ENABLED);
