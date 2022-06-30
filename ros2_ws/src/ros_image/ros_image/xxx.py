
"""
Real using node. Not for Practice
"""

from cv_bridge.core import CvBridgeError
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile
from sensor_msgs.msg import Image
from std_msgs.msg import Int32
from cv_bridge import CvBridge, CvBridgeError
import cv2 as cv
import numpy as np
import os

class Imagenode(Node):

    def __init__(self):
        super().__init__('image_node')           ### 노드 이름
        qos_profile = QoSProfile(depth=10)       ### 버퍼 설정

        text_path = '/home/ineogi2/Documents/ros_pics'
        os.chdir(text_path)

        """teensy 통신"""
        # self.host_pub = self.create_publisher(Int32, 'host_pub', qos_profile)
        # self.timer = self.create_timer(1, self.publish_msg)

        # self.host_sub = self.create_subscription(Int32, 'teensy_pub', self.subscribe_encoder, qos_profile)
        self.encoder = []

        """realsense 통신 subscriber"""
        self.image_subscriber = self.create_subscription(Image, '/color/image_raw', self.subscribe_pic, qos_profile)
        self.cv_bridge = CvBridge()

        self.depth_subscriber = self.create_subscription(Image, '/depth/image_rect_raw', self.subscribe_depth, qos_profile)
        self.depth_bridge = CvBridge()

        self.kernelopen = np.ones((5,5))
        self.kernelclose = np.ones((5,5))

        """색깔 영역 boundary 설정"""
        self.center = []

        self.lower_blue = np.array([100, 50, 70])
        self.upper_blue = np.array([128, 255, 255])
        self.blue_col = (255,0,0)
        # self.bule_center = []

        self.lower_green = np.array([25, 52, 50])
        self.upper_green = np.array([95, 255, 255])
        self.green_col = (0,255,0)
        # self.green_center = []


        self.lower_red = np.array([155, 70, 70])
        self.upper_red = np.array([190, 255, 255])
        self.red_col = (0,0,255)
        # self.red_center = []

        self.count = 1
        self.signal = 2
        self.finger = []
        self.tension = []


    # def publish_msg(self):
    #     if self.signal == 0:
    #         self.host_pub.publish(self.signal)
    #     else:
    #         self.host_pub.publish(-1)

    def subscribe_encoder(self, encoder):
        self.encoder = encoder.data
        if self.signal == 0:
            self.tension.append(self.encoder)
            self.signal = 2



    def subscribe_pic(self, img):
        # if len(self.finger) == 100:
        #     tension_list = np.array(self.tension)
        #     finger_list = np.reshape(np.array(self.finger), (100,9))
        #     self.get_logger().info('{0} file saved.'.format(self.count))
        #     # np.savetxt('tension {0}.csv'.format(self.count), tension_list, fmt='%f', delimiter=',')
        #     np.savetxt('finger {0}.csv'.format(self.count), finger_list, fmt='%f', delimiter=',')
        #     self.tension = []
        #     self.finger = []
        #     self.count += 1
            

        if self.signal == 2:
            self.center = []

            try:
                cv_image = self.cv_bridge.imgmsg_to_cv2(img, desired_encoding="bgr8")
                cv.imshow("Original", cv_image)

                # print()
                img_ = cv.cvtColor(cv_image, cv.COLOR_BGR2HSV)
                black = np.zeros((480, 848, 3), np.uint8)

                blue_contours = self.color_mask(img_, black, 'b')
                green_contours = self.color_mask(img_, black, 'g')
                red_contours = self.color_mask(img_, black, 'r')

                self.center.append(self.moment(blue_contours))
                self.center.append(self.moment(green_contours))
                self.center.append(self.moment(red_contours))

                cv.circle(black, self.center[0], 5, (255, 0, 0), -1, cv.LINE_4)
                cv.circle(black, self.center[1], 5, (0, 255, 0), -1, cv.LINE_4)
                cv.circle(black, self.center[2], 5, (0, 0, 255), -1, cv.LINE_4)

                cv.imshow("Image", black)
                cv.waitKey(10)

            except CvBridgeError:
                self.get_logger().info('No.{0} error'.format(self.count))

            # self.signal -= 1

    def subscribe_depth(self, depth_img):
        if self.signal == 1:
            try:
                cv_image = self.depth_bridge.imgmsg_to_cv2(depth_img, desired_encoding="passthrough")
                depth_array = np.array(cv_image, dtype=np.float32)
                self.get_logger().info('size : {0}'.format(depth_array.shape))
                for color in range(len(self.center)):           #b g r 순서
                    now = self.center[color]
                    x,y = now[0], now[1]  
                    depth = depth_array[y][x]

                    now.append(depth)
                    self.center[color] = now

                # self.finger.append(self.finger_vector(self.center))

                self.finger.append(self.center)

            except CvBridgeError:
                self.get_logger().info('Error')
                rclpy.shutdown()
            # self.signal -= 1
            self.signal = 2



    def finger_vector(self, center):
        dip = np.subtract(center[0], center[1])
        pip = np.subtract(center[1], center[2])
        finger = [dip.tolist(), pip.tolist()]

        return finger        

    def moment(self, contours):
        center = [[0, 0]]

        for cnt in contours:
            if cv.contourArea(cnt) > 600:
                mmt = cv.moments(cnt)
                for key, val in mmt.items():
                    if mmt['m00'] != 0:
                        cx = int(mmt['m10']/mmt['m00'])
                        cy = int(mmt['m01']/mmt['m00'])
                    else:
                        cx, cy = 0, 0
                center.append([cx, cy])

        center = center[-1]
        return center

    def color_mask(self, img_, back, color):
        ctr = []

        if color == 'b':    #blue mask
            low = self.lower_blue
            up = self.upper_blue
            col = self.blue_col
        elif color == 'g':  #green mask
            low = self.lower_green
            up = self.upper_green
            col = self.green_col
        else:               #red mask
            low = self.lower_red
            up = self.upper_red
            col = self.red_col


        mask = cv.inRange(img_, low, up)
        open = cv.morphologyEx(mask, cv.MORPH_OPEN, self.kernelopen)
        close = cv.morphologyEx(open, cv.MORPH_CLOSE, self.kernelclose)

        contours, hierarchy = cv.findContours(close, cv.RETR_LIST, cv.CHAIN_APPROX_NONE)
        for cnt in contours:
            if cv.contourArea(cnt) > 600:
                ctr.append(cnt)

        # cv.drawContours(back, ctr, -1, col, 2)

        return ctr





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