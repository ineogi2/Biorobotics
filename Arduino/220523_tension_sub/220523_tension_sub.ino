#include <EEPROM.h>
#include <HX711.h>

#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32.h>
#include <std_msgs/msg/int32.h>

// loadcell parameters
HX711 scale;

uint8_t dataPin = 17;
uint8_t clockPin = 18;
int LED_sync = 2;

// LPF parameters
double pre_val = 0.0;
double pre_t = 0.0;
double tau = 1.0;

rcl_publisher_t publisher;
rcl_subscription_t subscriber;

std_msgs__msg__Float32 pub_msg;
std_msgs__msg__Int32 sub_msg;

rclc_executor_t pub_executor;
rclc_executor_t sub_executor;

rclc_support_t support;

rcl_allocator_t allocator;

rcl_node_t node;

rcl_timer_t timer;

#define vicon_sync 2
#define LED_PIN 13

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{  
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisher, &pub_msg, NULL));
  }
}

/*
void subscription_callback(const void * msgin)
{  
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  digitalWrite(LED_PIN, (msg->data != pub_msg.data) ? HIGH : LOW);  
}
*/
void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_sync, OUTPUT);

  //loadcell
  scale.begin(dataPin, clockPin);
  scale.set_scale(890);
  scale.tare();
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  set_microros_transports();
  
  delay(10);

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
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "/tension"));

  // create timer,
  const unsigned int timer_timeout = 10;
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback));

  // create publishing executor
  RCCHECK(rclc_executor_init(&pub_executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&pub_executor, &timer));

  pub_msg.data = 0.0;         //counting number

/*
  // create subscriber
  RCCHECK(rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "host_pub_topic"));

  // create subscribing executor
  RCCHECK(rclc_executor_init(&sub_executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&sub_executor, &subscriber, &sub_msg, &subscription_callback, ON_NEW_DATA));
*/
  
}

void loop() {
  
  double cur_val = -scale.get_units();
  double val = 1/(tau+1)*pre_val + tau/(tau+1)*cur_val;

  pub_msg.data = val;
//  RCCHECK(rclc_executor_spin_some(&sub_executor, RCL_MS_TO_NS(10)));
  RCSOFTCHECK(rclc_executor_spin_some(&pub_executor, RCL_MS_TO_NS(10)));



  pre_val = val;

}
