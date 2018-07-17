#include <EtherCard.h>

static byte myip[] = { 192,168,0,200 };
static byte gwip[] = { 192,168,0,1 };
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x30 };
byte Ethernet::buffer[700];
static uint32_t timer;

int led = 5;
int relay = 7;
int sink = 8;
int counter = -2;

// called when a ping comes in (replies to it are automatic)
static void gotPinged (byte* ptr) {
  ether.printIp(">>> ping from: ", ptr);
}

void setup () {
  Serial.begin(57600);
  Serial.println("\n[pings]");
  
  if (ether.begin(sizeof Ethernet::buffer, mymac, 9) == 0){
    Serial.println("Failed to access Ethernet controller");
  }
  
  ether.staticSetup(myip, gwip); 
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);

  // call this to report others pinging us
  ether.registerPingCallback(gotPinged);
  ether.parseIp(ether.hisip, "74.125.230.127");
  
  timer = 0;
  Serial.println();
  
  pinMode(led,OUTPUT);
  pinMode(relay,OUTPUT);
  pinMode(sink,OUTPUT);
  digitalWrite(sink,LOW);
}

void loop () {
  word len = ether.packetReceive(); // go receive new packets
  word pos = ether.packetLoop(len); // respond to incoming pings
  
  if(counter >= 3){
   Serial.println("Google onbereikbaar, router wordt gereset...");
   resetRouter();
  }
  
  // report whenever a reply to our outgoing ping comes back
  if (len > 0 && ether.packetLoopIcmpCheckReply(ether.hisip)) {
    Serial.print("  ");
    Serial.print((micros() - timer) * 0.001, 3);
    Serial.println(" ms");
    counter--;
  }
  
  // ping a remote server once every few seconds
  if (micros() - timer >= 1000000) {
    digitalWrite(led,HIGH);
    delay(100);
    digitalWrite(led,LOW);
    Serial.println("Failed pings: " + String(counter));
    ether.printIp("Pinging: ", ether.hisip);
    timer = micros();
    ether.clientIcmpRequest(ether.hisip);
    counter++;
  }
}

void resetRouter(){
 digitalWrite(led,HIGH);
 digitalWrite(relay,HIGH);
 delay(1000);
 digitalWrite(relay,LOW);
 for(int i = 1;i<70;i++){
  digitalWrite(led,HIGH);
  delay(125);
  digitalWrite(led,LOW);
  delay(125); 
 }
 counter = 0; 
}
