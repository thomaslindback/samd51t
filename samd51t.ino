
#define _SAMD51G19A_
#define __SAMD51__

#include <tuple>

#include "printRegs.h"
#include "samd51_tc.h"
#include "samd51_tc_interrupt.h"
#include "utils.h"
#include "samd51_port.h"

RegOptions opts = {Serial, false};
int print = 0;

std::tuple<int, char, int> get_student(int id) {
    if (id == 0) return std::make_tuple(3, 'A', 6);
    if (id == 1) return std::make_tuple(2, 'C', 7);
    if (id == 2) return std::make_tuple(1, 'D', 8);
}

tc_module tc_instance;
tc_module tc_instance_t2;

uint32_t delayVal = 2000;
unsigned long tm = 0;
unsigned long tm2 = 0;

void tc2_callback(tc_module *const module_inst) {
    Serial.print("in callback TC2: ");
    if (tm2 == 0) {
        tm2 = millis();
        Serial.println("");
    } else {
        unsigned long now = millis();
        Serial.print(now - tm2);
        Serial.println(" ms");
        tm2 = now;
    }
}

void tc_callback_to_change_duty_cycle(tc_module *const module_inst) {
    // static uint16_t i = 0;
    // i += 128;
    // tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, i + 1);
    //Serial.print("in callback TC0: ");
    if (tm == 0) {
        tm = millis();
        Serial.println("");
    } else {
        unsigned long now = millis();
        //Serial.print(now - tm);
        //Serial.println(" ms");
        tm = now;
    }
}

void setup_and_start_TC0() {
    tc_config config;
    tc_get_config_defaults(&config);
    config.oneshot = false;
    config.count_direction = TC_COUNT_DIRECTION_UP;
    config.counter_size = TC_COUNTER_SIZE_32BIT;
    config.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
    config.counter_32_bit.value = 0x0;
    config.counter_32_bit.compare_capture_channel[0] = 468750;
    config.counter_32_bit.compare_capture_channel[1] = 0x0000;
    //config.counter_32_bit.compare_capture_channel[1] = 120000000000;
    config.clock_prescaler = TC_CLOCK_PRESCALER_DIV256;
    tc_init(&tc_instance, TC0, &config);
    tc_enable(&tc_instance);
    tc_register_callback(&tc_instance, tc_callback_to_change_duty_cycle,
                          TC_CALLBACK_CC_CHANNEL0);
    tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
}

void setup_events_TC2_ovf_to_pin_13_tgl() {
    MCLK->APBBMASK.reg |= MCLK_APBBMASK_EVSYS;         // Switch on the event system peripheral
 
     // TC2 as geerator
    tc_events event;
    event.generate_event_on_overflow = true;
    //event.generate_event_on_compare_channel[0] = false;
    //event.generate_event_on_compare_channel[1] = false;
    tc_enable_events(&tc_instance_t2, &event);

    // Select the event system user on channel 0 (USER number = channel number + 1)
    EVSYS->USER[EVSYS_ID_USER_PORT_EV_0].reg = EVSYS_USER_CHANNEL(1);         // Set the event user (receiver) as timer TC0

    // const EVSYS_CHANNEL_Type channel0Config = {
    //     bit.EDGSEL = EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val, // 0 when using async
    //     bit.EVGEN = EVSYS_ID_GEN_TC2_OVF,
    //     bit.PATH = EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val
    // };
    // EVSYS->CHANNEL[0].reg = channel0Config.reg;
    
    // Select the event system generator on channel 0
    EVSYS->Channel[0].CHANNEL.reg = EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT |              // No event edge detection
                                  EVSYS_CHANNEL_PATH_ASYNCHRONOUS |                 // Set event path as asynchronous
                                  EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_TC2_OVF);   // Set event generator (sender) as external interrupt 4     

    // input LED
    s_port_input_event_config port_event_config;
    port_input_event_get_config_defaults(&port_event_config);
    port_event_config.action = PORT_INPUT_EVENT_ACTION_TGL;
    port_event_config.gpio_pin = LED_BUILTIN;
    port_input_event_set_config(PORT_INPUT_EVENT_0, &port_event_config);
    port_enable_input_event(LED_BUILTIN, PORT_INPUT_EVENT_0);

}

