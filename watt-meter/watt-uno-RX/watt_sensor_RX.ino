#include <SPI.h>
#include <Ethernet.h>
#include <MANCHESTER.h>

#define PULSEkWH 75

EthernetClient client;
byte mac[] = {  0x90, 0xA2, 0xDA, 0x0D, 0x96, 0xFE };

//IPAddress server(127,0,0,1); //localhost
char unpluggAddress[] = "unplu.gg";
String unpluggAPIToken = "5LB3dFwTpRXq454QmdYX";
String meterID = "50f9549fd339be09a5000019";

boolean lastConnected = false;
int failedCounter = 0;
long lastConnectionTime = 0; 

void setup() {
  Serial.begin(9600);
  
  startEthernet();
  
  MANRX_SetRxPin(4);
  MANRX_SetupReceive();
  MANRX_BeginReceive();
}

void loop(){
  
  if (client.available()){
    char c = client.read();
    Serial.print(c);
  }

  // Disconnect
  if (!client.connected() && lastConnected){
    Serial.println("...disconnected");
    Serial.println();
    
    client.stop();
  }
  
  unsigned int data = receberRX();
  if(data > 0)
    enviarUnplugg(data);
  
  if (failedCounter > 3 )
    startEthernet();
  
  lastConnected = client.connected();
  
  delay(1000);
}

unsigned int receberRX(){
  if (MANRX_ReceiveComplete()) {
    Serial.println("Recebido");
    unsigned int data = MANRX_GetMessage();
    MANRX_BeginReceive();
    
    return data;
  }
}

void enviarUnplugg(unsigned int count){
  if (client.connect(unpluggAddress, 80)){    
    
    String data = "auth_token="+unpluggAPIToken;    
          data += "&meter_id="+meterID;
          data += "&consumption_value="+doubleToString(pulse2watt(count),8);
          
    client.print("POST /consumptions.json HTTP/1.0\n");
    client.print("Host: unplu.gg\n");
    client.print("Content-Type: application/json\n");
    client.print("Content-Length: ");
    client.print(data.length());
    client.print("\n\n");
    client.print(data);
    
    lastConnectionTime = millis();
    
    if (client.connected()){
      Serial.println("Connecting to unplugg...");
      Serial.println();
      failedCounter = 0;
    }
    else{
      failedCounter++;
      Serial.println("Connection to unplugg failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
    }
    
  }
  else{
    failedCounter++;
    
    Serial.println("Connection to unplugg Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    
    lastConnectionTime = millis(); 
  }
  client.stop();
}

double pulse2watt(unsigned int count){
  return ((double)count/PULSEkWH)*1000;
}

String doubleToString(double input,int decimalPlaces){
  char tmp[10];
  dtostrf(input,1,decimalPlaces,tmp);
  return String(tmp);
}

void startEthernet(){
  client.stop();

  Serial.println("Connecting Arduino to network...");
  Serial.println();  

  delay(1000);
  
  // Connect to network amd obtain an IP address using DHCP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("DHCP Failed, reset Arduino to try again");
    Serial.println();
  }
  else
  {
    Serial.print("Arduino connected (");
    Serial.print(Ethernet.localIP());
    Serial.println(")");
  }
  
  delay(1000);
}
