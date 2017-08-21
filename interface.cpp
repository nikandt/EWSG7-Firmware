#include "mbed.h"
#include "interface.h"
#include <string>

/***********************
    PROBE SPECIFIC
************************/
// TODO: use #defines instead of port numbers at "config.h"

// Probe leds
DigitalOut probeRsLed[] = {(PC_3),(PC_7),(PC_11),(PC_15)};
DigitalOut probePwrLed[] = {(PC_2),(PC_6),(PC_10),(PC_14)};

// Control outputs
DigitalOut probeRsCtrl[] = {(PA_7),(PA_9),(PB_1),(PB_3)};
DigitalOut probePwrCtrl[] = {(PA_6),(PA_8),(PB_0),(PB_2)};

// Probe-specific interrupt switches
InterruptIn probePwrBtn[] = {(PC_0),(PC_4),(PC_8),(PC_12)};
InterruptIn probeRsBtn[] = {(PC_1),(PC_5),(PC_9),(PC_13)};

// Master pwr and bus buttons
InterruptIn probePwrBtnMaster = PA_4;
InterruptIn probeRsBtnMaster = PA_5;

extern string buff;

void pwr_switch_verbose( uint8_t i, uint8_t state )
{
    char ibuf[8];
    sprintf(ibuf, "%d", i);
    
    switch(state)
    {
        case 0:
            buff += "Module ";
            buff += ibuf;
            buff += " powered on\n";
            break;
        default:
            buff += "Module ";
            buff += ibuf;
            buff += " powered off\n"; 
            break;
    }
    return;
}

void rs_switch_verbose( uint8_t i, uint8_t state )
{
    char ibuf[8];
    sprintf(ibuf, "%d", i);
    
    switch(state)
    {
        case 0:
            buff += "Module ";
            buff += ibuf;
            buff += " switched into bus\n";
            break;
        default:
            buff += "Module ";
            buff += ibuf;
            buff += " switched off bus\n"; 
            break;
    }
    return;
}

void toggle_pwr_1(void)
{
    uint8_t i = 0;
    probePwrCtrl[i] = !probePwrCtrl[i]; // Toggle power
    probePwrLed[i] = !probePwrLed[i]; // Toggle LED. TODO: active-low?

    // Be verbose
    pwr_switch_verbose(i, probePwrCtrl[i]);
}

void toggle_pwr_2(void)
{
    uint8_t i = 1;
    probePwrCtrl[i] = !probePwrCtrl[i]; // Toggle to power
    probePwrLed[i] = !probePwrLed[i];
    
    // Be verbose
    pwr_switch_verbose(i, probePwrCtrl[i]);
}

void toggle_pwr_3(void)
{
    uint8_t i = 2;
    probePwrCtrl[i] = !probePwrCtrl[i]; // Toggle to power
    probePwrLed[i] = !probePwrLed[i];
    pwr_switch_verbose(i, probePwrCtrl[i]);
}

void toggle_pwr_4(void)
{
    uint8_t i = 3;
    probePwrCtrl[i] = !probePwrCtrl[i]; // Toggle to power
    probePwrLed[i] = !probePwrLed[i];
    pwr_switch_verbose(i, probePwrCtrl[i]);
}

void toggle_rs_1(void)
{
    int i = 0;
    probeRsCtrl[i] = !probeRsCtrl[i]; // Toggle to bus
    probeRsLed[i] = !probeRsLed[i];
    rs_switch_verbose(i, probeRsCtrl[i]);
}

void toggle_rs_2(void)
{
    int i = 1;
    probeRsCtrl[i] = !probeRsCtrl[i]; // Toggle to bus
    probeRsLed[i] = !probeRsLed[i];
    rs_switch_verbose(i, probeRsCtrl[i]);
}
void toggle_rs_3(void)
{
    int i = 2;
    probeRsCtrl[i] = !probeRsCtrl[i]; // Toggle to bus
    probeRsLed[i] = !probeRsLed[i];
    rs_switch_verbose(i, probeRsCtrl[i]);
}
void toggle_rs_4(void)
{
    int i = 3;
    probeRsCtrl[i] = !probeRsCtrl[i]; // Toggle to bus
    probeRsLed[i] = !probeRsLed[i];
    rs_switch_verbose(i, probeRsCtrl[i]);
}

void (*pwr_toggle[4])(void) = { toggle_pwr_1, toggle_pwr_2, toggle_pwr_3, toggle_pwr_4 };
void (*rs_toggle[4])(void) = { toggle_rs_1, toggle_rs_2, toggle_rs_3, toggle_rs_4 };

void toggle_pwr_master() // Power all either on or off
{
    int i = 0; // Logic: if even one of them is off, switch all on.
    uint8_t state = 0;
    
    for(i=0;i<4;i++)
    {
        if(probePwrCtrl[i] == 0)
        {
            state = 1;
            break;
         }
    }
    
    for(i=0;i<4;i++)
    {
        probePwrCtrl[i] = state; //!probePwrCtrl[i]; // Toggle
        probePwrLed[i] = !state; //!probePwrLed[i];
    }
    buff += "All modules toggled on bus. State: ";
    char stateBuf[8];
    sprintf(stateBuf, "%d", state);
    buff += stateBuf;
    buff += "\n";
}

void toggle_rs_master()
{
    int i = 0; // TODO: Logic: if even one of them is off, switch all on.
    uint8_t state = 0;
    for(i=0;i<4;i++)
    {
        if(probeRsCtrl[i] == 0)
        {
            state = 1;
            break;
         }
    }
    
    for(i=0;i<4;i++)
    {
        probeRsCtrl[i] = state; //!probeRsCtrl[i]; // Toggle
        probeRsLed[i] = !state; //!probeRsLed[i];
    }
    buff += "All modules toggled on bus. State: ";
    char stateBuf[8];
    sprintf(stateBuf, "%d", state);
    buff += stateBuf;
    buff += "\n";
}

int detect_probes(void)
{
    int i = 0;
    char indexBuf[8];
    // Detect if probes are on / on the bus, and give out info
    for(i=0;i<4;i++)
    {
        if( probeRsCtrl[i] == 1 ) // In bus
        {
            // write to buff
            buff += "Probe %d ";
            sprintf(indexBuf, "%d", i+1);
            buff += indexBuf;
            buff += " in bus.\n";
        }
        
        if( probePwrCtrl[i] == 1 ) // In pwr line
        {
            // write to buff
            buff += "Probe %d ";
            sprintf(indexBuf, "%d", i+1);
            buff += indexBuf;
            buff += " in power line.\n";
        }
    }
    
    return 0;
}

int init_interface(void)
{
    int i = 0;   
    // Naive function pointer event handlers; debug beforehand
    for(i=0;i<4;i++)
    {
        probePwrBtn[i].fall(&(*pwr_toggle[i]));
        probeRsBtn[i].fall(&(*rs_toggle[i]));
        
        // TODO: check
        probeRsLed[i] = 1; // Active-low;
        probePwrLed[i] = 1;
    }
    
    probePwrBtnMaster.fall(&toggle_pwr_master);
    probeRsBtnMaster.fall(&toggle_rs_master);
    
    buff += "PT module interface initialized.\n";
    
    return 0;
}