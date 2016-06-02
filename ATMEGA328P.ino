// ACTIVATE DEBUG MESSAGES & TEST CODE
#define DEBUG
#ifdef DEBUG
    #define DEBUG_PRINT(x)  Serial.println (x)
    //#define DEBUG_SDcard
#else
    #define DEBUG_PRINT(x)
#endif


// SD LIBRARY:
#ifdef DEBUG_SDcard
    #include <SPI.h>
#endif
#include <SD.h>
Sd2Card card;
SdVolume volume;
SdFile root;
File myFile;
#define SDcardCHIPSELECT 2 //PD2 is free in Radial3_rev1 eagle 
char filename[] = "LOGGER00.csv";


// I2C library
//#include <Wire.h>


// SCREEN LIBRARY
// http://forum.arduino.cc/index.php?topic=176794
#include <SPI.h>
#include "PCD8544_SPI.h"
PCD8544_SPI lcd;


// VARISTOR voltage divider and coefficients (check curve fitting python script)
#define SERIESRESISTOR 4630    
#define THERMISTORNOMINAL 10000 
#define BCOEFFICIENT 4005.58
#define TEMPERATURENOMINAL 298.26 
#define THERMISTORPIN A3 
#define THERMISTOR_LOW_THRESHOLD 27.0   // Celcius
#define THERMISTOR_HIGH_THRESHOLD 29.0  // Celcius


// CURRENT SENSOR
#define CURRENTSENSORPIN A0 
#define COEFFICIENT_mVperAmp 185 
#define COEFFICIENT_ACSoffset 2500 


// SHIFT REGISTER
#define HC595_latchPin 7 //Pin connected to latch pin (ST_CP) of HC595
#define HC595_clockPin 6 //Pin connected to clock pin (SH_CP) of HC595
#define HC595_dataPin 5  //Pin connected to Data in   (DS) of HC595


// LOOP TIMING
#define LOOP_TIMING_ADC 500
unsigned long millis_ADC = 0;
#define LOOP_TIMING_SDcard 1000 
unsigned long millis_SDcard = 0;


// LED INDICATOR STATES
int Etat_bougie = 1;  // 1 : 0   < amp < 2    --> red   pin 1
                      // 3 : 2   < amp < 2.5  --> green pin 3
                      // 2 : 2.5 < amp        --> blue  pin 2
float temperature = 0;
float courant     = 0;


/////////////////////////////// MAIN FUNCTIONS ///////////////////////////////



void setup() {
      
    //Display 
    lcd.begin(); // 14X6 characters
    //           01234567890123
    lcd.print(F("              "));
    lcd.print(F("   FLIGHT     "));
    lcd.print(F("              "));
    lcd.print(F(" INSTRUMENTS  "));

    // SHIFT REGISTER
    pinMode(HC595_latchPin, OUTPUT);
    pinMode(HC595_dataPin, OUTPUT);  
    pinMode(HC595_clockPin, OUTPUT);
    registerWrite(1,LOW);
    delay(1000);   
      
    // SIGNIFY DEBUG MODE
#ifdef DEBUG
    for (int i=0; i <= 9; i++){
        registerWrite(1,HIGH);
        delay(100);   
        registerWrite(1,LOW);
        delay(100);   
    } 
#endif

    // UART
    Serial.begin(9600);
    Serial.println("\nConnection serial etablie");

    // SD CARD
#ifdef DEBUG_SDcard
    SD_card_test();
#endif
    SDcard_init();

    // UNIT LOOP scheduler
    millis_ADC    = millis();
    millis_SDcard = millis();
}





void loop() {
    if ( millis() - millis_ADC >= LOOP_TIMING_ADC ){
        String message;
        size_t len;
        millis_ADC  = millis();
        temperature = lire_temperature();
        courant     = lire_courant();
        message    = "T=" + String(int(temperature)) + " I=" + String(courant);
        //char message_lcd[84];
        //message.toCharArray(message_lcd,84);
        lcd.clear();
        lcd.print(message);
        Serial.println(message); 

        Etat_bougie = set_LED_indicator(Etat_bougie,temperature);
    }
    if ( millis() - millis_SDcard >= LOOP_TIMING_SDcard ){
        millis_SDcard = millis();
        myFile        = SD.open(filename, FILE_WRITE);
        if (myFile) {
            myFile.print(courant);
            myFile.print(",");
            myFile.println(temperature);
            myFile.close();
        }    
    }
}







/////////////////////////////// OTHER FUNCTIONS ///////////////////////////////

