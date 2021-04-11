#define HEX3_HEX0_BASE        0xFF200020    // first 4 of the 6 displays
#define HEX5_HEX4_BASE        0xFF200030    // far left 2 of the 6 displays
#define KEY_BASE              0xFF200050    // push buttons
#define SW_BASE               0xFF200040    // switches
#define JP1_BASE              0xFF200064    // parrallel ports for hot water output
#define JP2_BASE              0xFF200074    // parrallel ports for cold water output
    
// make port 31 on both parallel ports represent drain open-close
// to visualize the output, expacially with both temperatures
        
// JTAG UART
#define JTAG_UART_DATA ((volatile int*) 0xFF201000)
#define JTAG_UART_CONTROL ((volatile int*) (0xFF201000+4))
    
// global variables
int temp;
int waterTemp;
int hotTempValue; 
int coldTempValue;

// the temperature map is updated every time the bath does a full non-stop fill
// the time and temperature 
/* struct tempMap[20] = {
        **save the ratio of hot to cold and the resuling temperature to be used again**
    }

*/  
        
void DisplayTimeMinutes(int value) {

    // points for the first 7-segment display segment
        volatile int *hexPointer = (int)HEX3_HEX0_BASE;
    
    // subtract for hours
        int hours = 0;
        int minutes;
    
        if(value > 59){
            minutes = value % 60;
            hours = value/60;
        }
        else{
            minutes = value;
        }
    
    // if more than one hour, enable hour setting
    

    // 7-segement display lookup table
        volatile int displayState[61] = {
                0x3F,   // zero
                0x6,    // 1
                0x5B,
                0x4F,
                0x66,
                0x6D,
                0x7D,
                0x7,
                0x7F,
                0x6F,   // 9
                0x63F,  // 10
                0x606,  // 11
                0x65B,  // 12
                0x64F,
                0x666,
                0x66D,
                0x67D,
                0x607,
                0x67F,  
                0x66F,  // 19
                0x5B3F, // 20
                0x5B06, 
                0x5B5B, 
                0x5B4F,
                0x5B66,
                0x5B6D,
                0x5B7D,
                0x5B07,
                0x5B7F, 
                0x5B6F, // 29
                0x4F3F, // 30
                0x4F06, 
                0x4F5B, 
                0x4F4F,
                0x4F66,
                0x4F6D,
                0x4F7D,
                0x4F07,
                0x4F7F, 
                0x4F6F, // 39
                0x663F, // 40
                0x6606, 
                0x665B, 
                0x664F,
                0x6666,
                0x666D,
                0x667D,
                0x6607,
                0x667F, 
                0x666F, // 49
                0x6D3F, // 50
                0x6D06, 
                0x6D5B, 
                0x6D4F,
                0x6D66,
                0x6D6D,
                0x6D7D,
                0x6D07,
                0x6D7F, 
                0x6D6F, // 59   
                0x0,    // nothing  
        };

    // set display 
        *hexPointer = displayState[minutes]; 
}

void DisplayHex(int input)
{   
    volatile int *HEX0_ptr = HEX3_HEX0_BASE; 
    volatile int *HEX4_ptr = HEX5_HEX4_BASE; 
    
    volatile int disp01 = (input/1000000);
    volatile int disp23 = (input/10000) %100;
    volatile int disp45 = (input/100) %100;
    volatile int disp67 = input %100;
    
    volatile int d6 = disp67 %10 ;
    volatile int d7 = ((disp67-d6) /10);
    volatile int d2 = disp23 %10;
    volatile int d3 = ((disp23-d2) /10);
    volatile int d4 = disp45 % 10;
    volatile int d5 = (disp45-d4) /10;
    volatile int d0 = disp01 % 10;
    volatile int d1 = (disp01-d0) /10;

    volatile int lookUpTable[17] = { // same as Lab1 
        0x40,// 0 
        0x06,// 1
        0x5B,// 2
        0x4F,// .
        0x66,// .
        0x6D,// .
        0x7D,// .
        0x07,// .
        0x7F,// 8
        0x6F,// 9
        0x77,// A
        0x7C,// B
        0x39,// C
        0x5E,// D
        0x79,// E
        0x40,
    }; 

    *HEX0_ptr = lookUpTable[d6] + (lookUpTable[d7] << 8) + (lookUpTable[d4] << 16) + (lookUpTable[d5] << 24);
    *HEX4_ptr = lookUpTable[d2] + (lookUpTable[d3] << 8) + (lookUpTable[d0] << 16) + (lookUpTable[d1]<<24);
}

void DisplayStart(){
    volatile int *hexPointer = (int)HEX3_HEX0_BASE;
    volatile int *hexPointer2 = (int)HEX5_HEX4_BASE;    // for left 2 displays
    
    *hexPointer = 0x78775078;
    *hexPointer2 = 0x0000006d;
}

void DisplayCancel(){
    volatile int *hexPointer = (int)HEX3_HEX0_BASE;
    volatile int *hexPointer2 = (int)HEX3_HEX0_BASE;    // for left 2 displays

    *hexPointer = 0x37397938;
    *hexPointer2 = 0x00003977;
}

void EnterSetupMode(){
    // inputs 
    volatile int *buttonPointer = (int)KEY_BASE;    // push buttons
    volatile int *switchPointer = (int)SW_BASE;     // switches
    
    while(*buttonPointer == 0x00000000 && *switchPointer == 0x00000000){
        // do nothing
    }
    
    // check what has been pressed and perform the corresponding functionality
    if(*buttonPointer != 0x00000000){   // if a button is pressed
        *buttonPointer = 0x00000000;    // reset it
        
            
    }
    
    if(*switchPointer != 0x00000000){   // if a number is pressed           
        
    // tell user to click the start, temperature set, or timer set button
        /*if(*switchPointer == 0x0000007){  // temp button clicked
            
        }
        *//*
        if(*switchPointer == 0x00000002){   // start button clicked
            startBath();
        }
        
        if(*switchPointer = 0x ){   // timer button clicked
            
        }
        if(off){
            // if on prompt off screen
            // if in setup mode (which we are) turn off?
        }
        *buttonPointer = 0x00000000; // reset button    */
    }
}

