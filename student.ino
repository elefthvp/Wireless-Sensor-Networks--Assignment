#include <SPI.h>
#include <RF22.h>
#include <RF22Router.h>
#include <string.h>
//RF22_ROUTER_MAX_MESSAGE_LEN= 50
#define SOURCE_ADDRESS 27 // student
#define DESTINATION_ADDRESS 24 //class beacon
RF22Router rf22(SOURCE_ADDRESS);

int UpperThreshold = 518;
int LowerThreshold = 490;
int reading = 0;
float BPM = 0.0;
bool IgnoreReading = false;
bool FirstPulseDetected = false;
unsigned long FirstPulseTime = 0;
unsigned long SecondPulseTime = 0;
unsigned long PulseInterval = 0;

void environment_conditions();
void transmit_heart();

void setup() {
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  if (!rf22.setFrequency(431.0))
    Serial.println("setFrequency Fail");
  rf22.setTxPower(RF22_TXPOW_20DBM);
  //1,2,5,8,11,14,17,20 DBM
  rf22.setModemConfig(RF22::OOK_Rb40Bw335  );
  // Manually define the routes for this network
  rf22.addRouteTo(DESTINATION_ADDRESS, DESTINATION_ADDRESS);
  rf22.addRouteTo(SOURCE_ADDRESS, SOURCE_ADDRESS);

  //heart read prep
   pinMode(10, INPUT); // Setup for leads off detection LO +
   pinMode(12, INPUT); // Setup for leads off detection LO -
   char *sense="request to send";
}


void loop() {
  char msg[]="get ready for classroom conditions";
  
  reading = analogRead(0); 
  if(reading > UpperThreshold && IgnoreReading == false){
    if(FirstPulseDetected == false){
      FirstPulseTime = millis();
      FirstPulseDetected = true;
    }
    else{
      SecondPulseTime = millis();
      PulseInterval = SecondPulseTime - FirstPulseTime;
      FirstPulseTime = SecondPulseTime;
    }
    IgnoreReading = true;
  }
  if(reading < LowerThreshold){
    IgnoreReading = false;
  }  
  BPM = (1.0/PulseInterval) * 60.0 * 1000;
  transmit_heart(BPM);
  
  
  // There should be a message now
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  uint8_t len = sizeof(buf); //50 bytes
  uint8_t from;
  if (rf22.recvfromAck(buf, &len, &from))
  {
    buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
    Serial.print("got request from : ");
    Serial.println(from, DEC); //dec=decimal
    if(!strcmp(incoming,msg)){ //strcmp returns 0 if incoming="get ready for classroom conditions"
      environment_conditions();
      }
     }
 }

void environment_conditions(){
  float temperature;
  float humidity;
  float lighting;
  
  string temp;
  string hum;
  string light;
  
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  uint8_t len = sizeof(buf);
  uint8_t from;
  memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  
  if (rf22.recvfromAck(buf, &len, &from)){
      buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
      memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
      for(i=1;i<=23;i++){
        if(i<=7)
          temp = strcat(temp,incoming[i])
        else if(i<=15){
          hum = strcat(hum,incoming[i])
        }
        else{
          light = strcat(light,incoming[i])
        }
  }
      
  temperature=atof((char*)temp);
  humidity=atof((char*)hum);
  lighting=atof((char*)lighting);

  Serial.println("Classroom temperature is:");
  Serial.println(temperature);
  Serial.println("Classroom humidity is:");
  Serial.println(humidity);
  Serial.println("Classroom lighting is:");
  Serial.println(lighting);

  return;
}


void transmit_heart(float BPM){
  int counter=0;
  bool success=false;
  char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(data_read, '\0', RF22_ROUTER_MAX_MESSAGE_LEN); //fill an array with \0, size of array is RF22_ROUTER_MAX_MESSAGE_LEN
  memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  sprintf(data_read, "%d", BPM);
  data_read[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
  memcpy(data_send, data_read, RF22_ROUTER_MAX_MESSAGE_LEN);
  while(success==false && counter<=10)
  if (rf22.sendtoWait(sense, sizeof(sense), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE){
      Serial.println("sendtoWait failed");
      counter=counter+1;
  }
  else{
      Serial.println("sendtoWait Successful");
      if (rf22.sendtoWait(data_send, sizeof(data_send), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE){
       Serial.println("sendtoWait failed");
      }
      else{
       Serial.println("sendtoWait Successful");
       success=true;
      }
      delay(1000)
  }
}
