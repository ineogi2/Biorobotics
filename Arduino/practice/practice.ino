//-----------------------------------------------------------
/* for micro_ros */

#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/float32_multi_array.h>

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



//-----------------------------------------------------------
/* for micro_ros function */

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
  pub_msg.data ++;
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
  //-----------------------------------------------------------
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
  const unsigned int timer_timeout = 1000;
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

  pub_msg.data = 0;         //counting number


  // create subscriber
  RCCHECK(rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "host_pub_topic"));

  // create subscribing executor
  RCCHECK(rclc_executor_init(&sub_executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&sub_executor, &subscriber, &sub_msg, &subscription_callback, ON_NEW_DATA));

  data_msg.data.capacity = 6;
  data_msg.data.data = (float*) malloc(data_msg.data.capacity * sizeof(float));
  data_msg.data.size = 6;

  for(int32_t i = 0; i < 6; i++){
    data_msg.data.data[i] = 1;
  }

}

int number = 0;

void loop() {

//  RCCHECK(rclc_executor_spin_some(&sub_executor, RCL_MS_TO_NS(100)));
//  if (number%10000==0){
//    pub_msg.data++;
    RCSOFTCHECK(rclc_executor_spin_some(&pub_executor, RCL_MS_TO_NS(100)));
//    delay(100);
//  }
  RCSOFTCHECK(rclc_executor_spin_some(&data_executor, RCL_MS_TO_NS(100)));
  number ++;
}