void startBath(){
    // no need for "cancel" button to cause interupts
    volatile int *parralelPortHot = (int)JP1_BASE;
    volatile int *parralelPortCold = (int)JP2_BASE;
    
    // setting port addresses to global temperature variables that were set in setTemp()
    *parralelPortHot = hotTempValue;
    *parralelPortCold = coldTempValue;
    
    // edit "water" outputs correspondingly - todo figure out outputs
}

void drainBath(){
    volatile int *parralelPortHot = (int)JP1_BASE;
    volatile int *parralelPortCold = (int)JP2_BASE;
    
    // disable all water and enable "drain open" bits (the first two bits on both parralel port sets)
    *parralelPortHot = 0x80000000;      
    *parralelPortCold = 0x80000000;
}

//note address ff200060-ff200063 must be ffffffff
// and address ff200070-ff200073 must be ffffffff to properly display water
// todo reverse array so dark represents flow
void setValves(double intensityPercentHot, double intensityPercentCold){
    int intensityValueHot = (int)(31 * intensityPercentHot);    // does it matter if percent may be decimal?
    int intensityValueCold = (int)(31 * intensityPercentCold);  
    
    volatile int *parralelPortHot = (int)JP1_BASE;
    volatile int *parralelPortCold = (int)JP2_BASE;

    // writing intensity onto parallel ports to symbolize the intensity of hot and cold

    // when port address set to these values, all ports UP TO the corresponding port (index 0 = port 0) to true
    volatile int portLevelMap[31] = {
        0x00000000, // zero, note that to drain bath is 0x80000000
        0x00000001,
        0x00000003, 
        0x00000007,
        0x0000000f,
        0x0000001f,
        0x0000003f,
        0x0000007f,
        0x000000ff,
        0x000001ff,
        0x000003ff,
        0x000007ff,
        0x00000fff,
        0x00001fff,
        0x00003fff,
        0x00007fff,
        0x0000ffff,
        0x0001ffff,
        0x0003ffff,
        0x0007ffff,
        0x001fffff,
        0x003fffff,
        0x007fffff,
        0x00ffffff,
        0x01ffffff,
        0x03ffffff,
        0x07ffffff,
        0x0fffffff,
        0x1fffffff,
        0x3fffffff,
        0x7fffffff  // all on except 31 (max intensity)
    };
        
    // setting the ports to the specific values to "set" both water intensities
    // but do not turn them on yet (call startBath to apply address values)
        hotTempValue = portLevelMap[intensityValueHot];
        coldTempValue = portLevelMap[intensityValueCold];
    
    // set addresses of ports to the values correpsonding to the intensities 
        *parralelPortHot = hotTempValue;
        *parralelPortCold = coldTempValue;
}




// do nothing until a button is pressed
void sleep(){
    // inputs 
    volatile int *buttonPointer = (int)KEY_BASE;
    volatile int *hexPointer = (int)HEX3_HEX0_BASE;
    volatile int *hexPointer2 = (int)HEX3_HEX0_BASE;
    
    // await any button press
    while(*buttonPointer == 0x00000000 && *hexPointer == 0x00000000 && *hexPointer2 == 0x00000000){
        // do nothing
    }
    DisplayStart();
    EnterSetupMode();
}

void testCountUpTo59(){
    while(1)
    {
        for(int i = 0; i< 60; ++i){
            for (int j = 500000; j != 0; --j)
                    ; // delay loop
            DisplayTimeMinutes(i);
        }
    }
}

// re-calibrate/optimize temperate settings
// with respect to the water output?
void verifyTemp(){
    
}

void printStr(char output[]){
    unsigned char *pOutput;

    pOutput = output;
    while(*pOutput) //strings in C are zero terminated
    {
         //if room in output buffer
         if((*JTAG_UART_CONTROL)&0xffff0000  ) 
         {
            //then write the next character
            *JTAG_UART_DATA = (*pOutput++); 
         }
     }
}

void printInt(int output){
    unsigned int *pOutput;

    pOutput = output;
    while(*pOutput) //strings in C are zero terminated
    {
         //if room in output buffer
         if((*JTAG_UART_CONTROL)&0xffff0000  ) 
         {
            //then write the next character
            *JTAG_UART_DATA = (*pOutput++); 
         }
     }
}

void clearJTAG(){
    unsigned char *pOutput;
    pOutput = 1;
    
    *JTAG_UART_CONTROL = (*pOutput);
    
    /*while(*pOutput) //strings in C are zero terminated
    {
         //if room in output buffer
         if((*JTAG_UART_CONTROL)&0xffff0000  ) 
         {
            //then write the next character
            *JTAG_UART_DATA = (*pOutput++); 
         }
     }*/
}


int main(void) {
    //sleep();
    //testCountUpTo59(); //uncomment to test counting mode  
    //setHot(50);
    //drainBath();
    DisplayStart();
    // not working 
    //setTemp(0.5,0.5);
    
    // uncomment to test print char to console functionality
        /*char p[5] = {'h','e','l','l','o'};
        printStr(p);                            */

    // uncomment to test print int to console functionality
    /*  int address = 0x0000000f;
        int test = 34;
        printInt(test);     */
    
    // clear console
        //clearJTAG();
    
    //testCountUpTo59();
    
    while(1){
        // do nothing
    }
    
}





