#include <EtherCard.h>

//Author cristian658 

#define STATIC 1  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = {
  192,168,1,200 };
// gateway ip address
static byte gwip[] = {
  192,168,1,1 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[] = {
  0x74,0x69,0x69,0x2D,0x30,0x31 };


byte Ethernet::buffer[500]; // tcp/ip send and receive buffer
BufferFiller bfill;

int sensorPin = A0;
int sensorValue = 0; // moisture sensor

void setup(){
  Serial.begin(57600);
  Serial.println("\n[gardenSensorHardware]");

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println( "Failed to access Ethernet controller");
#if STATIC
  ether.staticSetup(myip, gwip);
#endif

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip); 
  ether.printIp("DNS: ", ether.dnsip); 
}
const char http_OK[] PROGMEM =
"HTTP/1.0 200 OK\r\n"
"Content-Type: application/json\r\n"
"Pragma: no-cache\r\n\r\n";

const char http_Found[] PROGMEM =
"HTTP/1.0 302 Found\r\n"
"Location: /\r\n\r\n";

const char http_Unauthorized[] PROGMEM =
"HTTP/1.0 401 Unauthorized\r\n"
"Content-Type: text/html\r\n\r\n"
"<h1>401 Unauthorized</h1>";

void sensorMoisture()
{
 
  bfill.emit_p(PSTR("HTTP/1.0 200 OK\r\nContent-Type: application/json\r\nPragma: no-cache\r\n\r\n"));
  bfill.emit_p(PSTR("{\"humedad\":\"$D\"}"), sensorValue);
}
void homeHumedadTemperaturaDTH()
{
 
  bfill.emit_p(PSTR("HTTP/1.0 200 OK\r\nContent-Type: application/json\r\nPragma: no-cache\r\n\r\n"));
  bfill.emit_p(PSTR("{\"humedad\":\"$D\"}"), sensorValue);
}
void loop(){
  // DHCP expiration is a bit brutal, because all other ethernet activity and
  // incoming packets will be ignored until a new lease has been acquired
  if (!STATIC && ether.dhcpLease()) {
    Serial.println("Acquiring DHCP lease again");
    ether.dhcpSetup();
  }
  
  Serial.print("sensor = " );
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);
  // wait for an incoming TCP packet, but ignore its contents
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  if (pos) {
    delay(1);   // necessary for my system
    bfill = ether.tcpOffset();
    char *data = (char *) Ethernet::buffer + pos;
    if (strncmp("GET /", data, 5) != 0) {
      // Unsupported HTTP request
      // 304 or 501 response would be more appropriate
      bfill.emit_p(http_Unauthorized);
    }
    else {
        sensorMoisture();
    }
    ether.httpServerReply(bfill.position());    // send http response
  }

}

