# Biorobotics
Biorobotics Lab Intern 2021.08 ~


## Biorobotics Lab Intern 2021.08 ~
### 2021.08 ~ 2021.11

### 2021.12 ~ 2022.02

### 2022.03 ~ 


## Git 명령어
git init - 해당 폴더를 git 과 연동 or git clone "SSH 주소" 로 폴더 다운받고 시작해도 됨
git add . - 현재 폴더 전부 등록
git status - 상태 확인
git commit -m "commit message" - commit 등록
git remote add origin "SSH주소" - git 주소 등록
git push origin main - 업로드

## ros2 토픽 명령어
ros2 topic list -t : 토픽 리스트 + 형태
ros2 topic echo <> : 토픽 내용
ros2 topic info <> : 토픽의 Publisher + subscriber
ros2 interface show <sensors.msg/msg/Image> : 토픽 형식
ros2 topic pub <topic name> <msg_type> "args" : 토픽 publishing
ros2 topic hz <> : 토픽 publish rate
