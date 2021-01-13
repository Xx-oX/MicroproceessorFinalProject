/* configuration */
#pragma config OSC = INTIO67  // Oscillator Selection bits (Internal oscillator
                              // block, port function on RA6 and RA7)
#pragma config FCMEN = OFF    // Fail-Safe Clock Monitor Enable bit (Fail-Safe
                              // Clock Monitor disabled)
#pragma config IESO = OFF     // Internal/External Oscillator Switchover bit
                              // (Oscillator Switchover mode disabled)

// #pragma config2L
#pragma config PWRT = OFF  // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = \
    SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware
             // only (SBOREN is disabled))
#pragma config BORV = 3  // Brown Out Reset Voltage bits (Minimum setting)

// #pragma config2H
#pragma config WDT = OFF  // Watchdog Timer Enable bit (WDT disabled (control is
                          // placed on the SWDTEN bit))
#pragma config WDTPS = 32768  // Watchdog Timer Postscale Select bits (1:32768)

// #pragma config3H
#pragma config CCP2MX = \
    PORTC  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = \
    OFF  // PORTB A/D Enable bit (PORTB<4:0> pins are #pragma configured as
         // analog input channels on Reset)
#pragma config LPT1OSC = \
    OFF  // Low-Power Timer1 Oscillator Enable bit (Timer1 #pragma configured
         // for higher power operation)
#pragma config MCLRE = \
    ON  // MCLR Pin Enable bit (MCLR pin enabled// RE3 input pin disabled)

// #pragma config4L
#pragma config STVREN = ON  // Stack Full/Underflow Reset Enable bit (Stack
                            // full/underflow will cause Reset)
#pragma config LVP = \
    OFF  // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = \
    OFF  // Extended Instruction Set Enable bit (Instruction set extension and
         // Indexed Addressing mode disabled (Legacy mode))

// #pragma config5L
#pragma config CP0 = \
    OFF  // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = \
    OFF  // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = \
    OFF  // Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = \
    OFF  // Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

// #pragma config5H
#pragma config CPB = OFF  // Boot Block Code Protection bit (Boot block
                          // (000000-0007FFh) not code-protected)
#pragma config CPD = \
    OFF  // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// #pragma config6L
#pragma config WRT0 = OFF  // Write Protection bit (Block 0 (000800-001FFFh) not
                           // write-protected)
#pragma config WRT1 = OFF  // Write Protection bit (Block 1 (002000-003FFFh) not
                           // write-protected)
#pragma config WRT2 = OFF  // Write Protection bit (Block 2 (004000-005FFFh) not
                           // write-protected)
#pragma config WRT3 = OFF  // Write Protection bit (Block 3 (006000-007FFFh) not
                           // write-protected)

// #pragma config6H
#pragma config WRTC = \
    OFF  // Configuration Register Write Protection bit (Configuration registers
         // (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF  // Boot Block Write Protection bit (Boot block
                           // (000000-0007FFh) not write-protected)
#pragma config WRTD = \
    OFF  // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// #pragma config7L
#pragma config EBTR0 = \
    OFF  // Table Read Protection bit (Block 0 (000800-001FFFh) not protected
         // from table reads executed in other blocks)
#pragma config EBTR1 = \
    OFF  // Table Read Protection bit (Block 1 (002000-003FFFh) not protected
         // from table reads executed in other blocks)
#pragma config EBTR2 = \
    OFF  // Table Read Protection bit (Block 2 (004000-005FFFh) not protected
         // from table reads executed in other blocks)
#pragma config EBTR3 = \
    OFF  // Table Read Protection bit (Block 3 (006000-007FFFh) not protected
         // from table reads executed in other blocks)

// #pragma config7H
#pragma config EBTRB = \
    OFF  // Boot Block Table Read Protection bit (Boot block (000000-0007FFh)
         // not protected from table reads executed in other blocks)

/* include */      
# include <xc.h>
# include <proc/pic18f4520.h> 

/* definition */
#define _XTAL_FREQ 8000000
# define RS PORTCbits.RC0
# define EN PORTCbits.RC1
# define FIRST_LINE_START 0x80
# define SECOND_LINE_START 0xc0

/* function declaration */
void init(void);
void delay_us(int);
void LCD_init(void);
void LCD_data(char);           
void LCD_cmd(char);            
void LCD_send(const char, const char*);
void trans(void);

/*global variables*/
int num = 0;
int lenth = 0;
int out = 0;
char str[16];
int pos = 0;

int main(void) {
    init();
    out = 1;
    while(1);  // halt
    
    return 0;
}

void __interrupt(high_priority) Hi_ISR(void)
{
    out = 0;
    __delay_ms(100);
    
    if(PORTB == 1){
        num = (num << 1) | 1;
        LATA = 0b01000000;
    }
    else{
        num = (num << 1) | 0;
        LATA = 0b01000000;
    }
    lenth = lenth + 1;
    __delay_ms(100);
    //LATD = 0;
    LATA = 0;
    INTCONbits.INT0IF = 0;
    //LATD = 1;
    return;
}

