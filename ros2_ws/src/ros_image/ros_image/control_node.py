import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2 as cv
import numpy as np
import sys

# ------Class------
class Imagenode(Node):

    def __init__(self, mode):
        super().__init__('image_node')           ### 노드 이름
        qos_profile = QoSProfile(depth=10)       ### 버퍼 설정

        """realsense 통신 subscriber"""
        self.image_subscriber = self.create_subscription(Image, '/camera/color/image_raw', self.subscribe_pic, qos_profile)
        self.cv_bridge = CvBridge()

        # mode 1 : hsv / mode 2 : canny
        self.mode = mode
        self.val = []


    # ------callback------
    def subscribe_pic(self, img):

        image = self.cv_bridge.imgmsg_to_cv2(img, desired_encoding="bgr8")
        cv.imshow('original', image)
        cv.namedWindow('image')

        if self.mode == 1:
            self.change_hsv(image)
        elif self.mode == 2:
            self.change_canny(image)

        cv.destroyAllWindows()


    # ------vision function------
    def nothing(self, x):
        pass

    def change_hsv(self, image):
        # Create trackbars for color change
        cv.createTrackbar('HMin', 'image', 0, 179, self.nothing)
        cv.createTrackbar('SMin', 'image', 0, 255, self.nothing)
        cv.createTrackbar('VMin', 'image', 0, 255, self.nothing)
        cv.createTrackbar('HMax', 'image', 0, 179, self.nothing)
        cv.createTrackbar('SMax', 'image', 0, 255, self.nothing)
        cv.createTrackbar('VMax', 'image', 0, 255, self.nothing)

        # Set default value for Max HSV trackbars
        cv.setTrackbarPos('HMax', 'image', 179)
        cv.setTrackbarPos('SMax', 'image', 255)
        cv.setTrackbarPos('VMax', 'image', 255)

        # Initialize HSV min/max values
        hMin = sMin = vMin = hMax = sMax = vMax = 0
        self.val = [hMin, hMax, sMin, sMax, vMin, vMax]

        while(1):
            # Get current positions of all trackbars
            hMin = cv.getTrackbarPos('HMin', 'image')
            sMin = cv.getTrackbarPos('SMin', 'image')
            vMin = cv.getTrackbarPos('VMin', 'image')
            hMax = cv.getTrackbarPos('HMax', 'image')
            sMax = cv.getTrackbarPos('SMax', 'image')
            vMax = cv.getTrackbarPos('VMax', 'image')
            self.val = [hMin, hMax, sMin, sMax, vMin, vMax]

            # Set minimum and maximum HSV values to display
            lower = np.array([hMin, sMin, vMin])
            upper = np.array([hMax, sMax, vMax])

            # Convert to HSV format and color threshold
            hsv = cv.cvtColor(image, cv.COLOR_BGR2HSV)
            mask = cv.inRange(hsv, lower, upper)
            result = cv.bitwise_and(image, image, mask=mask)

            # Display result image
            cv.imshow('image', result)
            if cv.waitKey(10) & 0xFF == ord('q'):
                self.get_logger().info(f'hue : ({hMin}, {hMax}) / sat ({sMin}, {sMax}) / val ({vMin}, {vMax})')
                break

    def change_canny(self, image):
        # Create trackbars for change canny threshold
        cv.createTrackbar('Low', 'image', 0, 1000, self.nothing)
        cv.createTrackbar('High', 'image', 0, 1000, self.nothing)
        cv.createTrackbar('Threshold', 'image', 0, 500, self.nothing)
        cv.createTrackbar('MinLineLength', 'image', 0, 300, self.nothing)
        cv.createTrackbar('MaxLineGap', 'image', 0, 10, self.nothing)

        # Set default threshold for canny trackbars
        cv.setTrackbarPos('Low', 'image', 50)
        cv.setTrackbarPos('High', 'image', 150)
        cv.setTrackbarPos('Threshold', 'image', 50)
        cv.setTrackbarPos('MinLineLength', 'image', 50)
        cv.setTrackbarPos('MaxLineGap', 'image', 2)

        self.val = [50, 150, 50, 50, 2]

        while(1):
            # Get current positions of all trackbars
            low = cv.getTrackbarPos('Low', 'image')
            high = cv.getTrackbarPos('High', 'image')
            th = cv.getTrackbarPos('Threshold', 'image')
            length = cv.getTrackbarPos('MinLineLength', 'image')
            gap = cv.getTrackbarPos('MaxLineGap', 'image')
            self.val = [low, high, th, length, gap]

            # Do canny function
            img_canny = cv.Canny(image, low, high)
            lines = cv.HoughLinesP(img_canny, 1, np.pi/180., threshold=th, minLineLength=length, maxLineGap=gap)
            dst = cv.cvtColor(img_canny, cv.COLOR_GRAY2BGR)

            if lines is not None:
                for i in range(lines.shape[0]):
                    pt1 = (lines[i][0][0], lines[i][0][1])
                    pt2 = (lines[i][0][2], lines[i][0][3])
                    cv.line(dst, pt1, pt2, (0,0,255), 2, cv.LINE_AA)

            # Display result image
            cv.imshow('image', dst)
            if cv.waitKey(10) & 0xFF == ord('q'):
                self.get_logger().info(f'Low : {low} / High : {high} / Threshold : {th} / MinLineLength : {length} / MaxLineGap : {gap}')
                break

# ------main function------
def main():
    mode = int(input("Choose mode : 1 -> HSV control / 2 -> Canny control:\t"))
    rclpy.init()
    node = Imagenode(mode)
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        if node.mode == 1:
            node.get_logger().info(f'\n\nhue : ({node.val[0]}, {node.val[1]}) / sat ({node.val[2]}, {node.val[3]}) / val ({node.val[4]}, {node.val[5]})\n')
        elif node.mode == 2:
            node.get_logger().info(f'\n\nLow : {node.val[0]} / High : {node.val[1]} / Threshold : {node.val[2]} / MinLineLength : {node.val[3]} / MaxLineGap : {node.val[4]}\n')
        node.get_logger().info('Keyboard Interrupt (SIGINT)')
    finally:
        node.destroy_node()
        rclpy.shutdown()

# ------execution------
if __name__ == '__main__':
    main()