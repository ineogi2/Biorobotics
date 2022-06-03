# Biorobotics
Biorobotics Lab Intern 2021.08 ~


## Biorobotics Lab Intern 2021.08 ~
### 2021.08 ~ 2021.11

### 2021.12 ~ 2022.02

### 2022.03 ~ 
## 06/03 result
* Distal / Middle / Proximal 손가락 마커 인식 -> Tension 데이터와 결합
* 영상 참조
* Future work : Green color 인식 개선 필요 + 마커 여러 개 사용

## Command 모음

### ROS2 Node 모음
* **ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyACM0** : host ~ teensy
* **ros2 launch realsense2_camera rs_launch.py**  or  
  **ros2 run realsense2_camera realsense2_camera_node** : camera node
* **ros2 run ros_image ros_image** : image_processing node

### Git  
* git init - 해당 폴더를 git 과 연동 or git clone "SSH 주소" 로 폴더 다운받고 시작해도 됨  
* git add . - 현재 폴더 전부 등록  
* git status - 상태 확인  
* git commit -m "commit message" - commit 등록  
* git remote add origin "SSH주소" - git 주소 등록  
* git push origin main - 업로드  

### ros2 토픽  
* ros2 topic list -t : 토픽 리스트 + 형태  
* ros2 topic echo <> : 토픽 내용  
* ros2 topic info <> : 토픽의 Publisher + subscriber  
* ros2 interface show <sensors.msg/msg/Image> : 토픽 형식  
* ros2 topic pub <topic name> <msg_type> "args" : 토픽 publishing  
* ros2 topic hz <> : 토픽 publish rate  

### Ubuntu  
#### 복사
* cp [option] <원본> <사본>  
  * -p : 원본 그대로 복사  
  * -r : 하위 디렉토리 및 파일 복사  
  
#### 이동
* mv [option] <원본> [디렉토리]  

#### 폴더 생성
* mkdir -p [디렉토리]  

#### 삭제
* rm [option] <파일 or 폴더>  
  * -f : 바로 삭제
  * -r : 디렉토리도 삭제 가능
  * -rf : 디렉토리까지 포함하여 전부 삭제
