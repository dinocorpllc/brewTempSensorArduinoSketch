#include <Event.h>
#include <Timer.h>

#include <aws_iot_mqtt.h>
#include <aws_iot_version.h>
#include "aws_iot_config.h"
#include <math.h>

int a;
float temperature;
int B=3975;                  //B value of the thermistor
float resistance;
int count = 0;

aws_iot_mqtt_client myClient;
char JSON_buf[100];
char float_buf[5];
bool success_connect = false;

bool print_log(char* src, int code) {
  bool ret = true;
  if(code == 0) {
    Serial.print("[LOG] command: ");
    Serial.print(src);
    Serial.println(" completed.");
    ret = true;
  }
  else {
    Serial.print("[ERR] command: ");
    Serial.print(src);
    Serial.print(" code: ");
    Serial.print(" code: ");
    Serial.println(code);
    ret = false;
  }
  return ret;
}


void sendTempToIoT(){
  a=analogRead(0);
  resistance=(float)(1023-a)*10000/a; //get the resistance of the sensor;
  temperature=1/(log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet&nbsp;;
  Serial.print("Current temperature is ");
  Serial.println(temperature);

  dtostrf(temperature, 4, 1, float_buf);
  float_buf[4] = '\0';

  sprintf(JSON_buf, "{ \"Operation\" : \"put\", \"TableName\" : \"BrewTempSensor\", \"Item\" : { \"Temp\":%s } }", float_buf);
  myClient.publish("/sensor/temperature", JSON_buf, strlen(JSON_buf), 0, false); // publish "myMessage" to topic "myTopic" in QoS 0 with retain flag set to false
//  sprintf(JSON_buf, "{ \"poop\" : \"poop\" }");
//  myClient.publish("/bananaaa", JSON_buf, strlen(JSON_buf), 0, false); // publish "myMessage" to topic "myTopic" in QoS 0 with retain flag set to false
  
//  if(myClient.yield()) {
//    Serial.println("Yield failed.");
//  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  
  char curr_version[80];
  sprintf(curr_version, "AWS IoT SDK Version(dev) %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);
  
  Serial.println(curr_version);

  if(print_log("setup", myClient.setup(AWS_IOT_CLIENT_ID))) {
    if(print_log("config", myClient.config(AWS_IOT_MQTT_HOST, AWS_IOT_MQTT_PORT, AWS_IOT_ROOT_CA_PATH, AWS_IOT_PRIVATE_KEY_PATH, AWS_IOT_CERTIFICATE_PATH))) {
      if(print_log("connect", myClient.connect())) {
        success_connect = true;
      }
    }
  }
}



void loop() {
  if(success_connect) {
    sendTempToIoT();
  }
  delay(9000);
}
