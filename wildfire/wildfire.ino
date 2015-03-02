/* 
  This a simple example of the REST Library for Arduino (Uno/Mega/Due/Teensy)
  using the CC3000 WiFi chip. See the README file for more details.
 
  Written in 2014 by Marco Schwartz under a GPL license. 
*/

#define DEBUG_MODE 0

#define NUMBER_VARIABLES 2
#define NUMBER_FUNCTIONS 1

// Import required libraries
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <CC3000_MDNS.h>
#include <aREST.h>
#include <avr/wdt.h>

// These are the pins for the CC3000 chip
#define ADAFRUIT_CC3000_IRQ   22
#define ADAFRUIT_CC3000_VBAT  23
#define ADAFRUIT_CC3000_CS    21

// Create CC3000 instance
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT);

// Create aREST instance
aREST rest = aREST();

// Your WiFi SSID and password                                         
#define WLAN_SSID       "your_wifi_name"
#define WLAN_PASS       "your_wifi_password"
#define WLAN_SECURITY   WLAN_SEC_WPA2

// The port to listen for incoming TCP connections 
#define LISTEN_PORT           80

// Server instance
Adafruit_CC3000_Server restServer(LISTEN_PORT);

// DNS responder instance
MDNSResponder mdns;

// Variables to be exposed to the API
int temperature;
int humidity;

void setup(void)
{  
  // Start Serial
  Serial.begin(9600);
  
  // Init variables and expose them to REST API
  rest.variable("temperature",&temperature);
  rest.variable("humidity",&humidity);
  temperature = 22;
  humidity = 39;
 
  rest.function("led",ledControl);
    
  // Give name and ID to device
  rest.set_id("1");
  rest.set_name("wildfire");
  
  // Set up CC3000 and get connected to the wireless network.
  Serial.println("CC3000 init ...");
  if (!cc3000.begin())
  {
    while(1);
  }
  
  Serial.println("CC3000 init OK");
  
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    while(1);
  }
  while (!cc3000.checkDHCP())
  {
    delay(100);
  }
  
  // Start multicast DNS responder
  if (!mdns.begin("wildfire", cc3000)) {
    while(1); 
  }
   
  // Start server
  restServer.begin();
  Serial.println(F("Listening for connections..."));
  
  displayConnectionDetails();
  
  wdt_enable(WDTO_4S);
  
}

void loop() {
  
    
  // Handle any multicast DNS requests
  mdns.update();
  
  // Handle REST calls
  Adafruit_CC3000_ClientRef client = restServer.available();
  rest.handle(client);
  wdt_reset();
  
  // Check connection
  if(!cc3000.checkConnected()){while(1){}}
  wdt_reset();
 
}

int ledControl(String command) {
  
  // Get state from command
  int state = command.toInt();
 
  digitalWrite(6,state);
  return 1;
}

// Print connection details of the CC3000 chip
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
