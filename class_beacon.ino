#include <SPI.h>
#include <RF22.h>
#include <RF22Router.h>
#include <string.h>
//temperature and humidity sensor
#include <dht.h>
#define DHT11_PIN 7
dht DHT;

#define SOURCE_ADDRESS1 1 //all student-users connected to the class beacon
#define SOURCE_ADDRESS2 2
#define SOURCE_ADDRESS3 3
#define SOURCE_ADDRESS4 4
#define SOURCE_ADDRESS5 6
#define SOURCE_ADDRESS6 8
#define SOURCE_ADDRESS7 27
#define DESTINATION_ADDRESS 24 //beacon address

RF22Router rf22(DESTINATION_ADDRESS);

RF22Router rf22(SOURCE_ADDRESS1);
RF22Router rf22(SOURCE_ADDRESS2);
RF22Router rf22(SOURCE_ADDRESS3);
RF22Router rf22(SOURCE_ADDRESS4);
RF22Router rf22(SOURCE_ADDRESS5);
RF22Router rf22(SOURCE_ADDRESS6);
RF22Router rf22(SOURCE_ADDRESS7);

int initial_time;
int max_counter = 100;
int student_n = 0;
int i;
int students[7];
int current_time;
int received_value = 0;

void environment_conditions();

void setup() {
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  if (!rf22.setFrequency(431.0))//θέτω την συχνότητα
    Serial.println("setFrequency Fail");
  rf22.setTxPower(RF22_TXPOW_20DBM);
  //1,2,5,8,11,14,17,20 DBM
  rf22.setModemConfig(RF22::OOK_Rb40Bw335  );
  //modulation

  // Manually define the routes for this network
  rf22.addRouteTo(SOURCE_ADDRESS1, SOURCE_ADDRESS1);
  rf22.addRouteTo(SOURCE_ADDRESS2, SOURCE_ADDRESS2);
  rf22.addRouteTo(SOURCE_ADDRESS3, SOURCE_ADDRESS3);
  rf22.addRouteTo(SOURCE_ADDRESS4, SOURCE_ADDRESS4);
  rf22.addRouteTo(SOURCE_ADDRESS5, SOURCE_ADDRESS5);
  rf22.addRouteTo(SOURCE_ADDRESS6, SOURCE_ADDRESS6);
  rf22.addRouteTo(SOURCE_ADDRESS7, SOURCE_ADDRESS7);
  rf22.addRouteTo(DESTINATION_ADDRESS, DESTINATION_ADDRESS);
}



void loop() {
  
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  uint8_t len = sizeof(buf);
  uint8_t from;
  
  //recvfromAck checks if message received
  if (rf22.recvfromAck(buf, &len, &from))
  {
    buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
    
    Serial.print("got request from : ");
    Serial.println(from, DEC);
    
    received_value = atoi((char*)incoming);
    if (students[from - 1] == 0) {
      student_n = student_n + 1;
      students[from - 1] = 1;
    }

  }
  current_time = millis() - initial_time;
  if (current_time >= 60000) // 60000ms = 1 minute
  {
    environment_conditions();
    initial_time = millis();
  }
}


void environment_conditions() {
  int chk = DHT.read11(DHT11_PIN);
  float sensorValue;
  sensorValue = analogRead(A1);

  int count_success=0;
  int counter;
  char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  const char s[2] = "-";
  char *token;
  char data_read1[8];
  char data_read2[8];
  char data_read3[8];
  
  sprintf(data_read1,"%d",DHT.temperature);
  sprintf(data_read2,"%d",DHT.humidity);
  sprintf(data_read3,"%d",sensorValue);
  
  char dest[50];
  strcat(dest,data_read1);
  strcat(dest,s);
  strcat(dest,data_read2);
  strcat(dest,s);
  strcat(dest,data_read3);
  memcpy(data_send,dest,RF22_ROUTER_MAX_MESSAGE_LEN);
  
  for (counter = 0; counter < max_counter; counter++)
  {
    if (rf22.sendtoWait(data_send, sizeof(data_send), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE)
    {
      Serial.println("sendtoWait failed");
    }
    else
    {
      Serial.println("sendtoWait Successful");
      if (count_success = student_n) {
        break;
      }
    }
}
