/*
 * File:   main.c
 *
 * Created on 2021年1月13日, 上午 9:58
 */

/* configuration */
#pragma config OSC = INTIO67
#pragma config FCMEN = OFF 
#pragma config IESO = OFF
// #pragma config2L
#pragma config PWRT = OFF
#pragma config BOREN = SBORDIS
#pragma config BORV = 3
// #pragma config2H
#pragma config WDT = OFF
#pragma config WDTPS = 32768
// #pragma config3H
#pragma config CCP2MX = PORTC
#pragma config PBADEN = OFF
#pragma config LPT1OSC = OFF
#pragma config MCLRE = ON
// #pragma config4L
#pragma config STVREN = ON
#pragma config LVP = OFF
#pragma config XINST = OFF
// #pragma config5L
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CP2 = OFF
#pragma config CP3 = OFF
// #pragma config5H
#pragma config CPB = OFF
#pragma config CPD = OFF
// #pragma config6L
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF
// #pragma config6H
#pragma config WRTC = OFF
#pragma config WRTB = OFF
#pragma config WRTD = OFF
// #pragma config7L
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF
#pragma config EBTR3 = OFF 
// #pragma config7H
#pragma config EBTRB = OFF


/* includes */      
# include <xc.h>
# include <proc/pic18f4520.h> 

/* definitions */
# define _XTAL_FREQ 8000000

# define CCP1 LATCbits.LATC2

void PWM_init(void)
{
    TRISC = 0x00;
    CCP1 = 0;
    // 設定TIMER2, prescaler為4
    T2CON = 0b01111101;

    // 設定OSC頻率，Fosc為125k = Tosc為8µs
    OSCCONbits.IRCF = 0b001;
    
    // 設定CCP1
    CCP1CONbits.CCP1M = 0b1100;
    
    // 設定pr2和ccp的數字來調整輸出的PWM週期和Duty Cycle
    // period = (PR2+1)*4*Tosc*(TMR2 prescaler) = (0x9b + 1) * 4 * 8µs * 4 = 0.019968s ~= 20ms
    PR2 = 0x9b;
    
    // 500µs ~= 8µs * 4 * (0x04 * 4 + 0)
    CCPR1L = 0x04;
    CCP1CONbits.DC1B = 0b00;
}