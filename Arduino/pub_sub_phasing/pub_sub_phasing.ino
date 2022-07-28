#include <FlexCAN.h>
#include <EEPROM.h>
#include <HX711.h>
#include "Faulcan.h"

#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/float32_multi_array.h>


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
int min1_init = 1500;


int max2 =10000;//flexion
int max2_init = 0;
int min2 = 0;
int min2_init = 3000;

int max3 =17000;//extension proximal limit stroke
int max3_init = 9800;
int min3 = 0;

//set maximum velocity of motor
int velM = 2000;
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

float flx_th;
float flx2_th;
float ex2_th;


double flx_th_step;
double flx2_th_step;
double ex2_th_step;

double grad_flx = 2;
double grad_flx2 = 2;
double grad_ex2 = 1;

unsigned long currentTime, previousTime;
double elapsedTime;
double cumerror1,cumerror2,cumerror3;
double rateerror1,rateerror2,rateerror3;
double lasterror1, lasterror2, lasterror3;




rcl_publisher_t publisher;
rcl_publisher_t data_publisher;
rcl_subscription_t subscriber;

std_msgs__msg__Int32 pub_msg;
std_msgs__msg__Float32MultiArray data_msg;
std_msgs__msg__Int32 sub_msg;

rclc_executor_t pub_executor;
rclc_executor_t data_executor;
rclc_executor_t sub_executor;

rclc_support_t support;

rcl_allocator_t allocator;

rcl_node_t node;

rcl_timer_t timer;
rcl_timer_t data_timer;

#define LED_PIN 13

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}


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
  
  if ( abs(K_p*error1 + K_i * cumerror1 + K_d * rateerror1) < velM){
    vel1 = (int)abs( K_p*error1 + K_i * cumerror1 + K_d * rateerror1); 
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
    if (abs(pos2 - min2_init) > 1)
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
    pos2 = mot2.Faul_abs_pos_rw(stop_pos);
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

int input = 3;
int old_input = 3;

void state_change() {
  
  input = sub_msg.data;

  if (input != old_input){
    if (input == 0) //flexion
    {
      pub_msg.data++;
      state = 0;
      cumerror1 = 0;
      cumerror2 = 0;
      cumerror3 = 0;
    }
    else if (input == 1) //back to initial
    {
      state = 1;
    }
    else if (input == 2){
      state = 2; //Emergency Stop 
    }
    else if (input == 3){
      state = 3; //quit the exp mode 
      pub_msg.data++;
//      randomize();
      cumerror1 = 0;
      cumerror2 = 0;
      cumerror3 = 0;
    }
  
    old_input = input;
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
    
    if (state == 0 && re1 < 20 && re2 < 20 && re3 < 20) // state1 = flexion, motor2 flexor
    {
      delay_timer();
    }

    else if (state == 1 && re4 < 5 && re5 < 5 && re6 < 5) // state1 = flexion, motor2 flexor
    {
      flx_th_step = load_flx;
      flx2_th_step = load_flx2;
      ex2_th_step = load_ex2;
      delay(500);
      pub_msg.data += 2;
      state = 0;
      phasing();      
    }
}


// hold on the posture
void delay_timer(){
  if (tv == 100)
  {
    tv = 1;
//    pub_msg.data++;
    state = 1;
    rr = 1;
  }
  else{
   delay(10);
    tv = tv + 1;
  }
}

void phasing(){

  if (flx2_th < 1500){
    flx2_th += 50;
  }
  else{
    flx2_th = 200;
    flx_th += 50;
  }
  
}


void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{  
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisher, &pub_msg, NULL));
  }
}

void data_timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{  
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&data_publisher, &data_msg, NULL));
  }
}

void subscription_callback(const void * msgin)
{  
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  digitalWrite(LED_PIN, (msg->data != pub_msg.data) ? HIGH : LOW);  
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

  flx_th = 1250;
  flx2_th = 350;
  ex2_th = 50;

  /* for micro_ros */
  
  set_microros_transports();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  
  
  delay(1000);

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  rcl_node_options_t node_ops_pub = rcl_node_get_default_options();
  node_ops_pub.domain_id = 7;
  
  // create publisher node
  RCCHECK(rclc_node_init_with_options(&node, "micro_ros_node", "", &support, &node_ops_pub));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "teensy_loop_topic"));

  RCCHECK(rclc_publisher_init_default(
    &data_publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray),
    "teensy_encoder_topic"));

  // create timer,
  const unsigned int timer_timeout = 100;
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback));

  const unsigned int data_timer_timeout = 1;
  RCCHECK(rclc_timer_init_default(
    &data_timer,
    &support,
    RCL_MS_TO_NS(data_timer_timeout),
    data_timer_callback));

  // create publishing executor
  RCCHECK(rclc_executor_init(&pub_executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&pub_executor, &timer));

  RCCHECK(rclc_executor_init(&data_executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&data_executor, &data_timer));

  pub_msg.data = 215;         //counting number

  data_msg.data.capacity = 9;
  data_msg.data.data = (float*) malloc(data_msg.data.capacity * sizeof(float));
  data_msg.data.size = 9;

  for(int32_t i = 0; i < 9; i++){
    data_msg.data.data[i] = 1;
  }

  // create subscriber
  RCCHECK(rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "host_pub_topic"));

  // create subscribing executor
  RCCHECK(rclc_executor_init(&sub_executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&sub_executor, &subscriber, &sub_msg, &subscription_callback, ON_NEW_DATA));

  
}

void loop() {

  load_flx = - scale.get_units();
  load_flx2 = - scale2.get_units();
  load_ex2 = - scale3.get_units();
  load_ex2 = load_ex2 / 2.28;

  // receive host message
  RCCHECK(rclc_executor_spin_some(&sub_executor, RCL_MS_TO_NS(10)));
  state_change();
  
  PD_con();
  flexion_2();
  extension_2();
  flexion();
  repeat_fe();
  
  data_msg.data.data[0] = flx_th;
  data_msg.data.data[1] = flx2_th;
  data_msg.data.data[2] = ex2_th;
  data_msg.data.data[3] = load_flx;
  data_msg.data.data[4] = load_flx2;
  data_msg.data.data[5] = load_ex2;
  data_msg.data.data[6] = pos;
  data_msg.data.data[7] = pos2;
  data_msg.data.data[8] = pos3;
  

  RCSOFTCHECK(rclc_executor_spin_some(&pub_executor, RCL_MS_TO_NS(10)));
  RCSOFTCHECK(rclc_executor_spin_some(&data_executor, RCL_MS_TO_NS(10)));

}
