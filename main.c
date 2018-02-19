//Drew Robert and Joe Niski
//ECE 2049
//Lab 3

#include <msp430.h>
#include "peripherals.h"

// Temperature Sensor Calibration = Reading at 30 degrees C is stored at addr 1A1Ah
// See end of datasheet for TLV table memory mapping
#define CALADC12_15V_30C  *((unsigned int *)0x1A1A)
// Temperature Sensor Calibration = Reading at 85 degrees C is stored at addr 1A1Ch                                            //See device datasheet for TLV table memory mapping
#define CALADC12_15V_85C  *((unsigned int *)0x1A1C)

//FUNCTIONS
void decimalASCIITime(long unsigned int timeInput);
void decimalASCIIDate(long unsigned int dateInput);
void runtimerA2(void);
void stoptimerA2(int reset);
unsigned int potValue(void);
void printPotVal(unsigned int gal);
void configPot(void);

//GLOBAL VARIABLES
unsigned int in_temp;
long unsigned int timer_cnt = 15552000; //June 29th 00:00:00
unsigned char timeArray[8] = {' '};
unsigned char dateArray[6] = {' '};
unsigned char secArray[2] = {' '};
unsigned char minArray[2] = {' '};
unsigned char hourArray[2] = {' '};
unsigned char monthArray[3] = {' '};
unsigned char daysArray[2] = {' '};
int i = 0, j = 0;
int once = 1;
long unsigned int days = 0;
long unsigned int actDays = 0;
unsigned char potArray[5] = {' '};
unsigned int pot = 0;
unsigned int potVal = 0;


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
                                     // You can then configure it properly, if desired
    initLeds();

    _BIS_SR(GIE);

    configDisplay();
    configKeypad();

    runtimerA2();

    configPot();
    // *** Intro Screen ***
    Graphics_clearDisplay(&g_sContext); // Clear the display

    while (1)
    {
        pot = potValue();
        printPotVal(pot);
        decimalASCIIDate(timer_cnt);
        decimalASCIITime(timer_cnt);

        // Write some text to the display
        Graphics_drawStringCentered(&g_sContext, dateArray, 6, 48, 35,
                                    OPAQUE_TEXT);

        // Write some text to the display
        Graphics_drawStringCentered(&g_sContext, timeArray, 8, 48, 45,
                                    OPAQUE_TEXT);

        Graphics_drawStringCentered(&g_sContext, potArray, 5, 48, 55,
                                    OPAQUE_TEXT);

        // Update display
        Graphics_flushBuffer(&g_sContext);
    }

}

void runtimerA2(void)
{
// This function configures and starts Timer A2
// Timer is counting ~1 seconds
//
// Input: none, Output: none
//
// smj, ECE2049, 17 Sep 2013
//
// Use ACLK, 16 Bit, up mode, 1 divider
    TA2CTL = TASSEL_1 + MC_1 + ID_0;
    TA2CCR0 = 32767; // 32767+1 = 32768 ACLK tics = ~1 seconds
    TA2CCTL0 = CCIE; // TA2CCR0 interrupt enabled
}

void stoptimerA2(int reset)
{
// This function stops Timer A2 andresets the global time variable
// if input reset = 1
//
// Input: reset, Output: none
//
// smj, ECE2049, 17 Sep 2013
//
    TA2CTL = MC_0; // stop timer
    TA2CCTL0 &= ~CCIE; // TA2CCR0 interrupt disabled
    if (reset)
        timer_cnt = 0;
}

// Timer A2 interrupt service routine
#pragma vector=TIMER2_A0_VECTOR
__interrupt void TimerA2_ISR(void)
{
    timer_cnt++;
    //ADC12CTL0 |= ADC12SC + ADC12ENC;
}

void decimalASCIITime(long unsigned int timeInput){

    long unsigned int date = 0;
    long unsigned int hour = 0;
    long unsigned int min = 0;
    long unsigned int sec = 0;

    date = date + timeInput/(60L*60*24);
    hour = (timeInput - date*60*60*24)/(60*60);
    min = (timeInput - date*60*60*24 - hour*60*60)/(60);
    sec = (timeInput - date*60*60*24 - hour*60*60 - min*60);

    for (i = 1; i >= 0; i--){
        secArray[i] = ((sec % 10) + 0x30);
        sec = sec / 10;
    }

    for (i = 1; i >= 0; i--){
        minArray[i] = ((min % 10) + 0x30);
        min = min / 10;
    }

    for (i = 1; i >= 0; i--){
        hourArray[i] = ((hour % 10) + 0x30);
        hour = hour / 10;
    }

    timeArray[0] = hourArray[0];
    timeArray[1] = hourArray[1];
    timeArray[2] = ':';
    timeArray[3] = minArray[0];
    timeArray[4] = minArray[1];
    timeArray[5] = ':';
    timeArray[6] = secArray[0];
    timeArray[7] = secArray[1];

}

