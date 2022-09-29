
"""
07/11 newly fixed - vision + tension + image_save
MEAN OF THE COORDINATE
"""

from platform import node
import cv2 as cv
from cv_bridge.core import CvBridgeError
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile
from sensor_msgs.msg import Image
from std_msgs.msg import Float32
from std_msgs.msg import Int32
from cv_bridge import CvBridge, CvBridgeError
import numpy as np
import os

text_path = '/home/ineogi2/Biorobotics/Data'
# header_ = 'Distal,-,-,Middle,-,-,Proximal,-,-,Tension'
header_ = 'Tension'

# --------------------------------------------------------------------------------
# Node

class Imagenode(Node):

    def __init__(self):
        super().__init__('image_node')           ### 노드 이름
        qos_profile = QoSProfile(depth=10)       ### 버퍼 설정

        os.chdir(text_path)

        """initializing 통신 subscriber"""
        self.init_subscriber = self.create_subscription(Int32, '/init', self.subscribe_init, qos_profile)

        """teensy 통신 subscriber"""
        self.tension_subscriber = self.create_subscription(Float32, '/tension', self.subscribe_tension, qos_profile)

        """realsense 통신 subscriber"""
        self.image_subscriber = self.create_subscription(Image, '/camera/color/image_raw', self.subscribe_pic, qos_profile)
        self.cv_bridge = CvBridge()

        self.depth_subscriber = self.create_subscription(Image, '/camera/depth/image_rect_raw', self.subscribe_depth, qos_profile)
        self.depth_bridge = CvBridge()

        """색깔 영역 boundary 설정"""
        self.color_num = 3    # 색깔 개수

        self.kernelopen = np.ones((5,5))
        self.kernelclose = np.ones((5,5))

        self.lower_blue = np.array([93, 180, 130])
        self.upper_blue = np.array([128, 255, 255])

        self.lower_green = np.array([60, 85, 70])
        self.upper_green = np.array([93, 255, 255])

        self.lower_red = np.array([155, 100, 70])
        self.upper_red = np.array([190, 255, 255])

        self.col = [(255,0,0),(0,255,0),(0,0,255)]
        self.order = ['b','g','r']
        
        """Global 변수 설정"""
        self.buffer_length = 500        # buffer size
        self.center = []                # marker center points
        self.count = 1
        self.signal = 3
        self.data_save = True
        self.image_save = False         # don't change
        self.image_ = np.array([]); self.tension = 0
        self.image = []; self.depth = []; self.data = []    # data array


    # --------------------------------------------------------------------------------
    # callback funtion

    def subscribe_init(self, init_msg):
        if init_msg.data == 1:          # start
            # self.signal = 2
            self.signal = 0
            self.get_logger().info('Start.')

        elif init_msg.data == 0:        # end
            self.signal = -1

    def subscribe_tension(self, tension):
        if self.signal == 0:
            # self.center = np.array(self.center).flatten().tolist()
            # self.center.append(round(tension.data, 2))
            # self.data.append(self.center)
            # self.signal = 2
            # self.data.append(round(tension.data, 2))
            self.tension = round(tension.data, 2)

    def subscribe_pic(self, img):            
        if self.signal == 0:
            self.center = []

            try:
                cv_image = self.cv_bridge.imgmsg_to_cv2(img, desired_encoding="bgr8")
                    # cv.imshow("Original", cv_image)

                img_ = cv.cvtColor(cv_image, cv.COLOR_BGR2HSV)
                self.image_ = np.array(img_)
                    # black = np.zeros((720, 1280, 3), np.uint8)

                    # if self.image_save:
                    #     cv.imwrite(f'{self.count}.png',cv_image)
                    #     self.count += 1

                    # for i in range(self.color_num):
                    #     contour = self.color_mask(img_,i)
                    #     center = self.moment(contour)
                    #     self.center.append(center)
                    #     if center != [0,0]:
                    #         cv.circle(black, tuple(center), 5, self.col[i], -1, cv.LINE_4)

                    # cv.imshow("Image", black)
                    # cv.waitKey(10)

            except CvBridgeError:
                self.get_logger().info('No.{0} error'.format(self.count))

        # self.signal = 1

    def subscribe_depth(self, depth_img):
        if self.signal == 0:
            try:
                cv_image = self.depth_bridge.imgmsg_to_cv2(depth_img, desired_encoding="passthrough")
                depth_array = np.array(cv_image, dtype=np.float32)
                self.depth.append(depth_array)
                self.image.append(self.image_)
                self.data.append(self.tension)
                # for color in range(self.color_num):           #b g r 순서
                #     now = self.center[color]
                #     x,y = int(now[0]*848/1280), int(now[1]*480/720)
                #     depth = self.depth_preprocessing(depth_array, (x,y))

                #     now.append(depth)
                #     self.center[color] = now

            except CvBridgeError:
                self.get_logger().info('Error')
                rclpy.shutdown()
                
                # # when no Tension
                # if self.data_save:
                #     self.center = np.array(self.center).flatten().tolist()
                #     self.data.append(self.center)
                    
                # self.signal = 0


    # --------------------------------------------------------------------------------
    # vision function

    def moment(self, contours):
        center = [[0, 0]]

        for cnt in contours:
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
        if color == 0:
            self.lower, self.upper = self.lower_blue, self.upper_blue
        elif color == 1:
            self.lower, self.upper = self.lower_green, self.upper_green
        else:
            self.lower, self.upper = self.lower_red, self.upper_red

        mask = cv.inRange(img_, self.lower, self.upper)
        open = cv.morphologyEx(mask, cv.MORPH_OPEN, self.kernelopen)
        close = cv.morphologyEx(open, cv.MORPH_CLOSE, self.kernelclose)

        contours, _ = cv.findContours(close, cv.RETR_LIST, cv.CHAIN_APPROX_NONE)
        for cnt in contours:
            if cv.contourArea(cnt) > 200:
                ctr.append(cnt)

        return ctr

    def depth_preprocessing(self, depth_array, pos):
        depth = []; x,y = pos[0], pos[1]
        for i in range(-3,4):
            for j in range(-3,4):
                try:
                    d = depth_array[y+j][x+i]
                    if d != 0:
                        depth.append(d)
                except:
                    continue

        if depth == []:
            return 0
        depth = np.mean(depth)
        return int(depth)


# --------------------------------------------------------------------------------
# main

def main(args=None):
    rclpy.init(args=args)
    node = Imagenode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info('\nEnd.')
        if node.data_save:
            node.get_logger().info(f'{len(node.data)}')
            os.chdir(text_path+'/png')
            for i in range(len(node.data)):
                cv.imwrite(f'{i}.png',node.image[i])
            os.chdir(text_path+'/depth')
            for i in range(len(node.data)):
                np.savetxt(f'depth {i}.csv', node.depth[i],fmt='%f', delimiter=',')
            # data_list = np.reshape(np.array(node.data), (len(node.data),len(node.data[0])))
            data_list = np.array(node.data)
            node.get_logger().info('file saved.\n')
            os.chdir(text_path)
            np.savetxt('tension.csv', data_list, header=header_,fmt='%f', delimiter=',')
        else:
            node.get_logger().info('Keyboard Interrupt (SIGINT)')
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()