
"""
05/09 newly fixed - vision + tension
MEAN OF THE COORDINATE
"""

from cv_bridge.core import CvBridgeError
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile
from rclpy.qos import QoSHistoryPolicy
from sensor_msgs.msg import Image
from std_msgs.msg import Int32
from cv_bridge import CvBridge, CvBridgeError
import cv2 as cv
import numpy as np
import os

# --------------------------------------------------------------------------------
# Node

class Imagenode(Node):

    def __init__(self):
        super().__init__('image_node')           ### 노드 이름
        qos_profile = QoSProfile(history=QoSHistoryPolicy.KEEP_LAST, depth=10)       ### 버퍼 설정

        text_path = '/home/ineogi2/Documents/ros_data'
        os.chdir(text_path)

        """teensy 통신 subscriber"""
        self.tension_subscriber = self.create_subscription(Int32, '/tension', self.subscribe_tension, qos_profile)

        """realsense 통신 subscriber"""
        self.image_subscriber = self.create_subscription(Image, '/color/image_raw', self.subscribe_pic, qos_profile)
        # self.image_subscriber = self.create_subscription(Image, '/infra1/image_rect_raw', self.subscribe_pic, qos_profile)

        self.cv_bridge = CvBridge()

        self.depth_subscriber = self.create_subscription(Image, '/depth/image_rect_raw', self.subscribe_depth, qos_profile)
        self.depth_bridge = CvBridge()

        """색깔 영역 boundary 설정"""
        self.color_num = 3    # 색깔 개수

        self.kernelopen = np.ones((5,5))
        self.kernelclose = np.ones((5,5))

        self.lower_blue = np.array([100, 50, 70])
        self.upper_blue = np.array([128, 255, 255])

        self.lower_green = np.array([25, 52, 50])
        self.upper_green = np.array([95, 255, 255])


        self.lower_red = np.array([155, 70, 70])
        self.upper_red = np.array([190, 255, 255])

        self.lower = [self.lower_blue,self.lower_green,self.lower_red]
        self.upper = [self.upper_blue,self.lower_green,self.upper_red]
        self.col = [(255,0,0),(0,255,0),(0,0,255)]
        self.order = ['b','g','r']
        
        """Global 변수 설정"""
        self.buffer_length = 100     # buffer 크기
        self.center = []    # marker center points
        self.count = 1      # 저장 주기
        self.signal = 2     # 통신 변수
        self.data = []
        # self.finger = []    # finger direction
        # self.tension = []   # tension data


# --------------------------------------------------------------------------------
# callback funtion

    def subscribe_tension(self, tension):
        if self.signal == 0:
            self.data[-1].append(round(tension.data,2))
            self.signal = 2



    def subscribe_pic(self, img):
        if len(self.data) == self.buffer_length:
            data_list = np.reshape(np.array(self.data), (self.buffer_length,4))
            self.get_logger().info('{0} file saved.'.format(self.count))
            np.savetxt('data {0}.csv'.format(self.count), data_list, fmt='%f', delimiter=',')
            self.data = []
            self.count += 1
            
        if self.signal == 2:
            self.center = []

            try:
                cv_image = self.cv_bridge.imgmsg_to_cv2(img, desired_encoding="bgr8")
                # cv.imshow("Original", cv_image)

                img_ = cv.cvtColor(cv_image, cv.COLOR_BGR2HSV)
                black = np.zeros((360, 640, 3), np.uint8)

                for i in range(self.color_num):
                    contour = self.color_mask(img_,i)
                    center = self.moment(contour)
                    self.center.append(center)
                    if center != [0,0]:
                        cv.circle(black, [x//2 for x in center], 5, self.col[i], -1, cv.LINE_4)

                cv.imshow("Image", black)
                cv.waitKey(10)

            except CvBridgeError:
                self.get_logger().info('No.{0} error'.format(self.count))

            self.signal -= 1

    def subscribe_depth(self, depth_img):
        if self.signal == 1:
            try:
                cv_image = self.depth_bridge.imgmsg_to_cv2(depth_img, desired_encoding="passthrough")
                depth_array = np.array(cv_image, dtype=np.float32)
                for color in range(self.color_num):           #b g r 순서
                    now = self.center[color]
                    x,y = int(now[0]*848/1280), int(now[1]*480/720)  
                    depth = depth_array[y][x]

                    now.append(depth)
                    self.center[color] = now

                self.data.append(self.center)

            except CvBridgeError:
                self.get_logger().info('Error')
                rclpy.shutdown()
            
            # self.signal = 2
            self.signal -= 1


# --------------------------------------------------------------------------------
# vision function

    def moment(self, contours):
        center = [[0, 0]]

        for cnt in contours:
            if cv.contourArea(cnt) > 600:
                mmt = cv.moments(cnt)
                if mmt['m00'] != 0:
                    cx = int(mmt['m10']/mmt['m00'])
                    cy = int(mmt['m01']/mmt['m00'])
                else:
                    cx, cy = 0, 0

                center.append([cx, cy])

        for e in center:
            if e == [0,0]:
                center.remove(e)

        if center == []:
            return [0,0]

        else:
            center = np.array(center)
            val = np.mean(center, axis=0).tolist()
            val = [int(val[0]), int(val[1])]
            return val

    def color_mask(self, img_, color):
        ctr = []
        mask = cv.inRange(img_, self.lower[color], self.upper[color])
        open = cv.morphologyEx(mask, cv.MORPH_OPEN, self.kernelopen)
        close = cv.morphologyEx(open, cv.MORPH_CLOSE, self.kernelclose)

        contours, _ = cv.findContours(close, cv.RETR_LIST, cv.CHAIN_APPROX_NONE)
        for cnt in contours:
            if cv.contourArea(cnt) > 600:
                ctr.append(cnt)

        return ctr


# --------------------------------------------------------------------------------
# main

def main(args=None):
    rclpy.init(args=args)
    node = Imagenode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info('Keyboard Interrupt (SIGINT)')
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()