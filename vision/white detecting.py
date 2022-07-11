# Data Preprocessing
from genericpath import isfile
import os
import shutil
import glob
import cv2 as cv
import numpy as np
from math import acos as ac

import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile
from rclpy.qos import QoSHistoryPolicy
from sensor_msgs.msg import Image
from std_msgs.msg import Float32
from cv_bridge import CvBridge, CvBridgeError
np.set_printoptions(threshold=np.inf, linewidth=np.inf) #inf = infinity 

# Get Coordinates from Image Clicking
def GetCoord(path):
    img = cv.imread(path)
    cv.imshow("image", img)
    cv.setMouseCallback("image", click)
    cv.waitKey(0)
    cv.destroyAllWindows()

def click(event, x, y, flags, params):
    if event == cv.EVENT_LBUTTONDOWN:
        print(x, ' ', y)

    
# Crop Image with Designated Coordinates
def RoI(InPath, OutPath, coord):
    for ipath in glob.glob(os.path.join(InPath, '*.png')):
        image = cv.imread(ipath)
        cv.imwrite(OutPath+os.path.basename(ipath), Crop(image, coord))
    return

def Crop(img, coord):
    x, y, w, h = coord
    cropped = img[y:y+h, x:x+w]
    return cropped




# vector & angle dectector
def vector_length(vector):
    vector = np.array(vector)
    
    return np.inner(vector, vector) ** 0.5

def angle_detect(center_point):
    vectors = []
    angles = []
    for i in range(4):
        vector = np.subtract(center_point[i+1], center_point[i])
        vector = vector/vector_length(vector)
        vectors.append(vector)
        # print(vector_length(vector))
    for i in range(3):
        try:
            inner_product = np.inner(vectors[i], vectors[i+1])
            angles.append(ac(inner_product))
        except:
            angles.append(0)

    return angles




#execute
if __name__ == "__main__":
    save_path = '/home/ineogi2/Biorobotics/Documents/Data' 
    pic_path = '/home/ineogi2/Biorobotics/Documents/Data/Pics';  os.chdir(pic_path)
    x, y, w, h = 0, 0, 430, 345
    count = 1; workpic = f'{count}.png'
    data = []

    print('\nImage processing start\n')
    while os.path.isfile(workpic):
        workpic_ = cv.imread(workpic)
        vector = []

        center_point, error_count = moment(workpic_, loop_number, error_count)
        angles = angle_detect(center_point)
        angle_array.append(angles)

        angle_array = np.array(angle_array)
        np.savetxt('angle {0}.csv'.format(str(int(loop_number/2)).zfill(3)), angle_array, fmt='%f', delimiter=',')
        print("Error : {0} % \n".format(round(error_count*100/pic_number, 2)))
        shutil.rmtree('/home/ineogi2/Documents/ros_pics/{0}'.format(loop_number))
        
        count += 1
        workpic = f'/home/ineogi2/Biorobotics/Documents/Pics/{count}.png'