#include <FlexCAN.h>
#include <EEPROM.h>
#include <HX711.h>
#include "Faulcan.h"

Faulcan mot1(5);// flexion
Faulcan mot2(4);// extension_mid
Faulcan mot3(3);// extension_proximal

// loadcell parameters
HX711 scale;
HX711 scale2;
HX711 scale3;

uint8_t dataPin = 15;
uint8_t clockPin = 16;

uint8_t dataPin2 = 17;//
uint8_t clockPin2 = 18;//

uint8_t dataPin3 = 19;//
uint8_t clockPin3 = 20;//


uint32_t start, stop;

//int flx_th = 100;
//int max_flx_th = 300;
//int flx2_th = 0;
//int max_flx2_th = 300;
//int ex2_th = 0;
//int max_ex2_th = 300;
//

int LED_sync = 2;

// actuation value definition
int pos;
int pos2;
int pos3;
int stop_pos;
int stop_pos2;
int stop_pos3;

int state = 3;
char state_v;
int repeat;

int rr = 3;

// tuning parameter:
//setting max,min stroke limitation 
int max1=40000;//flexion limit stroke
int min1=0;
int min1_init = 3000;


int max2 =10000;//flexion
int max2_init = 0;
int min2 = 0;
int min2_init = 1500;

int max3 =18000;//extension proximal limit stroke
int max3_init = 9000;
int min3 = 0;

//set maximum velocity of motor
int velM = 4800;
int velM2 = 2000;
int velM3 = 2000;

int tv = 1;

//P-control Value

int vel1;
int vel2;
int vel3;

float error1;
float error2;
float error3;

volatile float load_flx;
volatile float load_flx2;
volatile float load_ex2;

float flx_th = 1500;
float flx2_th = 300;
float ex2_th = 600;


double flx_th_step;
double flx2_th_step;
double ex2_th_step;

double grad_flx = 1;
double grad_flx2 = 1;
double grad_ex2 = 1;

unsigned long currentTime, previousTime;
double elapsedTime;
double cumerror1,cumerror2,cumerror3;
double rateerror1,rateerror2,rateerror3;
double lasterror1, lasterror2, lasterror3;

void PD_con(){


  float K_p = 1.2; // p-gain for PID-Control
  float K_i = 0; // I-gain for PID-Control
  float K_d = 0.5; // D-gain for PID-Control
  
  double re1;
  double re2;
  double re3;
  double re4;
  double re5;
  double re6;
  
  if(flx_th_step < flx_th){
    flx_th_step = flx_th_step + grad_flx;  
  }
  else if( flx_th_step > flx_th){
    flx_th_step = flx_th;
  }
  
  if(flx2_th_step < flx2_th){ 
    flx2_th_step = flx2_th_step + grad_flx2;
  }
  else if( flx2_th_step > flx2_th){
    flx2_th_step = flx2_th;
  }
  
  if(ex2_th_step < ex2_th){
    ex2_th_step = ex2_th_step + grad_ex2;
  }
  else if( ex2_th_step > ex2_th){
    ex2_th_step = ex2_th;
  }
  
  
  
  error1 = flx_th_step - load_flx;
  error2 = flx2_th_step - load_flx2;
  error3 = ex2_th_step - load_ex2;
  
  
  
  currentTime = millis();
  elapsedTime = (double)(currentTime-previousTime)*0.001;
  cumerror1 += error1 * elapsedTime;
  cumerror2 += error2 * elapsedTime;
  cumerror3 += error3 * elapsedTime;
  
  rateerror1 = (error1-lasterror1) / elapsedTime;
  rateerror2 = (error2-lasterror2) / elapsedTime;
  rateerror3 = (error3-lasterror3) / elapsedTime;
  
  if ( abs(2.4* K_p*error1 + K_i * cumerror1 + K_d * rateerror1) < velM){
    vel1 = (int)abs(2.4*  K_p*error1 + K_i * cumerror1 + K_d * rateerror1); 
  }
  else {
    vel1 = velM;
  }
  
  if (abs(K_p*error2+ K_i * cumerror2 + K_d * rateerror2)< velM2){
    vel2 = (int)abs( K_p*error2+ K_i * cumerror2 + K_d * rateerror2); 
  }
  else {
    vel2 = velM2;
  }
  
  if (abs(K_p*error3 + K_i * cumerror3 + K_d * rateerror3) < velM3){
    vel3 = (int)abs( K_p*error3 + K_i * cumerror3+ K_d * rateerror3); 
  }
  else {
    vel3 = velM3;
  }
  previousTime = currentTime;
  lasterror1 = error1;
  lasterror2 = error2;
  lasterror3 = error3;

}


