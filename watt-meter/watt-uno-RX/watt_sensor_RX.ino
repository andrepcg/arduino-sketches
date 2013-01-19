#include <SPI.h>
#include <Ethernet.h>
#include <MANCHESTER.h>


EthernetClient client;
byte mac[] = {  0x90, 0xA2, 0xDA, 0x0D, 0x96, 0xFE };

char server[] = "unplu.gg";

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
  if (client.connect(server, 80)){    
    
	if (client.connected()){	
		Serial.println("Connecting to unplugg...");	
		client.print("GET \n");
		client.print("/unplugg.php?pulsos=");
		client.print(count);
		client.println();
		
		lastConnectionTime = millis();
    
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

}

void startEthernet(){
  Serial.println("Connecting Arduino to network...");
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
