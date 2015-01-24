#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

#include "configuration.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed

#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.

uint32_t ip;

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

void draw(String command)
{
  if (command == "clear") {
    matrix.clear();
  }

  if (command == "countdown") {
    drawCountdown();
  }

  if (command == "red") {
    matrix.clear();
    matrix.fillRect(0,0, 8,8, LED_RED);
  }

  if (command == "green") {
    matrix.clear();
    matrix.fillRect(0,0, 8,8, LED_GREEN);
  }

  if (command == "yellow") {
    matrix.clear();
    matrix.fillRect(0,0, 8,8, LED_YELLOW);
  }

  matrix.writeDisplay();  // write the changes we just made to the display
}

void drawCountdown()
{
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextSize(1);
  matrix.setTextColor(LED_GREEN);
  matrix.setRotation(1);

  matrix.clear();
  matrix.setCursor(2,1);
  matrix.print("3");
  matrix.writeDisplay();
  delay(500);

  matrix.clear();
  matrix.setCursor(2,1);
  matrix.print("2");
  matrix.writeDisplay();
  delay(500);

  matrix.clear();
  matrix.setCursor(2,1);
  matrix.print("1");
  matrix.writeDisplay();
  delay(500);
}

//void rxCallback(uint8_t *buffer, uint8_t len)
//{
//  String command = "";
//  for(int i=0; i<len; i++) {
//   command = String(command + (char)buffer[i]);
//  }
//
//  draw(command);
//
//  debugMsg(buffer, len);
//  uart.write(buffer, len); // Echo the same data back via BLE
//}

void setup(void)
{
  Serial.begin(115200);
  while(!Serial); // Leonardo/Micro should wait for serial init

  matrix.begin(0x70);  // pass in the address

  draw("countdown");

  draw("red");
  delay(250);
  draw("yellow");
  delay(250);
  draw("green");
  delay(250);
  draw("clear");
  
  matrix.clear();
  matrix.drawPixel(0, 0, LED_RED);
  matrix.writeDisplay();
//  delay(500);
  
  connect();
}

void connect() {
    Serial.println(F("Hello, CC3000!\n")); 

  Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);
  
  /* Initialise the module */
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  
  // Optional SSID scan
  // listSSIDResults();
  
  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
  Serial.println(F("Connected!"));
  
  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  /* Display the IP address DNS, Gateway, etc. */  
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  ip = 0;
  // Try looking up the website's IP address
  Serial.print(WEBSITE); Serial.print(F(" -> "));
  while (ip == 0) {
    if (! cc3000.getHostByName(WEBSITE, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }

  cc3000.printIPdotsRev(ip);
  
  // Optional: Do a ping test on the website
  /*
  Serial.print(F("\n\rPinging ")); cc3000.printIPdotsRev(ip); Serial.print("...");  
  replies = cc3000.ping(ip, 5);
  Serial.print(replies); Serial.println(F(" replies"));
  */
}

void disconnect(void) {
  //  Serial.println(F("\n\nDisconnecting"));
  cc3000.disconnect();
}

void findNextPicture(void) {
  Serial.println(F("--- LOOKING FOR NEXT PICTURE ---"));
  
   /* Try connecting to the website.
     Note: HTTP/1.1 protocol is used to keep the server from closing the connection before all data is read.
  */
  Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 80);
  if (www.connected()) {
    www.fastrprint(F("GET "));
    www.fastrprint(WEBPAGE);
    www.fastrprint(F(" HTTP/1.1\r\n"));
    www.fastrprint(F("Host: ")); www.fastrprint(WEBSITE); www.fastrprint(F("\r\n"));
    www.fastrprint(F("\r\n"));
    www.println();
  } else {
    Serial.println(F("Connection failed"));    
    return;
  }

  Serial.println(F("-------------------------------------"));
  
  /* Read data until either the connection is closed, or the idle timeout is reached. */ 
  unsigned long lastRead = millis();
  while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (www.available()) {
      char c = www.read();
      Serial.print(c);
      lastRead = millis();
    }
  }
  www.close();
  Serial.println(F("-------------------------------------"));
}

void loop()
{
  findNextPicture();
  
  matrix.clear(); 
  
  int row = 0;
  int col = 0;
  char pic[] = "grgyrygrygygryrgyrgyrgyrgyyrgygyrgyrygyryrygyrygyrygyrgyrgyrgyry";
  char pic1[] = "0rgy0rgy0rgy0rgy0rgy0rgy0rgy0rgy0rgy0rgy0rgy0rgy0rgy0rgy0rgy0rgy";
  char pic2[] = "0rgyygr00rgyygr00rgyygr00rgyygr00rgyygr00rgyygr00rgyygr00rgyygr0";
  char smile[] = "00gggg000g0000g0g0g00g0gg000000gg0g00g0gg00gg00g0g0000g000gggg00";  

  char *current;
  switch (random(4)) {
    case 0:
      current = pic;
      break;
    case 1:
      current = pic1;
      break;
    case 2:
      current = pic2;
      break;
    case 3:
      current = smile;
      break;
  }

  for (int i = 0; i < 64; i++) {
     if (current[i] == 'r') {
       matrix.drawPixel(col, row, LED_RED);
     }
     if (current[i] == 'g') {
       matrix.drawPixel(col, row, LED_GREEN);
     }
     if (current[i] == 'y') {
       matrix.drawPixel(col, row, LED_YELLOW);
     }
     
     col++;
     if (col > 7) {
       col = 0;
       row++;
     }
  }
  matrix.writeDisplay();

  Serial.println(F("Waiting 20 seconds..."));
  delay(20000);

//  matrix.clear();
  matrix.drawPixel(0, 0, LED_YELLOW);
  matrix.writeDisplay();
//  delay(500);
}

void listSSIDResults(void)
{
  uint32_t index;
  uint8_t valid, rssi, sec;
  char ssidname[33]; 

  if (!cc3000.startSSIDscan(&index)) {
    Serial.println(F("SSID scan failed!"));
    return;
  }

  Serial.print(F("Networks found: ")); Serial.println(index);
  Serial.println(F("================================================"));

  while (index) {
    index--;

    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);
    
    Serial.print(F("SSID Name    : ")); Serial.print(ssidname);
    Serial.println();
    Serial.print(F("RSSI         : "));
    Serial.println(rssi);
    Serial.print(F("Security Mode: "));
    Serial.println(sec);
    Serial.println();
  }
  Serial.println(F("================================================"));

  cc3000.stopSSIDscan();
}

/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
