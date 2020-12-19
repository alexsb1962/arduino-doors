#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
//#include <Timer.h>

const char* ssid = "theflat";
const char* password = "sheludko";
 

WiFiUDP Udp;
unsigned int localUdpPort   = 54545;  // local port to listen on
unsigned int remouteUdpPort = 54546;  // приемный порт пульта для ответов
char incomingPacket[128];  // buffer for incoming packets
char  replyPacketOk[] = "ok";  // a reply string to send back
char replayPacketName[] = "doors";

void blink( int num){
  for(int i=0;i<num;i++){
    digitalWrite(LED_BUILTIN, LOW);   
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);   
    delay(200);
  }
  delay(200);
}


void wifi_event_handler_cb(System_Event_t * event) {
  //ehConsolePort.print(EVENT_NAMES[event->event]);
  //ehConsolePort.print(" (");

  switch (event->event) {
    case EVENT_STAMODE_CONNECTED:
      break;
    case EVENT_STAMODE_DISCONNECTED:
      break;
    case EVENT_STAMODE_AUTHMODE_CHANGE:
      break;
    case EVENT_STAMODE_GOT_IP:
      break;
    case EVENT_SOFTAPMODE_STACONNECTED:
    case EVENT_SOFTAPMODE_STADISCONNECTED: {
        char mac[32] = {0};
    //    snprintf(mac, 32, MACSTR ", aid: %d", MAC2STR(event->event_info.sta_connected.mac), event->event_info.sta_connected.aid);

      //  ehConsolePort.print(mac);
      }
      break;
  }

  //ehConsolePort.println(")");
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);    
  pinMode(LED_BUILTIN, OUTPUT);  
  pinMode(D2, OUTPUT);  
//  digitalWrite(, HIGH);   
  blink(3);blink(3);blink(3);blink(1);
  
  Serial.begin(115200);
  Serial.println();
  for(int i =0;i<20; i++){
    Serial.printf("Проверка связи с компьютером. Строка № %d \n",i+1);
  }

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
  }
//  wifi_set_event_handler_cb(wifi_event_handler_cb);

  Serial.println(" connected");
  blink(2);

  Udp.begin(localUdpPort);
  //udp.beginMulticast(WiFi.localIP(), multicast_ip_addr, localUdpPort)
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}


void loop(){

  if (WiFi.status() != WL_CONNECTED)  {
    // перезапуск без разговоров при пропадании сети
    Serial.println("Нет сети. Перезагрузка......"); 
    ESP.restart();
  }
  
  int packetSize = Udp.parsePacket();
  if (packetSize)  {
    digitalWrite(LED_BUILTIN, LOW); delay(10); digitalWrite(LED_BUILTIN, HIGH);   

    // receive incoming UDP packets
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0) incomingPacket[len] = 0;
    
    //Serial.printf("UDP packet contents: %s\n", incomingPacket);

    if(strcmp("IsSomebodyHere",incomingPacket) == 0){
      Udp.beginPacket( Udp.remoteIP(), remouteUdpPort );
      Udp.write( replayPacketName );
      Udp.endPacket();
      return ;
    }
    
    if(strcmp("press",incomingPacket) == 0){
//      Serial.println("press  resived");
      Udp.beginPacket( Udp.remoteIP(), remouteUdpPort );
      Udp.write( replyPacketOk );
      Udp.endPacket();
      // включаем ( выключение по таймеру)
      digitalWrite(D2,LOW);digitalWrite(LED_BUILTIN, LOW);
      delay(400);
      digitalWrite(D2,HIGH);digitalWrite(LED_BUILTIN, HIGH);
    }

  }
}