void SDcard_init(){
    // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
    // Note that even if it's not used as the CS pin, the hardware SS pin 
    // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
    // or the SD library functions will not work. 
    //pinMode(10, OUTPUT);
    if (!SD.begin(SDcardCHIPSELECT)) Serial.println("SD Card initialization failed!");
    // create a new file
    for (uint8_t i = 0; i < 100; i++) {
        filename[6] = i/10 + '0';
        filename[7] = i%10 + '0';
        if (! SD.exists(filename)) {
            // only open a new file if it doesn't exist
            myFile = SD.open(filename, FILE_WRITE); 
            break;  // leave the loop!
        }
    }
    if (! myFile) Serial.println("couldnt create file");
    Serial.print("Logging to: ");
    Serial.println(filename);
    if (myFile) {
        myFile.print("Courant");
        myFile.print(",");
        myFile.println("Temperature");
        myFile.close();
        Serial.println("SD card initialization done.");
    }
}


int set_LED_indicator(int Etat, float temperature){
    registerWrite(Etat, LOW);
    if( temperature < THERMISTOR_LOW_THRESHOLD ){
        Etat = 1;
        registerWrite(Etat, HIGH);
    } else if( (THERMISTOR_LOW_THRESHOLD <= temperature) && (temperature < THERMISTOR_HIGH_THRESHOLD) ){
        Etat = 3;
        registerWrite(Etat, HIGH);
    } else{
        Etat = 2;
        registerWrite(Etat, HIGH);
    }
    return Etat;
}


float lire_temperature() {
    // convert the varistor value to temperature
    float reading;
    reading = analogRead(THERMISTORPIN);
    reading = (1023 / reading)  - 1;
    reading = SERIESRESISTOR / reading;
    //Steinhart-Hart equation (simplified - voir curve fit en python)
    // (1/T) = (1/T0) + 1/B ln (R/Ro)
    float steinhart;
    steinhart = reading / THERMISTORNOMINAL;     // (R/Ro)
    steinhart = log(steinhart);                  // ln(R/Ro)
    steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
    steinhart += 1.0 / (TEMPERATURENOMINAL);     // + (1/To)
    steinhart = 1.0 / steinhart;                 // Invert
    steinhart -= 273.15;                         // convert to C
    return steinhart;
}


float lire_courant() {
   float reading;
   reading = analogRead(CURRENTSENSORPIN);
   reading = (reading / 1023.0) * 5000;     // Gets you mV
   reading = ((reading  - COEFFICIENT_ACSoffset) / COEFFICIENT_mVperAmp);
   return reading;
}


void registerWrite(int whichPin, int whichState) {               // This method sends bits to the shift register:
    byte bitsToSend = 0;                                           // the bits you want to send
    digitalWrite(HC595_latchPin, LOW);                             // turn off the output so the pins don't light up while you're shifting bits:
    bitWrite(bitsToSend, whichPin, whichState);                    // turn on the next highest bit in bitsToSend:
    shiftOut(HC595_dataPin, HC595_clockPin, MSBFIRST, bitsToSend); // shift the bits out:
    digitalWrite(HC595_latchPin, HIGH);                            // turn on the output so the LEDs can light up:
}



///////////////////////////////     DEBUG       ///////////////////////////////

#ifdef DEBUG_SDcard
void SD_card_test(){
    Serial.print("\nInitializing SD card...");

    // we'll use the initialization code from the utility libraries
    // since we're just testing if the card is working!
    if (!card.init(SPI_HALF_SPEED, SDcardCHIPSELECT)) {
        Serial.println("initialization failed. Things to check:");
        Serial.println("* is a card inserted?");
        Serial.println("* is your wiring correct?");
        Serial.println("* did you change the SDcardCHIPSELECT pin to match your shield or module?");
        return;
    } else {
        Serial.println("Wiring is correct and a card is present.");
    }

    // print the type of card
    Serial.print("\nCard type: ");
    switch (card.type()) {
         case SD_CARD_TYPE_SD1:
             Serial.println("SD1");
             break;
         case SD_CARD_TYPE_SD2:
             Serial.println("SD2");
             break;
         case SD_CARD_TYPE_SDHC:
             Serial.println("SDHC");
             break;
         default:
             Serial.println("Unknown");
    }

    // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
    if (!volume.init(card)) {
        Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
        return;
    }


    // print the type and size of the first FAT-type volume
    uint32_t volumesize;
    Serial.print("\nVolume type is FAT");
    Serial.println(volume.fatType(), DEC);
    Serial.println();

    volumesize = volume.blocksPerCluster();       // clusters are collections of blocks
    volumesize *= volume.clusterCount();          // we'll have a lot of clusters
    volumesize *= 512;                            // SD card blocks are always 512 bytes
    Serial.print("Volume size (bytes): ");
    Serial.println(volumesize);
    Serial.print("Volume size (Kbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);
    Serial.print("Volume size (Mbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);


    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
    root.openRoot(volume);

    // list all files in the card with date and size
    root.ls(LS_R | LS_DATE | LS_SIZE);
}
#endif
