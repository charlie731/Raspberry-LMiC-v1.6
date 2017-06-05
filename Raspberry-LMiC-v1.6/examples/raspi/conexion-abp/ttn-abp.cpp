/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network.
 *
 * This uses ABP (Activation-by-personalisation), where a DevAddr and
 * Session keys are preconfigured (unlike OTAA, where a DevEUI and
 * application key is configured, while the DevAddr and session keys are
 * assigned/generated in the over-the-air-activation procedure).
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!
 *
 * To use this sketch, first register your application and device with
 * the things network, to set or generate a DevAddr, NwkSKey and
 * AppSKey. Each device should have their own unique values for these
 * fields.
 *
 * Do not forget to define the radio type correctly in config.h.
 *
 *******************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include <lmic.h>
#include <hal/hal.h>



// LoRaWAN NwkSKey, network session key
// Use this key for The Things Network
static const u1_t PROGMEM DEVKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
//void os_getDevKey (u1_t* buf) {  memcpy_P(buf, DEVKEY, 16);}

// LoRaWAN AppSKey, application session key
// Use this key to get your data decrypted by The Things Network
static const u1_t PROGMEM ARTKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
//void os_getArtKey (u1_t* buf) {  memcpy_P(buf, ARTKEY, 16);}

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
static const u4_t PROGMEM DEVADDR = 0x00000000; // 26 <-- Change this address for every node!
//void os_getDevAddr (u1_t* buf) { memcpy_P(buf, DEVADDR, 8);}

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

//////////////////////////////////////////////////
// APPLICATION CALLBACKS
//////////////////////////////////////////////////

/* provide application router ID (8 bytes, LSBF)
void os_getArtEui (u1_t* buf) {
    memcpy(buf, APPEUI, 8);
}

// provide device ID (8 bytes, LSBF)
void os_getDevEui (u1_t* buf) {
    memcpy(buf, DEVEUI, 8);
}

// provide device key (16 bytes)
void os_getDevKey (u1_t* buf) {
    memcpy(buf, DEVKEY, 16);
}
*/
static uint8_t mydata[] = {"start"};
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 10;

// Pin mapping

//Flag for Ctrl-C
volatile sig_atomic_t force_exit = 0;

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

#ifndef RF_LED_PIN
#define RF_LED_PIN NOT_A_PIN
#endif

void do_send(osjob_t* j) {
    char strTime[16];
    getSystemTime(strTime , sizeof(strTime));
    printf("%s: ", strTime);

    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        printf("OP_TXRXPEND, not sending\n");
    } else {
        digitalWrite(RF_LED_PIN, HIGH);
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);

    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void onEvent (ev_t ev) {

    char strTime[16];
    getSystemTime(strTime , sizeof(strTime));
    printf("%s: ", strTime);

    switch(ev) {
        case EV_SCAN_TIMEOUT:
             printf("EV_SCAN_TIMEOUT");
            break;
        case EV_BEACON_FOUND:
             printf("EV_BEACON_FOUND");
            break;
        case EV_BEACON_MISSED:
             printf("EV_BEACON_MISSED");
            break;
        case EV_BEACON_TRACKED:
             printf("EV_BEACON_TRACKED");
            break;
        case EV_JOINING:
             printf("EV_JOINING");
            break;
        case EV_JOINED:
             printf("EV_JOINED");
            break;
        case EV_RFU1:
             printf("EV_RFU1");
            break;
        case EV_JOIN_FAILED:
             printf("EV_JOIN_FAILED");
            break;
        case EV_REJOIN_FAILED:
             printf("EV_REJOIN_FAILED");
            break;
        case EV_TXCOMPLETE:
             printf("EV_TXCOMPLETE (includes waiting for RX windows)");

            if (LMIC.txrxFlags & TXRX_ACK)
               {
               printf("Frequencia ACK");
               printf("Frecuencia ACK %ld \n",LMIC.freq);
			   printf("Canal Transmision ACK \n");
               printf("Channel ACK%ld\n",LMIC.txChnl);

                 printf("Received ack \n");
               }
            if (LMIC.dataLen)
              {
                printf("Received\n ");
                printf("LMIC.datalen %ld\n",LMIC.dataLen);
                printf(" bytes of payload\n\n");

                printf("Frequencia Recepcion: ");
                printf("Frecuencia  %ld \n",LMIC.freq);
			    printf("Canal Recepcion: ");
                printf("Channel %ld\n",LMIC.txChnl);

                uint8_t downlink[LMIC.dataLen];
                memcpy(&downlink,&(LMIC.frame+LMIC.dataBeg)[0],LMIC.dataLen);
                // Turn on LED if we get the magic number
                // digitalWrite(13, downlink[0] == 42);
                printf("Data Received\n ");
                printf("LMIC: ");

                for (int x=LMIC.dataBeg; x<(LMIC.dataBeg + LMIC.dataLen);x++)
                {
                   printf("%c",LMIC.frame[x]);
                   //printf(" ");
                }
                printf("\n");
              }

            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
             printf("EV_LOST_TSYNC\n");
            break;
        case EV_RESET:
             printf("EV_RESET\n");
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
             printf("EV_RXCOMPLETE\n");
            break;
        case EV_LINK_DEAD:
             printf("EV_LINK_DEAD\n");
            break;
        case EV_LINK_ALIVE:
             printf("EV_LINK_ALIVE\n");
            break;
         default:
           printf("Frequencia y Canal En Default\n");
           printf("Frequencia Transmision\n");
           printf("Frecuencia %ld\n",LMIC.freq);
           printf("Canal Transmision\n");
           printf("Channel %ld\n",LMIC.txChnl);

            printf("Unknown event\n");
            break;
    }
}