// time check
int start_timing = 0;
int standard = 0;
int counter = 1;
int data_on = 0;




void flexion() {
  
  if (state == 0) 
  {
    if ((max1 - pos) >= 0 && load_flx < flx_th)
    {
      mot1.Faul_velset(vel1);
      pos = mot1.Faul_abs_pos_rw(max1);
    }
    else if ((pos - min1) >= 0 && load_flx > flx_th)
    {
      mot1.Faul_velset(vel1);
      pos = mot1.Faul_abs_pos_rw(min1);
    }
    else
    {
      pos = mot1.Faul_abs_pos_r();
    }
    //Serial.print(limit_max);
  }
  else if (state == 1)
  {
    if (abs(min1_init - pos) > 1)
    {
      mot1.Faul_velset(velM);
      pos = mot1.Faul_abs_pos_rw(min1_init);

    }
    else
    {
      pos = mot1.Faul_abs_pos_r();
    }
  }
  else if (state == 2)
  {
    stop_pos =  mot1.Faul_abs_pos_r();
    pos = mot1.Faul_abs_pos_rw(stop_pos);
  }
  else if (state == 3)
  {
    mot1.Faul_velset(velM);
    pos = mot1.Faul_abs_pos_rw(0);
  }
}

void flexion_2() {
  if (state == 1)
  {
    if (pos2 - min2_init > 1)
    {
      mot2.Faul_velset(velM2);
      pos2 = mot2.Faul_abs_pos_rw(min2_init);
    }
    else
    {
      pos2 = mot2.Faul_abs_pos_r();

    }
    //Serial.print(limit_max);
  }
  else if (state == 0)
  {
    if ((pos2 - min2) >= 0 && load_flx2 > flx2_th)
    {
      
      mot2.Faul_velset(vel2);
      pos2 = mot2.Faul_abs_pos_rw(min2);   
    }
    else if ( max2 - pos2 >= 0 && load_flx2 < flx2_th)
    {
      
      mot2.Faul_velset(vel2);
      pos2 = mot2.Faul_abs_pos_rw(max2);   
    }        
    else
    {
      pos2 = mot2.Faul_abs_pos_r();
    }
  }
 
  else if (state == 2)
  {
    stop_pos2 =  mot2.Faul_abs_pos_r();
    pos2 = mot2.Faul_abs_pos_rw(stop_pos2);
  }
     else if (state == 3)
  {
    mot2.Faul_velset(velM2);
    pos2 = mot2.Faul_abs_pos_rw(0);
  }
}

void extension_2() {
  if (state == 1)
  {
    if (max3 - pos3 > 1)
    {
      mot3.Faul_velset(velM3);
      pos3 = mot3.Faul_abs_pos_rw(max3_init);
    }  
    else
    {
      pos3 = mot3.Faul_abs_pos_r();
    }
    //Serial.print(limit_max);
  }
  else if (state == 0)
  {
    if (pos3-min3 >= 0 && load_ex2 > ex2_th)
    {
     
      mot3.Faul_velset(vel3);
      pos3 = mot3.Faul_abs_pos_rw(min3);
    }
    else if (max3 - pos3 >= 0 && load_ex2 < ex2_th)
    {
      
      mot3.Faul_velset(vel3);
      pos3 = mot3.Faul_abs_pos_rw(max3);
    }
    else
    {
      pos3 = mot3.Faul_abs_pos_r();
    }
  }
  else if (state == 2)
  {
    stop_pos3 =  mot3.Faul_abs_pos_r();
    pos3 = mot3.Faul_abs_pos_rw(stop_pos3);
  }
    else if (state == 3)
  {
    mot3.Faul_velset(velM3);
    pos3 = mot3.Faul_abs_pos_rw(0);
  }
}

char input;

void state_change() {
  if (Serial.available())
  {
    input = Serial.read();
    char enter = Serial.read(); // \n value

    if (input == 'a') //flexion
    {
      state = 0;
      cumerror1 = 0;
      cumerror2 = 0;
      cumerror3 = 0;
    }
    else if (input == 's') //back to initial
    {
      state = 1;
    }
    else if (input == 'w'){
      state = 2; //Emergency Stop 
    }
    else if (input == 'q'){
      state = 3; //quit the exp mode 
      cumerror1 = 0;
      cumerror2 = 0;
      cumerror3 = 0;
    }
    else if (input == 'u'){
      flx_th = flx_th + 10;
    }
    else if (input == 'i'){
      flx_th = flx_th - 10;
    }
    else if (input == 'j'){
      flx2_th = flx2_th + 10;
    }
    else if (input == 'k'){
      flx2_th = flx2_th - 10;
    }
    else if (input == 'n'){
      ex2_th = ex2_th + 10;
    }
    else if (input == 'm'){
      ex2_th = ex2_th - 10;
    }
  }
}

