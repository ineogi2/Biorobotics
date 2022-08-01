# Data Preprocessing
from genericpath import isfile
import os
import shutil
import glob
import cv2 as cv
import numpy as np
from math import acos as ac
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


# Draw Contour
def Contour(image):
    img_gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)
    ret, img_binary = cv.threshold(img_gray, 140, 255, cv.THRESH_BINARY_INV)
    contours, hierarchy = cv.findContours(img_binary, cv.RETR_LIST, cv.CHAIN_APPROX_NONE)
    #print(marker)
    #print(len(contours))
    for cnt in contours:
        area = cv.contourArea(cnt)
        if 200 < area < 800:
            cv.drawContours(image, [cnt], 0, (0,0,255), -1)
            print(area)
    cv.imshow("TEST", image)
    cv.waitKey(10)
    print("\n")
    return 0
    
def moment(image, loop_number, error_count):
    img_gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)
    ret, thr = cv.threshold(img_gray, 100, 255, cv.THRESH_BINARY_INV)
    contours, hierarchy = cv.findContours(thr, cv.RETR_TREE, cv.CHAIN_APPROX_NONE)
    number = 5
    check = 0
    # print(number)
    markers = [0] * number
    center_point = []
    # check_im = np.zeros(shape=image.shape, dtype=np.uint8)

    # contour 중 마커만 추가
    for cnt in contours:
        area = cv.contourArea(cnt)
        if area > 180 and area < 800:
            cv.drawContours(image, [cnt], 0, (0,0,255), -1)
            if area > 680:      #1번 마커
                markers[0] = cnt
                check += 1
            elif area > 510:    #2번 마커
                markers[1] = cnt
                check += 1
            elif area > 330:    #3번 마커
                markers[2] = cnt
                check += 1
            elif area > 260:    #4번 마커
                markers[3] = cnt
                check += 1
            elif area > 180:    #5번 마커
                markers[4] = cnt
                check += 1

    cv.imshow("loop : {0}".format(loop_number), image)
    cv.waitKey(10)
    
    if check == number:
        for marker in markers:
            # mk = np.array(marker)
            mmt = cv.moments(marker)
            for key, val in mmt.items():
                # print('%s:\t%.5f' %(key,val))
                if mmt['m00'] != 0:
                    cx = int(mmt['m10']/mmt['m00'])
                    cy = int(mmt['m01']/mmt['m00'])
                else:
                    cx, cy = 0, 0
            # print(cy,cx)

            center_point.append([cx,cy])

    else:
        center_point = [[0,0], [1,0], [2,0], [3,0], [4,0]]
        error_count += 1
        # print("-----{0}th error point occured-----".format(error_count))

    # print(center_point)
    

    return np.array(center_point), error_count


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
    x, y, w, h = 0, 0, 800, 700
    src = cv.imread('/home/ineogi2/Pictures/test.png')
    src = Crop(src, (x,y,w,h))
    src_hsv = cv.cvtColor(src, cv.COLOR_BGR2HSV)

    kernel = np.ones((3,3), np.uint8)

    blue = cv.inRange(src_hsv, (100, 30, 30), (150, 255, 255))
    black = cv.inRange(src_hsv, (10, 10, 10), (70, 70, 70))

    blue = cv.erode(blue, kernel, iterations=10)
    black = cv.erode(black, kernel, iterations=1)

    cv.imshow('test', src_hsv)
    # cv.imshow('test', black)
    cv.waitKey(0)




    # loop_number = 2
    # if (os.path.isdir('/home/ineogi2/Documents/ros_pics/0')):
    #     shutil.rmtree('/home/ineogi2/Documents/ros_pics/0')
    #     os.remove('/home/ineogi2/Documents/ros_pics/encoder 000.csv')
    # while True:
    #     workspace = '/home/ineogi2/Documents/ros_pics/{0}'.format(loop_number+2)

    #     if (os.path.isdir(workspace)):
    #         print("\n------------Working on loop : {0}------------\n".format(loop_number))
    #         data_path = '/home/ineogi2/Documents/ros_pics'
    #         os.chdir(data_path)     ### 디렉토리 이동
    #         # RoI(data_path, data_path, (x,y,w,h))
    #         cv.destroyAllWindows()
    #         angle_array = []
    #         pic_number = 0
    #         error_count = 0
    #         workpic = '/home/ineogi2/Documents/ros_pics/{0}/{0}_{1}.png'.format(loop_number, pic_number)

    #         while os.path.isfile(workpic):
    #             if pic_number % 100 == 0:
    #                 print("------Working on pic : {0}".format(pic_number))
    #                 print("      Current error : {0} % \n".format(round(error_count*100/(pic_number+1), 2)))
    #             workpic_ = cv.imread(workpic)
    #             workpic_ = Crop(workpic_, (x,y,w,h))
    #             # Contour(workpic_)
    #             center_point, error_count = moment(workpic_, loop_number, error_count)
    #             angles = angle_detect(center_point)
    #             angle_array.append(angles)
    #             pic_number += 1
    #             workpic = '/home/ineogi2/Documents/ros_pics/{0}/{0}_{1}.png'.format(loop_number, pic_number)

    #         angle_array = np.array(angle_array)
    #         np.savetxt('angle {0}.csv'.format(str(int(loop_number/2)).zfill(3)), angle_array, fmt='%f', delimiter=',')
    #         print("Error : {0} % \n".format(round(error_count*100/pic_number, 2)))
    #         shutil.rmtree('/home/ineogi2/Documents/ros_pics/{0}'.format(loop_number))
            

    #         loop_number += 2

    #     else:
    #         continue