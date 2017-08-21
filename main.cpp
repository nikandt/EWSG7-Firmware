#include "mbed.h"
#include <string>
#include "BufferedSerial.h"
#include "interface.h"

/* This module should programmatically be configured
either as "master" or as "slave", by commenting in the right line below. */
#define PT_MODULE_MASTER 1
//#define PT_MODULE_SLAVE 1

/*****************************************************************
Slave tasks:
* Listen local events from buttons, forward to intermodule control bus
* Forward any traffic to intermodule control bus
* Forward information of probes to intermodule control bus
Master tasks:
* Same but only listens the intermodule bus and forwards to PC
******************************************************************/

/*********************
        UART
    Half-duplex only
**********************/
//------------------------------------
// Hyperterminal configuration
// 9600 bauds, 8-bit data, no parity
//------------------------------------

std::string buff; // Intermodule UART buffer
std::string cmdBuff;

BufferedSerial pc(USBTX, USBRX);
// Intermodule control bus: TODO: test baud rate
BufferedSerial rs485control(PA_2, PA_3, 115200);
// Bus for sensing the probes, TODO: test baud rate
BufferedSerial rs485sense(PB_9, PB_8, 115200);

/*******************
    MODULE SPECIFIC
********************/

// A simple indicator LED
//DigitalOut led(LED1);

// Bus termination
DigitalOut termination = PB_15;

/****************
    UTILITIES
*****************/

// Probe inputs are in toggleProbes.cpp

#ifdef PT_MODULE_MASTER
void read_commands(void)
{
    // Read possible commands from PC
    if(pc.readable())
        cmdBuff = pc.getc();
    
    while( pc.getc() )
    {
        std::size_t detect = cmdBuff.find("detect"); // TODO: expand.
        if(detect)
            detect_probes();
    }
}

void intermodule_traffic()
{
    read_commands();
    
    if( buff.c_str() )
    {
        pc.printf( buff.c_str() );
        buff.clear(); // Flush buff
    }
}

#elif PT_MODULE_SLAVE
void intermodule_traffic()
{
    if( buff.c_str() )
    {
        rs485control.printf( buff.c_str() );
        buff.clear(); // Flush buff
    }
}
#endif

int main()
{
    pc.baud(115200);
    init_interface();
   
     while(1)
     {
         intermodule_traffic();
     }
}