double re1;
double re2;
double re3;
double re4;
double re5;
double re6;

void repeat_fe(){
    re1 = abs(flx_th - load_flx);
    re2 = abs(flx2_th - load_flx2);
    re3 = abs(ex2_th - load_ex2);
    re4 = abs(pos - min1_init);
    re5 = abs(pos2 - min2_init);
    re6 = abs(max3_init - pos3);
    if (state == 0 && re1 < 15 && re2 < 15 && re3 < 15) // state1 = flexion, motor2 flexor
    {
      data_on =1;//data sync signal on
      digitalWrite(LED_sync, HIGH);
      
      }
    else if (state == 1 && re4 < 5 && re5 < 5 && re6 < 5) // state1 = flexion, motor2 flexor
    {
      // data sync signal off
      flx_th_step = load_flx;
      flx2_th_step = load_flx2;
      ex2_th_step = load_ex2;
      delay(500);
//      if (rr==3){
//          flx_th_step = load_flx + grad_flx;
//          flx2_th_step = load_flx2 + grad_flx2;
//          ex2_th_step = load_ex2 + grad_ex2;
//      }
      state = 0;
      rr = 0;
     


     }
}


// hold on the posture
void delay_timer(){
  if (data_on == 1){
    if (tv == 100)
  {
    data_on = 0;
    digitalWrite(LED_sync, LOW);
    tv = 1;
    state = 1;
    rr = 1;
  }
    else{
     delay(30);
      tv = tv + 1;
    }
 }

}

int ran1;
int ran2;
int ran3;



void randomize(){
  if (rr == 0){
      ran1 = random(0, 15);
      ran2 = random(0, 15);
      ran3 = random(0, 15);

      
      flx_th = ran1 * 50;
      flx2_th = ran2 * 50;
      ex2_th = ran3 * 50;

      flx_th_step = load_flx + grad_flx;
      flx2_th_step = load_flx2 + grad_flx2;
      ex2_th_step = load_ex2 + grad_ex2;
      rr = 1;
      
      }
  }





void print_status() {
  Serial.print(state);
  Serial.print(", ");
  Serial.print(rr);
  Serial.print(", ");
  Serial.print(pos);
  Serial.print(", ");
  Serial.print(load_flx);
  Serial.print(", ");
  Serial.print(flx_th);
  Serial.print(", ");
  Serial.print(pos2);
  Serial.print(", ");
  Serial.print(load_flx2);
  Serial.print(", ");
  Serial.print(flx2_th);
  Serial.print(", ");
  Serial.print(pos3);
  Serial.print(", ");
  Serial.print(load_ex2);
  Serial.print(", ");
  Serial.print(ex2_th);
  Serial.print(", ");
  Serial.print(error1);
  Serial.print(", ");
  Serial.print(error2);
  Serial.print(", ");
  Serial.print(error3);
  Serial.print(", ");
//  Serial.print(tv);
//  Serial.print(", ");
  Serial.println(data_on);
//  Serial.print(data_on);
//  Serial.print(", ");
//  Serial.println(counter);
}


void setup() {
  Can0.begin(1000000);
//  ads.begin();
  Serial.begin(115200);

  //loadcell
  scale.begin(dataPin, clockPin);
  scale.set_scale(420.0983);
  scale.tare();
  scale2.begin(dataPin2, clockPin2);
  scale2.set_scale(420.0983);
  scale2.tare();
  scale3.begin(dataPin3, clockPin3);
  scale3.set_scale(420.0983);
  scale3.tare();

  // motor canbus initiating
  mot1.Faul_init();
  mot1.Faul_iniset();
  mot1.Faul_velset(velM);
  mot2.Faul_init();
  mot2.Faul_iniset();
  mot2.Faul_velset(velM2);
  mot3.Faul_init();
  mot3.Faul_iniset();
  mot3.Faul_velset(velM3);
  
  pos = mot1.Faul_abs_pos_r();
  pos2 = mot2.Faul_abs_pos_r();
  pos3 = mot2.Faul_abs_pos_r();
  state = 3;
  pinMode(LED_sync, OUTPUT);


  start_timing = millis();
}


void loop() {

  load_flx = - scale.get_units();
  load_flx2 = - scale2.get_units();
  load_ex2 = - scale3.get_units();
  load_ex2 = load_ex2 / 2.28;


  state_change();
  PD_con();
  flexion_2();
  extension_2();
  flexion();
  repeat_fe();
  randomize();
  delay_timer();
  
  print_status();
}
