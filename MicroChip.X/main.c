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
# define FIRST_LINE_START 0x80
# define SECOND_LINE_START 0xc0
# define PWM_VAL 50

/* port connection */
# define RS PORTCbits.RC0
# define EN PORTCbits.RC1
# define CCP1 LATCbits.LATC2
# define LED_NOTIFY LATAbits.LA6
# define LED_A LATAbits.LA0
# define LED_B LATAbits.LA1
# define LED_C LATAbits.LA2
# define LED_D LATAbits.LA3

/* function definitions */
void init(void);
void pwm(void);
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
int PWM_light = 0;

/* funtion declarations */
int main(void) {
    init();
    pwm();
    out = 1;
    
    while(1){
        if(PWM_light == 1){
            // breath led
            for(int d = 1; d < PWM_VAL; d++){
                if(PWM_light != 1){
                    break;
                }   
                CCPR1L ++;
                __delay_ms(5);
            }
            __delay_ms(100);
            for(int d = 1; d < PWM_VAL; d++){
                if(PWM_light != 1){
                    break;
                }
                CCPR1L --;
                __delay_ms(5);
            }
            __delay_ms(100);
        }
    }
     
    return 0;
}

void __interrupt(high_priority) Hi_ISR(void)
{
    CCPR1L = 0;
    PWM_light = 0;
    out = 0;
    __delay_ms(200);
    
    if(PORTB == 1){
        num = (num << 1) | 1;
        LED_NOTIFY = 1;
    }
    else{
        num = (num << 1) | 0;
        LED_NOTIFY = 1;
    }
    lenth = lenth + 1;
    __delay_ms(200);
    LED_NOTIFY = 0;
    INTCONbits.INT0IF = 0;
    return;
}

void __interrupt(low_priority) Lo_ISR(void) {
    CCPR1L = 0;
    PWM_light = 0;
    if(out == 1){
        LCD_init();
        LED_NOTIFY = 1;
        LCD_send(FIRST_LINE_START, str);

        // led commands
        if(str[0] == 'b' && str[1] == 'r'){
            PWM_light = 1;
        }

        if(str[0] == 'l' && str[1] == 'a'){
            LED_A = 1;
        }

        if(str[0] == 'l' && str[1] == 'b'){
            LED_B = 1;
        }

        if(str[0] == 'l' && str[1] == 'c'){
            LED_C = 1;
        }

        if(str[0] == 'l' && str[1] == 'd'){
            LED_D = 1;
        }
        
        if(str[0] == 'e' && str[1] == 'n' && str[2] == 'd'){
            LED_A = 0;
            LED_B = 0;
            LED_C = 0;
            LED_D = 0;
        }
        
        // clear buffer
        for(int i=0; i<16; ++i)
            str[i] = '\0';
        pos = 0;

    }else{
        trans();
        out = 1;
        num = 0;
        lenth = 0;
        LED_NOTIFY = 1;
    }
    INTCON3bits.INT1IF = 0;
    __delay_ms(500);
    LED_NOTIFY = 0;
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

void pwm(void)
{
    TRISC = 0x00;
    CCP1 = 0;
    T2CON = 0b01111101;  // prescaler=16
    CCP1CONbits.CCP1M = 0b1100;  // pwm mode    
    PR2 = PWM_VAL;
    CCPR1L = 0x00;
    CCP1CONbits.DC1B = 0b00;
}

void delay_us(int value)
{
    // delay funtion for LCD control
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
