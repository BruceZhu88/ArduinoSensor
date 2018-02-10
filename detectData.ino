#include "pt.h"  //protothread
static struct pt pt1, pt2, pt3;

int timer1_counter;
int relayPort = 2; // port 2-5 for relay
int sensorSound = A2; // select the input pin for the potentiometer
int sensorLight1 = A0;
int sensorLight2 = A1;
int pos = 0;
int seconds = 0;
int detect_time = 0;
String time_delay = "";
String cmd = "";
String msg[3] = {"", "", ""};
String data_out = "";
String data_sound = "";
String data_light1 = "";
long int count = 0;
long int sound_count = 0;
long int light1_count = 0;

void setup() {
  // init
  noInterrupts(); //Disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  //timer1_counter = 34286; // timer1 : 65536-16MHz/256/2Hz (1/16000000)*256*(65536-34286) = 0.5
  timer1_counter = 3036;  //1s
  TCNT1 = timer1_counter; //load timer
  TCCR1B |= (1 << CS12); // 256 prescaler
  
  interrupts(); //enable interrupts
  TIMSK1 |= (1 << TOIE1);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  Serial.begin(115200);
  Serial.setTimeout(1000);
  //PT_INIT(&pt1);
  //PT_INIT(&pt2);
  //PT_INIT(&pt3);
}

ISR(TIMER1_OVF_vect) {
  TCNT1 = timer1_counter;
  if (seconds >= detect_time) {
      detect_time = 0;
      seconds = 0;
      msg[0] = "";
  }
  else {
      seconds++;
  }
  //digitalWrite(ledPin, digitalRead(ledPin)^1);  
}

void concat_str(String d1, int d2) {
    String outData = "";
    outData = d1;
    outData.concat(d2);
    Serial.println(outData);
}

static int print_data(struct pt *pt) {
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  if (msg[0] == "all") {
     if (sound_count == 50){
        Serial.println(data_sound);
        data_sound = "";
        sound_count = 0;
    }
    if (light1_count == 10){
        Serial.println(data_light1);
        data_light1 = "";
        light1_count = 0;
    }
    //timestamp = millis();
    //PT_WAIT_UNTIL(pt, millis() - timestamp > 0);
    //concat_str("1L", analogRead(sensorLight1));
  }
  PT_END(pt);
}


static int detect_data(struct pt *pt) {
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  if (msg[0] == "all") {
    //timestamp = millis();
    //PT_WAIT_UNTIL(pt, millis() - timestamp > 0);
    //concat_str("2L", analogRead(sensorLight2));
    /*
    if (msg[1] == "2"){
      data_out.concat(analogRead(sensorLight2));
      data_out.concat("|");
    }
    data_out.concat(analogRead(sensorLight1));
    data_out.concat("|");
    data_out.concat(analogRead(sensorSound));
    data_out.concat(",");
    count++;
    */
  }
  PT_END(pt);
}

void loop() {
  int i;
  while (Serial.available() > 0) {
      cmd += char(Serial.read());
      delay(2);
  }
  if (cmd != "") {
     i = 0;
     do{
        pos = cmd.indexOf(',');
        if(pos != -1) {
           msg[i] = cmd.substring(0,pos);
           cmd = cmd.substring(pos+1,cmd.length()); 
        }
        else {
          if(cmd.length()>0){
             msg[i] = cmd;
          }
        }
        i++;
     }
     while(pos>=0);
     detect_time = msg[2].toInt()-1;
     cmd = ""; 
  }
  
  if(msg[0] == "sound") {
      Serial.println(analogRead(sensorSound)); 
  }
  else if (msg[0] == "light") {
      if (msg[1].toInt() == 1) {
        Serial.println(analogRead(sensorLight1));
      }
      else if (msg[1].toInt() == 2){
        Serial.println(analogRead(sensorLight2));
      }
  }
  else if (msg[0] == "all") {
    concat_str("1L", analogRead(sensorLight1));
    concat_str("S", analogRead(sensorSound));
    if (msg[1] == "2"){
      concat_str("2L", analogRead(sensorLight2));
    }
    delay(1);
    //Serial.println(analogRead(sensorLight1));
    //Serial.println(analogRead(sensorSound));
    //lightValue  = digitalRead(hzPin);
  }
  else if (msg[0] == "relay") { //msg[0].startsWith("relay")
      time_delay = msg[2].toInt();
      relayPort = msg[1].toInt();
      digitalWrite(4,HIGH);
      delay(time_delay.toInt());
      digitalWrite(4,LOW);
      msg[0] = "";
    }
  else {
    
  }
     //detect_sound(&pt1);
     //detect_light1(&pt2);
     //print_data(&pt3);
     //detect_data(&pt1);
}
