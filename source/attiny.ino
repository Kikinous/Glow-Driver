/*
   Julien Borghetti - March 2016
   Revision Avril 2016 - eagle file: Mono_rev1_GlowDriver.sch

   Programmed for an attiny85
   1MHz internal clock
   - read PIN_RECEIVER signal to turn ON/OFF the glow plug
   - ADC of sensed current
   - I2C to main MCU

                           +-\/-+
                   RESET  1|    |8  VCC   (2.7-5.5V)
    ACS712  (ADC3) Pin 3  2|    |7  Pin 2 (ADC3,SCK) I2C   SCK -> Uno A5
     RADIO  (ADC4) Pin 4  3|    |6  Pin 1 (PWM,MISO) LM2596/LED
                     GND  4|    |5  Pin 0 (PWM,MOSI) I2C   SDA -> Uno A4
                           +----+ 


   */
#define DEBUG

#include "TinyWireS.h"          // "ATTiny85 @ 1Mhz" pour la librairie.
//http://kasey.fr/ATTiny-capteur-universel-en-I2C.html

#define I2C_SLAVE_ADDR  0x26    // On note l'adresse I2C
#define PIN_REGULATOR 1   
#define PIN_RECEIVER 4   

#define TIME_OUT 25000

byte byteRcvd;
boolean driving_glow = false;

void setup()  {
    pinMode(PIN_REGULATOR , OUTPUT);
    pinMode(PIN_RECEIVER  , INPUT);


    TinyWireS.begin(I2C_SLAVE_ADDR);      // on rejoint le bus avec une adresse d'esclave (similaire a la lib wire)
    byteRcvd = 0;
}


void loop(){
    unsigned long duration;
    duration = pulseIn(PIN_RECEIVER, HIGH,TIME_OUT);

    if(duration < 1500){
        driving_glow = true;
        digitalWrite(PIN_REGULATOR, LOW);  
    }
    else{
        driving_glow = false;
        digitalWrite(PIN_REGULATOR, HIGH);
    }

    if (TinyWireS.available()){         // si on revoit quelque chose sur le bus I2C
        byteRcvd = TinyWireS.receive(); // on l'enregistre
#ifdef DEBUG
        Blink();                        // on blink un coup pour montrer que l'on est content
#endif
    }
}

void Blink() {                          // blinker du pauvre :)
    if(driving_glow == false){
        digitalWrite(PIN_REGULATOR,HIGH);
        delay(100);
        digitalWrite(PIN_REGULATOR,LOW);
    }
    else{
        digitalWrite(PIN_REGULATOR,LOW);
        delay(100);
        digitalWrite(PIN_REGULATOR,HIGH);
    }
    
}
