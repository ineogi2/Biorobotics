# Data Preprocessing
import os
import cv2 as cv
import numpy as np
from math import acos as ac

def get_line(img, count):
    edges = cv.Canny(img, 50, 150)
    lines = cv.HoughLinesP(edges, 1, np.pi/180., 120, minLineLength=50, maxLineGap=5)
    dst = cv.cvtColor(edges, cv.COLOR_GRAY2BGR)

    if lines is not None:
        for i in range(lines.shape[0]):
            pt1 = (lines[i][0][0], lines[i][0][1])
            pt2 = (lines[i][0][2], lines[i][0][3])
            cv.line(dst, pt1, pt2, (0,0,255), 2, cv.LINE_AA)
        # cv.imshow("Worked", dst)
        cv.waitKey(10)
    else:
        print(f'image {count} is not worked.\n')

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

def nothing(x):
        pass

def change_canny(image):
        # Create trackbars for change canny threshold
        cv.createTrackbar('Low', 'image', 0, 1000, nothing)
        cv.createTrackbar('High', 'image', 0, 1000, nothing)
        cv.createTrackbar('Threshold', 'image', 0, 500, nothing)
        cv.createTrackbar('MinLineLength', 'image', 0, 300, nothing)
        cv.createTrackbar('MaxLineGap', 'image', 0, 10, nothing)

        # Set default threshold for canny trackbars
        cv.setTrackbarPos('Low', 'image', 50)
        cv.setTrackbarPos('High', 'image', 150)
        cv.setTrackbarPos('Threshold', 'image', 50)
        cv.setTrackbarPos('MinLineLength', 'image', 50)
        cv.setTrackbarPos('MaxLineGap', 'image', 2)

        while(1):
            # Get current positions of all trackbars
            low = cv.getTrackbarPos('Low', 'image')
            high = cv.getTrackbarPos('High', 'image')
            th = cv.getTrackbarPos('Threshold', 'image')
            length = cv.getTrackbarPos('MinLineLength', 'image')
            gap = cv.getTrackbarPos('MaxLineGap', 'image')

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
                print(f'Low : {low} / High : {high} / Threshold : {th} / MinLineLength : {length} / MaxLineGap : {gap}')
                break



#execute
if __name__ == "__main__":
    save_path = '/home/ineogi2/Biorobotics/Documents/Data' 
    pic_path = '/home/ineogi2/Biorobotics/Documents/Data/Pics';  os.chdir(pic_path)
    x, y, w, h = 0, 0, 430, 345
    count = 1; workpic = f'{count}.png'
    data = []

    print('\nImage processing start\n')
    while os.path.isfile(workpic):
        workpic_ = cv.imread(workpic, cv.IMREAD_GRAYSCALE)
        # cv.imshow('original',workpic_)
        workpic_ = cv.rotate(workpic_, cv.ROTATE_90_CLOCKWISE)
        workpic_ = (cv.resize(workpic_,(1440, 848)))[300:700, 500:800]
        cv.imshow('original', workpic_)
        cv.imshow("image", workpic_)
        change_canny(workpic_)
        # get_line(workpic_, count)

        # 작업내용
        
        count += 1
        workpic = f'/home/ineogi2/Biorobotics/Documents/Pics/{count}.png'