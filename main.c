
//Drew Robert and Joe Niski
//ECE 2049
//Lab 3

#include <msp430.h>
#include "peripherals.h"
#include "math.h"

//#define CALADC12_25V_30C *((unsigned int *)0x1A22)
//#define CALADC12_25V_85C *((unsigned int *)0x1A24)

#define CALADC12_15V_30C  *((unsigned int *)0x1A1A)
// Temperature Sensor Calibration = Reading at 85 degrees C is stored at addr 1A1Ch
//See device datasheet for TLV table memory mapping
#define CALADC12_15V_85C  *((unsigned int *)0x1A1C)


//FUNCTIONS
void swDelay(char numLoops);
void decimalASCIITime(long unsigned int timeInput);
void decimalASCIIDate(long unsigned int dateInput);
void updateMonth(long unsigned int dateInput);
void updateDay(long unsigned int dateInput, unsigned int month);
void updateHour(long unsigned int timeInput);
void updateMin(long unsigned int timeInput);
void updateSec(long unsigned int timeInput);
void runtimerA2(void);
void stoptimerA2(int reset);
void adc12_config(void);
void displayTempC(unsigned int inTemp);
void displayTempF(void);
void averageTemp(unsigned int inTemp);
void configLaunchButtons();
char launchpadButtonStates();
unsigned int potValue(void);
void printPotVal(unsigned int gal);


//GLOBAL VARIABLES
unsigned int in_temp;
int state = 0;
long unsigned int timer_cnt = 15552000; //June 29th 00:00:00
long unsigned int leap_cnt = 0;
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
float tempCel = 0;
float tempF = 0;
int dispC = 0;
int dispF = 0;
unsigned char tenSecReadingsC[10] = {' '};
unsigned char tenSecReadingsF[10] = {' '};
float averageC = 0;
float averageF = 0;
unsigned char times[60] = {' '};
unsigned char tempC[5] = {' '};
char pressed2 = 0x00;
unsigned char potArray[5] = {' '};
unsigned int pot = 0;
unsigned int potVal = 0;
unsigned int updateValue = 0;
unsigned int lastVal = 0;
unsigned int updatedDay, monthLen, updatedHour, updatedMin, updatedSec;
long unsigned int outputVal = 0;

