#include <EtherCard.h>
#include <HttpRest.h>

byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

const char website[] PROGMEM = "192.168.1.213";
const char mediaTypeJSON[] PROGMEM = "application/json";

static byte session;

byte Ethernet::buffer[700];
Stash stash;

void sendDataToServer (EtherCard* ether, char url[], char key[], int value) {
	Serial.println("Enviando datos...");
	byte sd = stash.create();
	stash.print("{");
  stash.print(key);
  stash.print(":");
  stash.print(value);
  stash.print("}");
	stash.save();
	int stash_size = stash.size();
	
	Stash::prepare(PSTR("POST http://$F/$F HTTP/1.0" "\r\n"
    "Host: $F" "\r\n"
    "Content-Length: $D" "\r\n"
    "Content-Type: $F\r\n"
    "\r\n"
    "$H"),
		website, url, website, stash_size, mediaTypeJSON, sd);
		session = ether.tcpSend();
}
//Imprime respuesta
static void response(EtherCard* ether) {
	ether.packetLoop(ether.packetReceive());

  const char* reply = ether.tcpReply(session);
  if (reply != 0) {
    Serial.println("Got a response!");
    Serial.println(reply);
	}
}