/* ======================================================================
Function: sig_handler
Purpose : Intercept CTRL-C keyboard to close application
Input   : signal received
Output  : -
Comments: -
====================================================================== */
void sig_handler(int sig)
{
  printf("\nBreak received, exiting!\n");
  force_exit=true;
}

/* ======================================================================
Function: main
Purpose : not sure ;)
Input   : command line parameters
Output  : -
Comments: -
====================================================================== */

int main(void)
{
    // caught CTRL-C to do clean-up
    signal(SIGINT, sig_handler);

    printf("%s Starting\n", __BASEFILE__);

      // Init GPIO bcm
    if (!bcm2835_init()) {
        fprintf( stderr, "bcm2835_init() Failed\n\n" );
        return 1;
    }

	// Show board config
    // printConfig(RF_LED_PIN);
    //printKeys();

    // Light off on board LED
    //pinMode(RF_LED_PIN, OUTPUT);
    //digitalWrite(RF_LED_PIN, HIGH);

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    LMIC_setSession (0x1, DEVADDR, (u1_t*)DEVKEY, (u1_t*)ARTKEY);

	LMIC_selectSubBand(7);

	// Disable data rate adaptation
	LMIC_setAdrMode(0);
	// Disable link check validation
	LMIC_setLinkCheckMode(0);
	// Disable beacon tracking
	//LMIC_disableTracking ();
	// Stop listening for downstream data (periodical reception)
	//LMIC_stopPingable();

	// TTN uses SF9 for its RX2 window.
	LMIC.dn2Dr = DR_SF9;
	// Set data rate and transmit power (note: txpow seems to be ignored by the library)
	LMIC_setDrTxpow(DR_SF7,14);
	// Start job (sending automatically starts OTAA too)
    do_send(&sendjob);

    while(!force_exit) {

        //os_runloop();
		  os_runloop_once();

      // We're on a multitasking OS let some time for others
      // Without this one CPU is 99% and with this one just 3%
      // On a Raspberry PI 3
      usleep(1000);
    }

    // We're here because we need to exit, do it clean

    // Light off on board LED
    digitalWrite(RF_LED_PIN, LOW);

    // module CS line High
    digitalWrite(lmic_pins.nss, HIGH);
    printf( "\n%s, done my job!\n", __BASEFILE__ );
    bcm2835_close();
    return 0;
}