int main(void)
 {
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
                                     // You can then configure it properly, if desired
    initLeds();

    _BIS_SR(GIE);

    configDisplay();
    configKeypad();
    configLaunchButtons();

    adc12_config();


    // *** Intro Screen ***
    Graphics_clearDisplay(&g_sContext); // Clear the display

    while (1)
    {
        displayTempC(adc_inTemp);

        switch (state){
        case 0:  // display mode
            runtimerA2();

            adc12_config();

            displayTempC(adc_inTemp);

            decimalASCIIDate(timer_cnt);
            decimalASCIITime(timer_cnt);


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

            if (pressed2 == 0x01){
                //stoptimerA2(0);
                state = 1;
            }

            break;

        case 1: //Edit MONTH
            displayTempC(adc_inTemp);

            potVal = potValue();

            timer_cnt = (potVal*(60L*60*24))/12;

            updateMonth(timer_cnt);

            Graphics_drawStringCentered(&g_sContext, dateArray, 6, 48, 25,
                                        OPAQUE_TEXT);

            Graphics_drawStringCentered(&g_sContext, tempArrayC, 5, 48, 55,
                                        OPAQUE_TEXT);

            Graphics_drawStringCentered(&g_sContext, tempArrayF, 5, 48, 65,
                                        OPAQUE_TEXT);
            // Update display
            Graphics_flushBuffer(&g_sContext);

            if (pressed2 == 0x01){
                timer_cnt = outputVal;
                state = 2;
            }

            else if (pressed2 == 0x04){
                state = 0; // go back to display mode
            }

            break;

        case 2: //Edit DAY
            displayTempC(adc_inTemp);

            potVal = potValue();

            updatedDay = potVal/131;

            updateDay(updatedDay, (monthLen+1));

            Graphics_drawStringCentered(&g_sContext, dateArray, 6, 48, 25,
            OPAQUE_TEXT);

            Graphics_drawStringCentered(&g_sContext, tempArrayC, 5, 48, 55,
                                        OPAQUE_TEXT);

            Graphics_drawStringCentered(&g_sContext, tempArrayF, 5, 48, 65,
                                        OPAQUE_TEXT);
            // Update display
            Graphics_flushBuffer(&g_sContext);

            if (pressed2 == 0x01){
                timer_cnt = timer_cnt + outputVal;
                state = 3;
            }

            else if (pressed2 == 0x04){
                timer_cnt = timer_cnt + outputVal;
                 state = 0; // go back to display mode
            }

            break;

        case 3: //Edit HOURS
           displayTempC(adc_inTemp);

           potVal = potValue();

           updatedHour = potVal/164;

           updateHour(updatedHour);

           Graphics_drawStringCentered(&g_sContext, timeArray, 8, 48, 35,
                                       OPAQUE_TEXT);

           Graphics_drawStringCentered(&g_sContext, tempArrayC, 5, 48, 55,
                                       OPAQUE_TEXT);

           Graphics_drawStringCentered(&g_sContext, tempArrayF, 5, 48, 65,
                                       OPAQUE_TEXT);
           // Update display
           Graphics_flushBuffer(&g_sContext);

            if (pressed2 == 0x01){
                timer_cnt = timer_cnt + outputVal;
                state = 4;
            }

            else if (pressed2 == 0x04){
                timer_cnt = timer_cnt + outputVal;
                 state = 0; // go back to display mode
            }

            break;

       case 4: //Edit MINUTES
          displayTempC(adc_inTemp);

          potVal = potValue();

          updatedMin = potVal/68;

          updateMin(updatedMin);

          Graphics_drawStringCentered(&g_sContext, timeArray, 8, 48, 35,
                                      OPAQUE_TEXT);

          Graphics_drawStringCentered(&g_sContext, tempArrayC, 5, 48, 55,
                                      OPAQUE_TEXT);

          Graphics_drawStringCentered(&g_sContext, tempArrayF, 5, 48, 65,
                                      OPAQUE_TEXT);
          // Update display
          Graphics_flushBuffer(&g_sContext);

            if (pressed2 == 0x01){
                timer_cnt = timer_cnt + outputVal;
                state = 5;
            }

            else if (pressed2 == 0x04){
                timer_cnt = timer_cnt + outputVal;
                 state = 0; // go back to display mode
            }

            break;

        case 5: //Edit SECONDS
           displayTempC(adc_inTemp);

           potVal = potValue();

           updatedSec = potVal/68;

           updateSec(updatedSec);

           Graphics_drawStringCentered(&g_sContext, timeArray, 8, 48, 35,
                                       OPAQUE_TEXT);

           Graphics_drawStringCentered(&g_sContext, tempArrayC, 5, 48, 55,
                                       OPAQUE_TEXT);

           Graphics_drawStringCentered(&g_sContext, tempArrayF, 5, 48, 65,
                                       OPAQUE_TEXT);
           // Update display
           Graphics_flushBuffer(&g_sContext);

            if (pressed2 == 0x01){
                timer_cnt = timer_cnt + outputVal;
                state = 1;
            }

            else if (pressed2 == 0x04){
                timer_cnt = timer_cnt + outputVal;
                 state = 0; // go back to display mode
             }

            break;
        }
    }
}

void runtimerA2(void)
{
// This function configures and starts Timer A2
// Timer is counting ~1/1000 seconds
//
// Input: none, Output: none
//
// smj, ECE2049, 17 Sep 2013
//
// Use ACLK, 16 Bit, up mode, 1 divider
    TA2CTL = TASSEL_1 + MC_1 + ID_0;
    TA2CCR0 = 32; // 32+1 = 33 ACLK tics = ~1/1000 seconds
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
    pressed2 = launchpadButtonStates(); //determine when the button is pressed

    leap_cnt++;
    if(leap_cnt == 1000)
    {
        if(state == 0){
        timer_cnt++;
        }
        leap_cnt = 0;
    }

    ADC12CTL0 |= ADC12SC + ADC12ENC; //start conversion
}

