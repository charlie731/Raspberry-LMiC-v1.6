Installing for Raspberry PI

You need 3 dependencies:

build essential package
 apt-get install build-essential
other tools packages
 apt-get install git-core wget
bcm2835_library see installation

// Example program showing how to detect multiple module                                                                   // RH_RF69/RH_RF95 on Raspberry Pi
// Requires bcm2835 library to be already installed
// http://www.airspayce.com/mikem/bcm2835/
// Use the Makefile in this directory:
// cd example/raspi/ttn-abp
// make
// sudo ./ttn-abp
//
// Will check for RFM92/95/96/98 or RFM69/RFM69HCW/RFM69W modules on SPI BUS
