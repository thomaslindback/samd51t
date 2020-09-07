
#include <Arduino.h>
#include "printRegs.h"
#include <initializer_list>
#include "utils.h""

#define PRINTAI(x)            \
    do {                      \
        Serial.print("[");    \
        Serial.print(x, DEC); \
        Serial.print("]");    \
    } while (0)
#define PRINTBIT(x, y)             \
    do {                            \
        if (x.bit.y) {              \
            opts.out.print(" " #y); \
        }                           \
    } while (0)

void printRegs(RegOptions &opts) {
    printRegsMCLK(opts);
    printRegsGCLK(opts);
    printRegsTC(opts);
    printRegsPORT(opts);
    printRegsEVSYS(opts);
}

static const char *const GCLK_PCHCTRL_NAMES[] = {
    "GCLK_OSCCTRL_DFLL48", "GCLK_OSCCTRL_FDPLL0", "GCLK_OSCCTRL_FDPLL1",
    "GCLK_OSCCTRL_FDPLL0_32K, GCLK_OSCCTRL_FDPLL1_32K, GCLK_SDHC0_SLOW, GCLK_SDHC1_SLOW, GCLK_SERCOM[0..7]_SLOW",
    "GCLK_EIC", "GCLK_FREQM_MSR", "GCLK_FREQM_REF", "GCLK_SERCOM0_CORE",
    "GCLK_SERCOM1_CORE", "GCLK_TC0, GCLK_TC1", "GCLK_USB", "GCLK_EVSYS[0]",
    "GCLK_EVSYS[1]", "GCLK_EVSYS[2]", "GCLK_EVSYS[3]", "GCLK_EVSYS[4]",
    "GCLK_EVSYS[5]", "GCLK_EVSYS[6]", "GCLK_EVSYS[7]", "GCLK_EVSYS[8]",
    "GCLK_EVSYS[9]", "GCLK_EVSYS[10]", "GCLK_EVSYS[11]", "GCLK_SERCOM2_CORE",
    "GCLK_SERCOM3_CORE", "GCLK_TCC0, GCLK_TCC1", "GCLK_TC2, GCLK_TC3", "GCLK_CAN0",
    "GCLK_CAN1", "GCLK_TCC2, GCLK_TCC3", "GCLK_TC4, GCLK_TC5", "GCLK_PDEC",
    "GCLK_AC", "GCLK_CCL", "GCLK_SERCOM4_CORE", "GCLK_SERCOM5_CORE",
    "GCLK_SERCOM6_CORE", "GCLK_SERCOM7_CORE", "GCLK_TCC4", "GCLK_TC6, GCLK_TC7",
    "GCLK_ADC0", "GCLK_ADC1", "GCLK_DAC", "GCLK_I2S",
    "GCLK_I2S", "GCLK_SDHC0", "GCLK_SDHC1", "GCLK_CM4_TRACE"};

void printRegsGCLK(RegOptions &opts) {
    while (GCLK->CTRLA.bit.SWRST || GCLK->SYNCBUSY.bit.SWRST ||
           GCLK->SYNCBUSY.bit.GENCTRL0 || GCLK->SYNCBUSY.bit.GENCTRL1 ||
           GCLK->SYNCBUSY.bit.GENCTRL2 || GCLK->SYNCBUSY.bit.GENCTRL3 ||
           GCLK->SYNCBUSY.bit.GENCTRL4 || GCLK->SYNCBUSY.bit.GENCTRL5 ||
           GCLK->SYNCBUSY.bit.GENCTRL6 || GCLK->SYNCBUSY.bit.GENCTRL7 ||
           GCLK->SYNCBUSY.bit.GENCTRL8 || GCLK->SYNCBUSY.bit.GENCTRL9 ||
           GCLK->SYNCBUSY.bit.GENCTRL10 || GCLK->SYNCBUSY.bit.GENCTRL11) {
    }
    Serial.println("--------------------------- GCLK");
    for (uint8_t i = 0; i < 12; ++i) {
        Serial.print("GENCTR");
        PRINTAI(i);
        Serial.print(": ");
        if (!GCLK->GENCTRL[i].bit.GENEN) {
            Serial.println(" -");
            continue;
        }
        PRINTBIT(GCLK->GENCTRL[i], GENEN);
        Serial.print(" ");
        switch (GCLK->GENCTRL[i].bit.SRC) {
            case 0x0:
                Serial.print("XOSC0");
                break;
            case 0x1:
                Serial.print("XOSC1");
                break;
            case 0x2:
                Serial.print("GCLKIN");
                break;
            case 0x3:
                Serial.print("GCLKGEN1");
                break;
            case 0x4:
                Serial.print("OSCULP32K");
                break;
            case 0x5:
                Serial.print("XOSC32K");
                break;
            case 0x6:
                Serial.print("DFLL");
                break;
            case 0x7:
                Serial.print("DPLL0");
                break;
            case 0x8:
                Serial.print("DPLL1");
                break;
        }
        if (GCLK->GENCTRL[i].bit.IDC) {
            Serial.print(" IDC");
        }
        if (GCLK->GENCTRL[i].bit.OOV) {
            Serial.print(" OOV");
        }
        if (GCLK->GENCTRL[i].bit.OE) {
            Serial.print(" OE");
        }
        if (GCLK->GENCTRL[i].bit.DIVSEL) {
            Serial.print(" DIVSEL: ");
            Serial.print(GCLK->GENCTRL[i].bit.DIV);
        }
        if (GCLK->GENCTRL[i].bit.RUNSTDBY) {
            Serial.print(" RUNSTDBY");
        }

        Serial.println("");
    }
    for (uint16_t i = 0; i < 48; ++i) {
        Serial.print("PCHCTRL");
        PRINTAI(i);
        Serial.print(": ");
        if (GCLK->PCHCTRL[i].bit.CHEN) {
            Serial.print("CHEN");
        } else {
            Serial.print("----");
        }
        Serial.print(" Generator: ");
        Serial.print(GCLK->PCHCTRL[i].bit.GEN);
        if (GCLK->PCHCTRL[i].bit.WRTLOCK) {
            Serial.print(" WRTLOCK");
        }
        Serial.print(" ");
        Serial.print(GCLK_PCHCTRL_NAMES[i]);
        Serial.println("");
        //}
    }
}

void printRegsMCLK(RegOptions &opts) {
    Serial.println("--------------------------- MCLK");
    Serial.print("INTENCLR: CKRDY: ");
    if(MCLK->INTENCLR.bit.CKRDY) {
        Serial.println("1");
    } else {
        Serial.println("0");
    }
    Serial.print("INTENSET: CKRDY: ");
    if(MCLK->INTENSET.bit.CKRDY) {
        Serial.println("1");
    } else {
        Serial.println("0");
    }
    Serial.print("INTFLAG: CKRDY: ");
    if(MCLK->INTFLAG.bit.CKRDY) {
        Serial.println("1");
    } else {
        Serial.println("0");
    }
    Serial.print("HSDIV: DIV: ");
    Serial.println(MCLK->HSDIV.bit.DIV);
    Serial.print("CPUDIV: DIV: ");
    Serial.println(MCLK->CPUDIV.bit.DIV);
    Serial.print("AHBMASK: ");
    Serial.print(MCLK->AHBMASK.bit.HPB0_ ? "HPB0_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.HPB1_ ? "HPB1_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.HPB2_ ? "HPB2_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.HPB3_ ? "HPB3_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.DSU_ ? "DSU_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.HMATRIX_ ? "HMATRIX_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.NVMCTRL_ ? "NVMCTRL_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.HSRAM_ ? "HSRAM_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.CMCC_ ? "CMCC_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.DMAC_ ? "DMAC_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.USB_ ? "USB_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.BKUPRAM_ ? "BKUPRAM_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.PAC_ ? "PAC_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.QSPI_ ? "QSPI_ " : ""); 
    Serial.print(MCLK->AHBMASK.bit.SDHC0_ ? "SDHC0_ " : "");
    Serial.print(MCLK->AHBMASK.bit.SDHC1_ ? "SDHC1_ " : "");
    Serial.print(MCLK->AHBMASK.bit.ICM_ ? "ICM_ " : "");
    Serial.print(MCLK->AHBMASK.bit.PUKCC_ ? "PUKCC_ " : "");
    Serial.print(MCLK->AHBMASK.bit.QSPI_2X_ ? "QSPI_2X_ " : "");
    Serial.print(MCLK->AHBMASK.bit.NVMCTRL_SMEEPROM_ ? "NVMCTRL_SMEEPROM_ " : "");
    Serial.println(MCLK->AHBMASK.bit.NVMCTRL_CACHE_ ? "NVMCTRL_CACHE_ " : "");
    
    Serial.print("APBAMASK: ");
    Serial.print(MCLK->APBAMASK.bit.PAC_ ? "PAC " : "");
    Serial.print(MCLK->APBAMASK.bit.PM_ ? "PM " : "");
    Serial.print(MCLK->APBAMASK.bit.MCLK_ ? "MCLK_ " : "");
    Serial.print(MCLK->APBAMASK.bit.RSTC_ ? "RSTC_ " : "");
    Serial.print(MCLK->APBAMASK.bit.OSCCTRL_ ? "OSCCTRL_ " : "");
    Serial.print(MCLK->APBAMASK.bit.OSC32KCTRL_ ? "OSC32KCTRL_ " : "");
    Serial.print(MCLK->APBAMASK.bit.SUPC_ ? "SUPC_ " : "");
    Serial.print(MCLK->APBAMASK.bit.GCLK_ ? "GCLK_ " : "");
    Serial.print(MCLK->APBAMASK.bit.WDT_ ? "WDT_ " : "");
    Serial.print(MCLK->APBAMASK.bit.RTC_ ? "RTC_ " : "");
    Serial.print(MCLK->APBAMASK.bit.EIC_ ? "EIC_ " : "");
    Serial.print(MCLK->APBAMASK.bit.FREQM_ ? "FREQM_ " : "");
    Serial.print(MCLK->APBAMASK.bit.SERCOM0_ ? "SERCOM0_ " : "");
    Serial.print(MCLK->APBAMASK.bit.SERCOM1_ ? "SERCOM1_ " : "");
    Serial.print(MCLK->APBAMASK.bit.TC0_ ? "TC0_ " : "");
    Serial.println(MCLK->APBAMASK.bit.TC1_ ? "TC1_ " : "");

    Serial.print("APBBMASK: ");
    Serial.print(MCLK->APBBMASK.bit.USB_ ? "USB_ " : "");
    Serial.print(MCLK->APBBMASK.bit.DSU_ ? "DSU_ " : "");
    Serial.print(MCLK->APBBMASK.bit.NVMCTRL_ ? "NVMCTRL_ " : "");
    Serial.print(MCLK->APBBMASK.bit.PORT_ ? "PORT_ " : "");
    Serial.print(MCLK->APBBMASK.bit.HMATRIX_ ? "HMATRIX_ " : "");
    Serial.print(MCLK->APBBMASK.bit.EVSYS_ ? "EVSYS_ " : "");
    Serial.print(MCLK->APBBMASK.bit.SERCOM2_ ? "SERCOM2_ " : "");
    Serial.print(MCLK->APBBMASK.bit.SERCOM3_ ? "SERCOM3_ " : "");
    Serial.print(MCLK->APBBMASK.bit.TCC0_ ? "TCC0_ " : "");
    Serial.print(MCLK->APBBMASK.bit.TCC1_ ? "TCC1_ " : "");
    Serial.print(MCLK->APBBMASK.bit.TC2_ ? "TC2_ " : "");
    Serial.print(MCLK->APBBMASK.bit.TC3_ ? "TC3_ " : "");
    Serial.print(MCLK->APBBMASK.bit.TAL_ ? "TAL_ " : "");
    Serial.print(MCLK->APBBMASK.bit.RAMECC_ ? "RAMECC_ " : "");

    Serial.print("APBCMASK: ");
    Serial.print(MCLK->APBCMASK.bit.TCC2_ ? "TCC2_ " : "");
    Serial.print(MCLK->APBCMASK.bit.TCC3_ ? "TCC3_ " : "");
    Serial.print(MCLK->APBCMASK.bit.TC4_ ? "TC4_ " : "");
    Serial.print(MCLK->APBCMASK.bit.TC5_ ? "TC5_ " : "");
    Serial.print(MCLK->APBCMASK.bit.PDEC_ ? "PDEC_ " : "");
    Serial.print(MCLK->APBCMASK.bit.AC_ ? "AC_ " : "");
    Serial.print(MCLK->APBCMASK.bit.AES_ ? "AES_ " : "");
    Serial.print(MCLK->APBCMASK.bit.TRNG_ ? "TRNG_ " : "");
    Serial.print(MCLK->APBCMASK.bit.ICM_ ? "ICM_ " : "");
    Serial.print(MCLK->APBCMASK.bit.QSPI_ ? "QSPI_ " : "");
    Serial.print(MCLK->APBCMASK.bit.CCL_ ? "CCL_ " : "");
    
    Serial.print("APBDMASK: ");
    Serial.print(MCLK->APBDMASK.bit.SERCOM4_ ? "SERCOM4_ " : "");
    Serial.print(MCLK->APBDMASK.bit.SERCOM5_ ? "SERCOM5_ " : "");
    Serial.print(MCLK->APBDMASK.bit.SERCOM6_ ? "SERCOM6_ " : "");
    Serial.print(MCLK->APBDMASK.bit.SERCOM7_ ? "SERCOM7_ " : "");
    Serial.print(MCLK->APBDMASK.bit.TCC4_ ? "TCC4_ " : "");
    Serial.print(MCLK->APBDMASK.bit.TC6_ ? "TC6_ " : "");
    Serial.print(MCLK->APBDMASK.bit.TC7_ ? "TC7_ " : "");
    Serial.print(MCLK->APBDMASK.bit.ADC0_ ? "ADC0_ " : "");
    Serial.print(MCLK->APBDMASK.bit.ADC1_ ? "ADC1_ " : "");
    Serial.print(MCLK->APBDMASK.bit.DAC_ ? "DAC_ " : "");
    Serial.print(MCLK->APBDMASK.bit.I2S_ ? "I2S_ " : "");
    Serial.print(MCLK->APBDMASK.bit.PCC_ ? "PCC_ " : "");
    Serial.println("");
}

void printRegsTC(RegOptions &opts) {
    Serial.println("--------------------------- TC");
    int i = 0;
    for(auto tc: TC_INSTS) {
        Serial.print("TC");
        Serial.print(i);
        Serial.print(": ");
        if(tc->COUNT8.CTRLA.bit.MODE == 0) {
            Serial.println("COUNT16 ");
        } 
        else if(tc->COUNT8.CTRLA.bit.MODE == 1) {
            Serial.println("COUNT8 ");
        } else {
            Serial.println("COUNT32 ");
        }
        Serial.print("    CTRLA: ");
        Serial.print(tc->COUNT8.CTRLA.bit.ENABLE ? "ENABLE " : "");
        Serial.print(tc->COUNT8.CTRLA.bit.SWRST ? "SWRST " : "");
        Serial.print(tc->COUNT8.CTRLA.bit.RUNSTDBY ? "RUNSTDBY " : "");
        Serial.print(tc->COUNT8.CTRLA.bit.ONDEMAND ? "ONDEMAND " : "");
        switch (tc->COUNT8.CTRLA.bit.PRESCALER) {
            case TC_CTRLA_PRESCALER_DIV1_Val:
                Serial.print("DIV1 ");
                break;
            case TC_CTRLA_PRESCALER_DIV2_Val:
                Serial.print("DIV2 ");
                break;
            case TC_CTRLA_PRESCALER_DIV4_Val:
                Serial.print("DIV4 ");
                break;
            case TC_CTRLA_PRESCALER_DIV8_Val:
                Serial.print("DIV8 ");
                break;
            case TC_CTRLA_PRESCALER_DIV16_Val:
                Serial.print("DIV16 ");
                break;
            case TC_CTRLA_PRESCALER_DIV64_Val:
                Serial.print("DIV64 ");
                break;
            case TC_CTRLA_PRESCALER_DIV256_Val:
                Serial.print("DIV256 ");
                break;
            case TC_CTRLA_PRESCALER_DIV1024_Val:
                Serial.print("DIV1024 ");
                break;
        }
        Serial.print(tc->COUNT8.CTRLA.bit.ALOCK ? "ALOCK " : "");
        Serial.print(tc->COUNT8.CTRLA.bit.CAPTEN0 ? "CAPTEN0 " : "");
        Serial.print(tc->COUNT8.CTRLA.bit.CAPTEN1 ? "CAPTEN1 " : "");
        Serial.print(tc->COUNT8.CTRLA.bit.COPEN0 ? "COPEN0 " : "");
        Serial.print(tc->COUNT8.CTRLA.bit.COPEN1 ? "COPEN1 " : "");
        if(tc->COUNT8.CTRLA.bit.CAPTMODE0 == TC_CTRLA_CAPTMODE0_DEFAULT_Val) {
            Serial.print("CAPTMODE0_DEFAULT ");
        } else if(tc->COUNT8.CTRLA.bit.CAPTMODE0 == TC_CTRLA_CAPTMODE0_CAPTMIN_Val) {
            Serial.print("CAPTMODE0_CAPTMIN ");
        } else {
             Serial.print("CAPTMODE0_CAPTMAX ");
        }
        if(tc->COUNT8.CTRLA.bit.CAPTMODE1 == TC_CTRLA_CAPTMODE1_DEFAULT_Val) {
            Serial.print("CAPTMODE1_DEFAULT ");
        } else if(tc->COUNT8.CTRLA.bit.CAPTMODE1 == TC_CTRLA_CAPTMODE1_CAPTMIN_Val) {
            Serial.print("CAPTMODE1_CAPTMIN ");
        } else {
             Serial.print("CAPTMODE1_CAPTMAX ");
        }
        Serial.println("");
        Serial.print("    CTRLBCLR: ");
        Serial.print(tc->COUNT8.CTRLBCLR.reg, BIN);
        Serial.print(" ");
        Serial.print(tc->COUNT8.CTRLBCLR.bit.DIR ? "DOWN " : "UP ");
        Serial.print(tc->COUNT8.CTRLBCLR.bit.LUPD ? "LUPD " : "");
        Serial.print(tc->COUNT8.CTRLBCLR.bit.ONESHOT ? "ONESHOT  " : "");
        Serial.print("CMD: ");
        switch (tc->COUNT8.CTRLBCLR.bit.CMD) {
            case TC_CTRLBCLR_CMD_NONE_Val:
                Serial.println("NONE ");
                break;
            case TC_CTRLBCLR_CMD_RETRIGGER_Val:
                Serial.println("RETRIGGER ");
                break;
            case TC_CTRLBCLR_CMD_STOP_Val:
                Serial.println("RETRIGGER ");
                break;
            case TC_CTRLBCLR_CMD_UPDATE_Val:
                Serial.println("UPDATE ");
                break;
            case TC_CTRLBCLR_CMD_READSYNC_Val:
                Serial.println("READSYNC ");
                break;
            case TC_CTRLBCLR_CMD_DMAOS_Val:
                Serial.println("DMAOS");
                break;
        }
        Serial.print("    EVCTRL: ");
        printReg<16>(tc->COUNT8.EVCTRL.reg);
        Serial.println("");
        Serial.print("    STATUS: ");
        printReg<8>(tc->COUNT8.STATUS.reg);
        PRINTBIT(tc->COUNT8.STATUS, STOP);
        PRINTBIT(tc->COUNT8.STATUS, SLAVE);
        PRINTBIT(tc->COUNT8.STATUS, PERBUFV);
        PRINTBIT(tc->COUNT8.STATUS, CCBUFV0);
        PRINTBIT(tc->COUNT8.STATUS, CCBUFV1);
        Serial.println("");
        Serial.print("    INTENSET: ");
        printReg<8>(tc->COUNT8.INTENSET.reg);
        Serial.println("");

        i++;
        Serial.println(" ");
    }
}

void printRegsPORT(RegOptions &opts) {

    Serial.println("--------------------------- PORT");
    int i = 0;
    for(int i = 0; i < 4; i++) {
        Serial.print("PORT");
        PRINTAI(i);
        Serial.println(": ");

        PortGroup *pg = &PORT->Group[i]; 
        if(pg->DIR.reg) {
            Serial.print("    DIR: ");
            printReg<32>(pg->DIR.reg);
            Serial.println("");
        }
        if(pg->EVCTRL.reg) {
            Serial.print("    EVCTRL: ");
            printReg<32>(pg->EVCTRL.reg);
            Serial.println("");
        }
        for(int j = 0; j < 16; j++) {
            if(pg->PMUX[j].reg) {
                Serial.print("        PMUX");
                PRINTAI(j);
                Serial.print(": ");  
                printReg<8>(pg->PMUX[j].reg);
                Serial.println("");
            }
        }
        for(int j = 0; j < 32; j++) {
            if(pg->PINCFG[j].reg) {
                Serial.print("        PINCFG");
                PRINTAI(j);
                Serial.print(": ");  
                printReg<8>(pg->PINCFG[j].reg);
                Serial.println("");
            }
        }
    }

}

void printRegsEVSYS(RegOptions &opts) {
    Serial.println("--------------------------- EVSYS");
    for(uint8_t i = 0; i < 32; i++) { 
        if(EVSYS->Channel[i].CHANNEL.reg) {
            Serial.print("CHANNEL");
            PRINTAI(i);
            Serial.println(": ");   
            printReg<32>(EVSYS->Channel[i].CHANNEL.reg);
            Serial.println("");
        }
    }
    for(uint8_t i = 0; i <67; i++) {
        if(EVSYS->USER[i].reg) {
            Serial.print("USER");
            PRINTAI(i);
            Serial.print(".CHANNEL: ");   
            printReg<32>(EVSYS->USER[i].reg);
            Serial.println("");
        }
    }
}