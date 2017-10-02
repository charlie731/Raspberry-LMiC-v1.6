
Trabaja en US902-928 Mhz y EU863-870. BANDS

Supports SX1272/SX1276 and HopeRF RFM92/RFM95 tranceivers

Arduino port of the LMIC (LoraWAN-in-C, formerly LoraMAC-in-C) framework provided by IBM.

# Raspberry-LMiC-v1.6
Adaptación de la Librería LMiC v1.6 para Raspberry Pi 2/3 Radios RFM92/RFM95... (SX1272)(SX1276)

1.-Installing for Raspberry PI

You need 3 dependencies:

build essential package

apt-get install build-essential

other tools packages 

apt-get install git-core wget

2.- You need install -> bcm2835_library see installation... 

Connection and pins definition

Boards pins (Chip Select, IRQ line, Reset and LED) definition are set in the samples sketch when needed see Raspberry PI .png .pdf

//#define RF_LED_PIN RPI_V2_GPIO_P1_07 // Led on GPIO4 so P1 connector pin #7

#define RF_CS_PIN  RPI_V2_GPIO_P1_26 // Slave Select on CE0 so P1 connector pin #24

//#define RF_IRQ_PIN RPI_V2_GPIO_P1_22 // IRQ on GPIO25 so P1 connector pin #22

#define RF_RST_PIN RPI_V2_GPIO_P1_29// Reset on GPIO5 so P1 connector pin #29



// Pin mapping

const lmic_pinmap lmic_pins = {

    .nss  = RF_CS_PIN,
    
    .rxtx = LMIC_UNUSED_PIN,
    
    .rst  = RF_RST_PIN,
    
    .dio  = {4, 20, 21},
    
};

Then in your code you'll have exposed RF_CS_PIN, RF_IRQ_PIN, RF_RST_PIN and RF_LED_PIN and you'll be able to do some #ifdef RF_LED_LIN for example. See ttn-otaa sample code.