void setup_and_start_TC2() {
    tc_config config;
    tc_get_config_defaults(&config);
    config.oneshot = false;
    config.count_direction = TC_COUNT_DIRECTION_UP;
    config.counter_size = TC_COUNTER_SIZE_32BIT;
    //config.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
    config.counter_32_bit.value = 0x0;
    //config.counter_32_bit.compare_capture_channel[0] = 468750;
    //config.counter_32_bit.compare_capture_channel[1] = 0x0000;
    //config.counter_32_bit.compare_capture_channel[1] = 120000000000;
    config.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
    tc_init(&tc_instance_t2, TC2, &config);
    setup_events_TC2_ovf_to_pin_13_tgl();
    tc_enable(&tc_instance_t2);
    //tc_register_callback(&tc_instance_t2, tc2_callback,
    //                     TC_CALLBACK_OVERFLOW);
    //tc_enable_callback(&tc_instance_t2, TC_CALLBACK_OVERFLOW);
}


// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    //pinMode(A1, OUTPUT);
    //pinMode(D1, INPUT);
    delay(20);
}

// the loop function runs over and over again forever
void loop() {
                // wait for a second
    //port_toggle_pin(LED_BUILTIN);   // turn the LED off by making the voltage LOW
    delay(delayVal);                  // wait for a second

    //Serial.println("Tick");
    // std::bitset<8> reg(tc_instance.register_callback_mask);
    // Serial.print("register_callback_mask<");
    // Serial.print(reg.to_string().c_str());
    // Serial.println("> ");
    // std::bitset<8> reg2(tc_instance.enable_callback_mask);
    // Serial.print("enable_callback_mask<");
    // Serial.print(reg2.to_string().c_str());
    // Serial.println("> ");

    //Serial.print("Counter val: ");
    //Serial.println(TC0->COUNT32.COUNT.bit.COUNT);
    //std::bitset<8> b(42);
    //Serial.println(b.to_string().c_str());
    ++print;
    if (print == 2) {
        setup_and_start_TC0();
        setup_and_start_TC2();
    }
    if (print == 4) {
        delayVal = 20000;
        printRegs(opts);

        Serial.print("TC0->COUNT32.CTRLA: ");
        printReg<32>(TC0->COUNT32.CTRLA.reg);
        Serial.println("");
        Serial.print("TC0->COUNT32.CC[0].reg: ");
        printReg<32>(TC0->COUNT32.CC[0].reg);
        Serial.println("");
        Serial.print("TC0->COUNT32.COUNT: ");
        TC0->COUNT32.CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
        while (TC0->COUNT32.CTRLBSET.reg & TC_CTRLBSET_CMD_READSYNC)
            ;
        printReg<32>(TC0->COUNT32.COUNT.reg);
        Serial.println("");
        //setup_and_start_TC2();
    }
    EVSYS_ID_USER_TC1_EVU;
    //TC0->COUNT32;
    //Tc *const tc4 = TC1;

    //samd51_tc t1{TC0, TC_COUNTER_SIZE_32BIT};
    // tc_enable(&tc_instance);
    // tc_register_callback(&tc_instance, tc_callback_to_change_duty_cycle,
    //                       TC_CALLBACK_CC_CHANNEL0);
    // tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);

    // TC_CTRLA_MODE_COUNT32;

    // Tc *const tc_modules[TC_INST_NUM] = TC_INSTS;
    //SYSCTRL->OSC32K;

    // struct tc_config config_tc;
    // tc_get_config_defaults(&config_tc);
    // config_tc.counter_size = TC_COUNTER_SIZE_16BIT;
    // config_tc.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;
    // config_tc.counter_16_bit.compare_capture_channel[0] = 0xFFFF;
    // config_tc.pwm_channel[0].enabled = true;
    // config_tc.pwm_channel[0].pin_out = 6;
    // config_tc.pwm_channel[0].pin_mux = 7;
    // tc_init(&tc_instance, TC0, &config_tc);
    // tc_enable(&tc_instance);
    //tc_register_callback(&tc_instance, tc_callback_to_change_duty_cycle,
    //                     TC_CALLBACK_CC_CHANNEL0);
    //tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);

    //_irqs[0];

    //pinPeripheral()
    //auto student0 = get_student(0);
}
