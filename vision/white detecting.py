# Data Preprocessing
import os
import cv2 as cv
import numpy as np
from math import acos as ac

def get_line(img, count):
    edges = cv.Canny(img, 50, 150)
    lines = cv.HoughLinesP(edges, 1, np.pi/180., 120, minLineLength=10, maxLineGap=2)
    dst = cv.cvtColor(edges, cv.COLOR_GRAY2BGR)

    if lines is not None:
        for i in range(lines.shape[0]):
            pt1 = (lines[i][0][0], lines[i][0][1])
            pt2 = (lines[i][0][2], lines[i][0][3])
            cv.line(dst, pt1, pt2, (0,0,255), 2, cv.LINE_AA)
        cv.imshow("Worked", dst)
        cv.waitKey(500)
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
        # cv.imshow("Original", workpic_)
        get_line(workpic_, count)

        # 작업내용
        
        count += 1
        workpic = f'{count}.png'