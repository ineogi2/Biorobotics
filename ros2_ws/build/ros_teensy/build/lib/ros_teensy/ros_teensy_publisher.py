from cv_bridge.core import CvBridgeError
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile
from std_msgs.msg import Int32, Float32MultiArray
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
import cv2
import os
import numpy as np

class Hostnode(Node):

    def __init__(self):
        super().__init__('host_node')           ### 노드 이름
        qos_profile = QoSProfile(depth=20)      ### 버퍼 설정
        os.mkdir('/home/ineogi2/Documents/ros_pics/0')
        os.chdir('/home/ineogi2/Documents/ros_pics/0')     ### 디렉토리 이동

        # """teensy 통신 publisher"""
        # self.publisher = self.create_publisher(Int32, 'host_pub_topic', qos_profile)
        # ### host_node 가 생성하는 토픽이름이 teensy_topic
        # self.timer = self.create_timer(1, self.publish_msg)
        # ### c++에서 콜백함수에 해당. 아래 함수를 계속 돌려준다
        # self.msg = Int32()
        # self.msg.data = 2

        """realsense 통신 subscriber"""
        self.rs_subscriber = self.create_subscription(Image, '/color/image_raw', self.subscribe_pic, qos_profile)
        self.cv_bridge = CvBridge()

        """teensy loop number subscriber"""
        self.loop_subscriber = self.create_subscription(Int32, 'teensy_loop_topic', self.subscribe_msg, qos_profile)

        """teensy encoder subscriber"""
        self.encoder_subscriber = self.create_subscription(Float32MultiArray, 'teensy_encoder_topic', self.subscribe_encoder, qos_profile)

        self.count = 0
        self.signal = 0     ### subscribe image 저장 시그널     1 일 때 사진을 저장함
        self.number = 0     ### subscribe loop number
        self.stop = 0
        self.current_data = [0,0,0,0,0,0,0,0,0]
        self.loop_data = []

    # def publish_msg(self):
    #     self.msg.data = 1
    #     self.publisher.publish(msg)
    #     self.get_logger().info('Host publishing : {0}'.format(msg.data))

    def subscribe_msg(self, msg):
        if msg.data %2 != 0:
            self.stop = 1

        else:
            if self.number == msg.data:
                self.stop = 0

            else:
                self.stop = 1
                list = np.array(self.loop_data)
                self.get_logger().info('{0}, {1}'.format(self.count, len(list)))
                text_path = '/home/ineogi2/Documents/ros_pics'
                os.chdir(text_path)
                np.savetxt('encoder {0}.csv'.format(str(int(self.number/2)).zfill(3)), list, fmt='%f', delimiter=',')
                self.number = msg.data
                self.count = 0
                self.get_logger().info('Current loop : {0}'.format(self.number))
                path = '/home/ineogi2/Documents/ros_pics/{0}'.format(self.number)

                os.mkdir(path)
                os.chdir(path)

                self.loop_data = []
                self.stop = 0


    def subscribe_pic(self, img):
        if self.stop == 0:
            try:
                # self.signal = 1         ### teensy 에서 endcoder 받아와야 함
                self.loop_data.append(self.current_data)
                cv_image = self.cv_bridge.imgmsg_to_cv2(img, desired_encoding="bgr8")
                filename = '{0}_{1}.png'.format(self.number, self.count)
                # self.get_logger().info('No.{0} is saved.'.format(self.count))
                cv2.imwrite(filename, cv_image)
                self.count += 1

            except CvBridgeError:
                self.get_logger().info('Save error')
                rclpy.shutdown()
                

    def subscribe_encoder(self, msg_data):
        # self.get_logger().info('encoder saved')
        self.current_data = msg_data.data



def main(args=None):
    rclpy.init(args=args)
    node = Hostnode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info('Keyboard Interrupt (SIGINT)')
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
