#ifndef _PRINT_REGS__H_
#define _PRINT_REGS__H_

#include <Arduino.h>

struct RegOptions {
    Print   &out;
    bool    showDisabled;
};

void printRegs(RegOptions &opts);
void printRegsGCLK(RegOptions &opts);
void printRegsMCLK(RegOptions &opts);
void printRegsTC(RegOptions &opts);

#endif 
