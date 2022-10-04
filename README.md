# Biorobotics
- Biorobotics Lab Intern
- AAN을 위한 Joint Angle Estimation 을 주제로 tension과 vision을 통해 사용자의 현재 performance를 파악하려는 연구


## Biorobotics Lab Intern 2021.08 ~
### 2021.08 ~ 2021.10 : < 준비 >
* Realsense Camera ( D435i )를 통해 얻은 픽셀과 뎁스 값 -> user's intention 파악하고자 함
* ROS1 사용 -> 여러 노드를 통합,관리하기 용이함

  ------ AAN을 위한 재활 대상자의 상태 파악을 주제로 연구 시작 ------
  * strain guage를 통해 tension과 joint angle 파악하려 시도
  * polymer부터 전도성 천, 3D 프린터 등의 방법등을 이용

### 2021.11 ~ 2022.02 : < Mockup finger를 통한 실험 세팅 >
* Mockup finger를 통해 실험 세팅
* ROS2 사용 : embedded system에 micro_ros를 이용해 통신 가능 + realtime 제어
* 3개의 active tendon + 2개의 passive tendon 존재
* teensy에서 무작위로 input tension 값 생성 -> PD제어를 통해 해당 목표값을 향해 motor 동작
  
  ==> tension과 motor encoder를 통해 joint angle의 추세를 파악해 보기 위한 작업들

### 2022.03 ~ : < Wearable Robot 과의 결합 >
* wearable robot과 결합 -> Joint angle estimation의 최종 목적

### etc
* 사용한 tool
  * ros2-foxy / micro_ros / openCV 4.4.0 / teensy / realsense tools / ...

* **ros-foxy-realsense2-camera**
  * realsense SDK 2.0과 충돌하는 것처럼 보임


## Command 모음
### ROS2
#### -ros2 pkg
* (~/ws/src) ros2 pkg create <package_name> --build-type ament_python --dependencies rclpy std_msgs
  * pkg create 후, **source ~/ws/install/local_setup.bash 해줘야 함 <- micro_ros나 그냥 ros에서도 마찬가지

#### -ros2 토픽  
* ros2 topic list -t : 토픽 리스트 + 형태  
* ros2 topic echo <> : 토픽 내용  
* ros2 topic info <> : 토픽의 Publisher + subscriber  
* ros2 interface show <sensors.msg/msg/Image> : 토픽 형식  
* ros2 topic pub --once /init std_msgs/msg/Int32 '{data : '1'}' : 토픽 publishing  
* ros2 topic hz <> : 토픽 publish rate  

### 실험 세팅 단축키
* 'cam' -> 'teensy' -> 'record' -> 'ss' or 'qq'
