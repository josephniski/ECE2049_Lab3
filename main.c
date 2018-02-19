
//Drew Robert and Joe Niski
//ECE 2049
//Lab 3

#include <msp430.h>
#include "peripherals.h"

#define CALADC12_25V_30C *((unsigned int *)0x1A22)
#define CALADC12_25V_85C *((unsigned int *)0x1A24)


//FUNCTIONS
void decimalASCIITime(long unsigned int timeInput);
void decimalASCIIDate(long unsigned int dateInput);
void runtimerA2(void);
void stoptimerA2(int reset);
void adc12_config(void);
void displayTempC(unsigned int inTemp);
void displayTempF(void);

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
unsigned char tempArrayC[5] = {' '};
unsigned char tempArrayF[5] = {' '};
int i = 0, j = 0 , m = 0, n = 0;
int once = 1;
long unsigned int days = 0;
long unsigned int actDays = 0;
unsigned int adc_inTemp = 0;
float tempC = 0;
float tempF = 0;
int dispC = 0;
int dispF = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
                                     // You can then configure it properly, if desired
    initLeds();

    _BIS_SR(GIE);

    configDisplay();
    configKeypad();

    adc12_config();
    runtimerA2();

    // *** Intro Screen ***
    Graphics_clearDisplay(&g_sContext); // Clear the display

    while (1)
    {
        decimalASCIIDate(timer_cnt);
        decimalASCIITime(timer_cnt);
        displayTempC(adc_inTemp);

        Graphics_drawStringCentered(&g_sContext, dateArray, 6, 48, 25,
                                    OPAQUE_TEXT);

        Graphics_drawStringCentered(&g_sContext, timeArray, 8, 48, 35,
                                    OPAQUE_TEXT);

        Graphics_drawStringCentered(&g_sContext, tempArrayC, 5, 48, 55,
                                    OPAQUE_TEXT);

        Graphics_drawStringCentered(&g_sContext, tempArrayF, 5, 48, 65,
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

    ADC12CTL0 |= ADC12SC + ADC12ENC; //start conversion
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

void adc12_config(void){
    // Single channel, single converion (internal temp. sensor)
    // to ADC12MEM1 register
    REFCTL0 &= ~REFMSTR;
    ADC12CTL0 = ADC12SHT0_9 | ADC12REFON | ADC12ON | ADC12REF2_5V;
    ADC12CTL1 = ADC12SHP + ADC12CSTARTADD_1;
    ADC12MCTL1 = ADC12SREF_1 + ADC12INCH_10;
    ADC12IE = BIT1; // using ADC12MEM1 for conversion result
                    // so enable interrupt for MEM1
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR(void)
{
// Interrupt is generated when conversion (or last
// conversion if multi-channel) is complete so just
// read the results
adc_inTemp = ADC12MEM1; // Move results to global
                        // variable adc_inTemp
}

void displayTempC(unsigned int inTemp){

    tempC = (float)(((long)inTemp-CALADC12_25V_30C)*(85 - 30))/(CALADC12_25V_85C - CALADC12_25V_30C) + 30.0;
    dispC = tempC * 10;

    for (m = 4; m >= 0; m--)
    {
        if (m == 4)
        {
            tempArrayC[m] = 'C';
        }
        else if (m == 2)
        {
            tempArrayC[m] = '.';
        }
        else
        {
            tempArrayC[m] = ((dispC % 10) + 0x30);
            dispC = dispC / 10;
        }
    }

    displayTempF();

}

void displayTempF(void){

    tempF = (float)(tempC * (9/5)) + 32;
    dispF = tempF * 10;

    for (n = 4; n >= 0; n--)
    {
        if (n == 4)
        {
            tempArrayF[n] = 'F';
        }
        else if (n == 2)
        {
            tempArrayF[n] = '.';
        }
        else
        {
            tempArrayF[n] = ((dispF % 10) + 0x30);
            dispF = dispF / 10;
        }
    }

}