void decimalASCIIDate(long unsigned int dateInput){

    days = ((dateInput/(60L*60*24))%365);

    if (days > 0 && days <= 31){
        monthArray[0] = 'J';
        monthArray[1] = 'A';
        monthArray[2] = 'N';
        actDays = days;
        for (j = 1; j >= 0; j--){
            daysArray[j] = ((actDays % 10) + 0x30);
            actDays = actDays / 10;
        }
    }
    else if (days > 31 && days <= 59){
        monthArray[0] = 'F';
        monthArray[1] = 'E';
        monthArray[2] = 'B';
        actDays = days - 31;
        for (j = 1; j >= 0; j--){
            daysArray[j] = ((actDays % 10) + 0x30);
            actDays = actDays / 10;
        }
    }
    else if (days > 59 && days <= 90){
        monthArray[0] = 'M';
        monthArray[1] = 'A';
        monthArray[2] = 'R';
        actDays = days - 59;
        for (j = 1; j >= 0; j--){
            daysArray[j] = ((actDays % 10) + 0x30);
            actDays = actDays / 10;
        }
    }
    else if (days > 90 && days <= 120){
        monthArray[0] = 'A';
        monthArray[1] = 'P';
        monthArray[2] = 'R';
        actDays = days - 90;
        for (j = 1; j >= 0; j--){
            daysArray[j] = ((actDays % 10) + 0x30);
            actDays = actDays / 10;
        }
    }
    else if (days > 120 && days <= 151){
        monthArray[0] = 'M';
        monthArray[1] = 'A';
        monthArray[2] = 'Y';
        actDays = days - 120;
        for (j = 1; j >= 0; j--){
            daysArray[j] = ((actDays % 10) + 0x30);
            actDays = actDays / 10;
        }
    }
    else if (days > 151 && days <= 181){
        monthArray[0] = 'J';
        monthArray[1] = 'U';
        monthArray[2] = 'N';
        actDays = days - 151;
        for (j = 1; j >= 0; j--){
            daysArray[j] = ((actDays % 10) + 0x30);
            actDays = actDays / 10;
        }
    }
    else if (days > 181 && days <= 212){
        monthArray[0] = 'J';
        monthArray[1] = 'U';
        monthArray[2] = 'L';
        actDays = days - 181;
        for (j = 1; j >= 0; j--){
            daysArray[j] = ((actDays % 10) + 0x30);
            actDays = actDays / 10;
        }
    }
    else if (days > 212 && days <= 243){
        monthArray[0] = 'A';
        monthArray[1] = 'U';
        monthArray[2] = 'G';
        actDays = days - 212;
        for (j = 1; j >= 0; j--){
            daysArray[j] = ((actDays % 10) + 0x30);
            actDays = actDays / 10;
        }
    }
    else if (days > 243 && days <= 273){
        monthArray[0] = 'S';
        monthArray[1] = 'E';
        monthArray[2] = 'P';
        actDays = days - 243;
        for (j = 1; j >= 0; j--){
            daysArray[j] = ((actDays % 10) + 0x30);
            actDays = actDays / 10;
        }
    }
    else if (days > 273 && days <= 304){
        monthArray[0] = 'O';
        monthArray[1] = 'C';
        monthArray[2] = 'T';
        actDays = days - 273;
        for (j = 1; j >= 0; j--){
            daysArray[j] = ((actDays % 10) + 0x30);
            actDays = actDays / 10;
        }
    }
    else if (days > 304 && days <= 334){
        monthArray[0] = 'N';
        monthArray[1] = 'O';
        monthArray[2] = 'V';
        actDays = days - 304;
        for (j = 1; j >= 0; j--){
            daysArray[j] = ((actDays % 10) + 0x30);
            actDays = actDays / 10;
        }
    }
    else if (days > 334 && days <= 365){
        monthArray[0] = 'D';
        monthArray[1] = 'E';
        monthArray[2] = 'C';
        actDays = days - 334;
        for (j = 1; j >= 0; j--){
            daysArray[j] = ((actDays % 10) + 0x30);
            actDays = actDays / 10;
        }
    }

    dateArray[0] = monthArray[0];
    dateArray[1] = monthArray[1];
    dateArray[2] = monthArray[2];
    dateArray[3] = ' ';
    dateArray[4] = daysArray[0];
    dateArray[5] = daysArray[1];

}

void configPot(void)
{
    //unsigned int potVal;
    REFCTL0 &= ~REFMSTR;
    ADC12CTL0 = ADC12SHT0_9 | ADC12ON;
    ADC12CTL1 = ADC12SHP;
    ADC12MCTL0 = ADC12SREF_0 + ADC12INCH_0;
    P6SEL |= BIT0;
    ADC12CTL0 &= ~ADC12SC;
    ADC12CTL0 = ADC12SC + ADC12ENC;
}

unsigned int potValue(void)
{
    /*while(ADC12CTL1 & ADC12BUSY)
    {
        __no_operation();
    }*/

    potVal = ADC12MEM0 & 0x0FFF;
    return potVal;
}

void printPotVal(unsigned int gal)
{
    for (j = 5; j >= 0; j--)
    {
        potArray[j] = ((gal % 10) + 0x30);
        gal = gal / 10;
    }
}

