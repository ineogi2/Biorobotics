# angle / encoder number comparison Preprocessing
import glob

from numpy.lib.function_base import angle

data_path = '/home/ineogi2/Documents/ros_pics/'

def isAngle():
    angles = glob.glob(data_path+'angle*.csv')
    return angles
def isEncoder():
    encoders = glob.glob(data_path+'encoder*.csv')
    return encoders

#execute
if __name__ == "__main__":

    angles = isAngle()
    encoders = isEncoder()
    angle_number = []
    encoder_number = []

    # print(angles, encoders)
    print('angle : {0} / encoder : {1}'.format(len(angles), len(encoders)))

    for i in range(len(angles)):
        val = int(angles[i][-7:-4])
        angle_number.append(val)

    for i in range(len(encoders)):
        val = int(encoders[i][-7:-4])
        encoder_number.append(val)

    angle_number.sort()
    encoder_number.sort()

    i = 0
    j = 0

    for k in range(len(encoders)):
        if angle_number[i] == encoder_number[j]:
            # print('angle : {0} - encoder : {1}'.format(angle_number[i], encoder_number[j]))
            i+=1
            j+=1
            continue
        else:
            print('-----angle : {0} / encoder : {1}-----'.format(angle_number[i], encoder_number[j]))
            j+=1