void decimalASCIITime(long unsigned int timeInput){

    long unsigned int date = 0;
    long unsigned int hour = 0;
    long unsigned int min = 0;
    long unsigned int sec = 0;

    date = date + timeInput/(60L*60*24);
    hour = (timeInput - date*60*60*24)/(60*60); //hour = time input - date * 24
    min = (timeInput - date*60*60*24 - hour*60*60)/(60); //minutes = time input - date * 24 * 60
    sec = (timeInput - date*60*60*24 - hour*60*60 - min*60); //seconds = time input - date * 24 * 60 * 60

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

void updateHour(long unsigned int timeInput)
{
    long unsigned int hour = 0;
    hour = timeInput % 24;
    outputVal = hour * 60L * 60;

    for (i = 1; i >= 0; i--){
        hourArray[i] = ((hour % 10) + 0x30);
        hour = hour / 10;
    }

    timeArray[0] = hourArray[0];
    timeArray[1] = hourArray[1];
}

void updateMin(long unsigned int timeInput)
{
    long unsigned int minute = 0;
    minute = timeInput % 60;
    outputVal = minute * 60L;

    for (i = 1; i >= 0; i--){
        minArray[i] = ((minute % 10) + 0x30);
        minute = minute / 10;
    }

    timeArray[3] = minArray[0];
    timeArray[4] = minArray[1];
}
void updateSec(long unsigned int timeInput)
{
    long unsigned int second = 0;
    second = timeInput % 60;
    outputVal = second;

    for (i = 1; i >= 0; i--){
        secArray[i] = ((second % 10) + 0x30);
        second = second / 10;
    }

    timeArray[6] = secArray[0];
    timeArray[7] = secArray[1];
}



void updateMonth(long unsigned int dateInput){

    days = ((dateInput/(60L*60*24))%365);

    if (days > 0 && days <= 31){
        monthArray[0] = 'J';
        monthArray[1] = 'A';
        monthArray[2] = 'N';
        actDays = days;
        monthLen = 31;
    }
    else if (days > 31 && days <= 59){
        monthArray[0] = 'F';
        monthArray[1] = 'E';
        monthArray[2] = 'B';
        actDays = days - 31;
        monthLen = 28;
    }
    else if (days > 59 && days <= 90){
        monthArray[0] = 'M';
        monthArray[1] = 'A';
        monthArray[2] = 'R';
        actDays = days - 59;
        monthLen = 31;
    }
    else if (days > 90 && days <= 120){
        monthArray[0] = 'A';
        monthArray[1] = 'P';
        monthArray[2] = 'R';
        actDays = days - 90;
        monthLen = 30;
    }
    else if (days > 120 && days <= 151){
        monthArray[0] = 'M';
        monthArray[1] = 'A';
        monthArray[2] = 'Y';
        actDays = days - 120;
        monthLen = 31;
    }
    else if (days > 151 && days <= 181){
        monthArray[0] = 'J';
        monthArray[1] = 'U';
        monthArray[2] = 'N';
        actDays = days - 151;
        monthLen = 30;
    }
    else if (days > 181 && days <= 212){
        monthArray[0] = 'J';
        monthArray[1] = 'U';
        monthArray[2] = 'L';
        actDays = days - 181;
        monthLen = 31;
    }
    else if (days > 212 && days <= 243){
        monthArray[0] = 'A';
        monthArray[1] = 'U';
        monthArray[2] = 'G';
        actDays = days - 212;
        monthLen = 31;
    }
    else if (days > 243 && days <= 273){
        monthArray[0] = 'S';
        monthArray[1] = 'E';
        monthArray[2] = 'P';
        actDays = days - 243;
        monthLen = 30;
    }
    else if (days > 273 && days <= 304){
        monthArray[0] = 'O';
        monthArray[1] = 'C';
        monthArray[2] = 'T';
        actDays = days - 273;
        monthLen = 31;
    }
    else if (days > 304 && days <= 334){
        monthArray[0] = 'N';
        monthArray[1] = 'O';
        monthArray[2] = 'V';
        actDays = days - 304;
        monthLen = 30;
    }
    else if (days > 334 && days <= 365){
        monthArray[0] = 'D';
        monthArray[1] = 'E';
        monthArray[2] = 'C';
        actDays = days - 334;
        monthLen = 31;
    }

    outputVal = days * 60 * 60 * 24;

    dateArray[0] = monthArray[0];
    dateArray[1] = monthArray[1];
    dateArray[2] = monthArray[2];
}

void updateDay(long unsigned int dateInput, unsigned int monthLen)
{

    days = dateInput % monthLen;
    outputVal = days * 60 * 60 * 24;
    for (j = 1; j >= 0; j--)
    {
        daysArray[j] = ((days % 10) + 0x30);
        days = days / 10;
    }

    dateArray[4] = daysArray[0];
    dateArray[5] = daysArray[1];

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
    ADC12CTL0 = ADC12SHT0_9 | ADC12REFON | ADC12ON | ADC12MSC;
    ADC12CTL1 = ADC12SHP + ADC12CSTARTADD_1;
    ADC12MCTL1 = ADC12SREF_1 + ADC12INCH_10; // for temperature sensor
    //ADC12MCTL2 = ADC12SREF_0 + ADC12INCH_0 + ADC12EOS; // for scroll wheel
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

    averageTemp(inTemp);

    dispC = averageC * 10;

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

    dispF = averageF * 10;

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


void averageTemp(unsigned int inTemp){

    tempCel = (float)(((long)inTemp-CALADC12_15V_30C)*(85 - 30))/(CALADC12_15V_85C - CALADC12_15V_30C) + 30.0;

    tenSecReadingsC[timer_cnt % 10] = tempCel;

    if(timer_cnt >= 15552010){
        averageC = (float)(tenSecReadingsC[0] + tenSecReadingsC[1] + tenSecReadingsC[2] + tenSecReadingsC[3] + tenSecReadingsC[4] + tenSecReadingsC[5]
                   + tenSecReadingsC[6] + tenSecReadingsC[7] + tenSecReadingsC[8] + tenSecReadingsC[9])/10;
        averageF = (float)(averageC * (1.8)) + 32;

    }

    times[timer_cnt % 60] = timer_cnt;
    tempC[timer_cnt % 60] = averageC;

}

void configLaunchButtons(){
    //2 Launchpad Buttons: P1.1, P2.1

    P2SEL &= (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT0); //xxxx xx0x
    P1SEL &= (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT0); //xxxx xx0x

    P2DIR &= (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT0); //xxxx xx0x
    P1DIR &= (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT0); //xxxx xx0x

    P2REN |= (BIT1);
    P1REN |= (BIT1);

    P2OUT |= (BIT1);
    P1OUT |= (BIT1);
}

char launchpadButtonStates(){
    //1 is not pressed
    //0 is pressed

    char inBits11, inBits21;
    char out11=0, out21=0, out=0;

    inBits21 = P2IN & (BIT1); //0000 00x0, keep bits 1
    inBits11 = P1IN & (BIT1); //0000 00x0, keep bit 1

    if (!(inBits21 & BIT1))
        out21 = 0x01; //0000 0001
    else if (!(inBits11 & BIT1))
        out11 = 0x04; //0000 0100
    else
        out21 = 0x00;

    out = (out21|out11);

    return out;
}

unsigned int potValue(void)
{
    REFCTL0 &= ~REFMSTR;
    ADC12CTL0 = ADC12SHT0_9 | ADC12REFON | ADC12REF2_5V | ADC12ON;
    ADC12CTL1 = ADC12SHP;
    ADC12MCTL0 = ADC12SREF_0 + ADC12INCH_0;

    P6SEL |= BIT0;
    ADC12CTL0 &= ~ADC12SC;

    ADC12CTL0 |= ADC12SC + ADC12ENC;

    while (ADC12CTL1 & ADC12BUSY)
    {
        __no_operation();
    }

    potVal = ADC12MEM0 & 0x0FFF;
    return potVal;
}

void printPotVal(unsigned int gal)
{
    for (j = 4; j >= 0; j--)
    {
        potArray[j] = ((gal % 10) + 0x30);
        gal = gal / 10;
    }
}

void swDelay(char numLoops)
{
    // This function is a software delay. It performs
    // useless loops to waste a bit of time
    //
    // Input: numLoops = number of delay loops to execute
    // Output: none
    //
    // smj, ECE2049, 25 Aug 2013

    volatile unsigned int i,j;  // volatile to prevent removal in optimization
                                // by compiler. Functionally this is useless code

    for (j=0; j<numLoops; j++)
    {
        i = 50000 ;                 // SW Delay
        while (i > 0)               // could also have used while (i)
           i--;
    }
}
