/*******************************************************************************
Adaptado Charlie "Vasco" OTAA
 *******************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
 
#include <lmic.h>
#include <hal/hal.h>

// This EUI must be in little-endian format, so least-significant-byte

// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,0x70.
static const u1_t PROGMEM APPEUI[8]={ 0x00, 0x46, 0x00, 0xF0, 0x7E, 0xD5, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]={ 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// Here on Raspi we use part of MAC Address do define devEUI so 
// This one above is not used, but you can still old method 
// reverting the comments on the 2 following line
//void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);} //{ getDevEuiFromMac(buf); }

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
static const u1_t PROGMEM APPKEY[16] = { 0x00, 0x00, 0x4F, 0x00, 0xCA, 0x43, 0x28, 0xE9, 0x33, 0x80, 0x8E, 0x9C, 0x33, 0x20, 0x61, 0x1C };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

static uint8_t mydata[] = "Vamos Colega!!!";
static osjob_t sendjob;

 u4_t netid = 0;
 devaddr_t devaddr = 0;
 u1_t nwkKey [16];
 u1_t artKey [16];

// Schedule TX every this many seconds (might become longer due to duty)
// cycle limitations).
const unsigned TX_INTERVAL = 15;

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
        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 1);
        printf("Packet queued\n");
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void onEvent (ev_t ev) {
    char strTime[16];
    getSystemTime(strTime , sizeof(strTime));
    printf("%s: ", strTime);
 
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            printf("EV_SCAN_TIMEOUT\n");
        break;
        case EV_BEACON_FOUND:
            printf("EV_BEACON_FOUND\n");
        break;
        case EV_BEACON_MISSED:
            printf("EV_BEACON_MISSED\n");
        break;
        case EV_BEACON_TRACKED:
            printf("EV_BEACON_TRACKED\n");
        break;
        case EV_JOINING:
            printf("EV_JOINING\n");
        break;
        case EV_JOINED:
            printf("EV_JOINED\n");
           
            LMIC_getSessionKeys (&netid, &devaddr, nwkKey, artKey);
            
            printf ("netid:%d ",netid);
            printf ("\n");
            printf ("devaddr:%X",devaddr);
            printf ("\n");
            printf ("artKey:");
            for (int i = 0; i <sizeof (artKey); ++ i)
            {
              printf ("%X",artKey[i]);
            }
            
            printf ("\n");
            
            printf ("nwkKey:");
            for (int i = 0; i <sizeof (nwkKey); ++ i)
            {
              printf ("%X",nwkKey[i]);
            }
            
            printf ("\n");
           
        break;
        case EV_RFU1:
            printf("EV_RFU1\n");
        break;
        case EV_JOIN_FAILED:
            printf("EV_JOIN_FAILED\n");
        break;
        case EV_REJOIN_FAILED:
            printf("EV_REJOIN_FAILED\n");
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
            printf("Unknown event\n");
        break;
    }
}

void setup() 
{
    
    bcm2835_init();
     

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    
    //LMIC_setSession (0x1, DEVADDR, (u1_t*)DEVKEY, (u1_t*)ARTKEY);
  
	LMIC_selectSubBand(7);
  
	// Disable data rate adaptation
	LMIC_setAdrMode(0);
	// Disable link check validation
	LMIC_setLinkCheckMode(0);
	
	
	// Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

void loop() 
{
      os_runloop_once();
 
      usleep(1000);
}

int main() {
  setup();

  while (1) {
    loop();
  }
  return 0;
}