void __interrupt(low_priority) Lo_ISR(void) {
    if(out == 1){
        // LCD_cmd(0x01);  // clear
        LCD_init();
        LATA = 0b01000000;
        LCD_send(FIRST_LINE_START, str);
        for(int i=0; i<16; ++i)
            str[i] = '\0';
        pos = 0;
    }else{
        trans();
        out = 1;
        num = 0;
        lenth = 0;
    }
    /*
    if(LATD == 1)
        LATD = 0;
    else
        LATD = 1;
    */
    INTCON3bits.INT1IF = 0;
    __delay_ms(200);
    LATA = 0;
    return;
}

void init(void)
{
    PORTB = 0;
    LATB = 0;
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    TRISA = 0;
    LATA = 0;
    ADCON1 = 0x0F;
    T0CON = 0b11111000;
    
    TMR0L = 0;
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    INTCONbits.INT0IE = 1;
    INTCON3bits.INT1IE = 1;
    INTCONbits.INT0IF = 0;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT1IP = 0;
    
    LCD_init();
}

void delay_us(int value)
{
    value = value >> 1;
    while(value!=1)
        value--;
}

void LCD_init(void)
{
    TRISC = 0x00;  // PORTC as output
    TRISD = 0x00;  // PORTD as output
    delay_us(25);
    LCD_cmd(0x38);  // 2 line, 5x8 dots format
    LCD_cmd(0x06);  // right shift, auto increment
    LCD_cmd(0x0c);  // display on
    LCD_cmd(0x01);  // clear display    
    delay_us(1000);  // delay 1ms
}

void LCD_data(char value)
{
    PORTD = value;  // output data to PORTD
    RS = 1;  // data mode
    EN = 1;  
    delay_us(25);                
    EN = 0;  // latch data
}
          
void LCD_cmd(char value)
{
    PORTD = value;  // output command to PORTD
    RS = 0;  // command mode
    EN = 1;  
    delay_us(25);                
    EN = 0;  // latch command
}
          
void LCD_send(const char addr, const char *str)
{
    LCD_cmd(addr);  // starting address to display string
    while(*str!='\0')
    {    
        LCD_data(*str);  // display char on LCD    
        str++;  // increment the pointer
    }
}

void trans(void){
    if(num == 1 && lenth == 2)
        str[pos]='a';
    else if(num == 8 && lenth == 4)
        str[pos]='b';
    else if(num == 10 && lenth == 4)
        str[pos]='c';
    else if(num == 4 && lenth == 3)
        str[pos]='d';
    else if(num == 0 && lenth == 1)
        str[pos]='e';
    else if(num == 2 && lenth == 4)
        str[pos]='f';
    else if(num == 6 && lenth == 3)
        str[pos]='g';
    else if(num == 0 && lenth == 4)
        str[pos]='h';
    else if(num == 0 && lenth == 2)
        str[pos]='i';
    else if(num == 7 && lenth == 4)
        str[pos]='j';
    else if(num == 5 && lenth == 3)
        str[pos]='k';
    else if(num == 4 && lenth == 4)
        str[pos]='l';
    else if(num == 3 && lenth == 2)
        str[pos]='m';
    else if(num == 2 && lenth == 2)
        str[pos]='n';
    else if(num == 7 && lenth == 3)
        str[pos]='o';
    else if(num == 6 && lenth == 4)
        str[pos]='p';
    else if(num == 13 && lenth == 4)
        str[pos]='q';
    else if(num == 2 && lenth == 3)
        str[pos]='r';
    else if(num == 0 && lenth == 3)
        str[pos]='s';
    else if(num == 1 && lenth == 1)
        str[pos]='t';
    else if(num == 1 && lenth == 3)
        str[pos]='u';
    else if(num == 1 && lenth == 4)
        str[pos]='v';
    else if(num == 3 && lenth == 3)
        str[pos]='w';
    else if(num == 9 && lenth == 4)
        str[pos]='x';
    else if(num == 11 && lenth == 4)
        str[pos]='y';
    else if(num == 12 && lenth == 4)
        str[pos]='z';
    else if(num == 15 && lenth == 5)
        str[pos]='1';
    else if(num == 7 && lenth == 5)
        str[pos]='2';
    else if(num == 3 && lenth == 5)
        str[pos]='3';
    else if(num == 1 && lenth == 5)
        str[pos]='4';
    else if(num == 0 && lenth == 5)
        str[pos]='0';
    else if(num == 16 && lenth == 5)
        str[pos]='6';
    else if(num ==24 && lenth == 5)
        str[pos]='7';
    else if(num == 28 && lenth == 5)
        str[pos]='8';
    else if(num == 30 && lenth == 5)
        str[pos]='9';
    else if(num == 31 && lenth == 5)
        str[pos]='0';
    else
        str[pos]='E';
    pos++;
}
