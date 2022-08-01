import os, pandas
import numpy as np
from keras.models import Sequential, load_model
from keras.layers import Dense, LSTM
from keras.callbacks import EarlyStopping

# data preprocessing
def data_sync(idx, time_step=5):
    vicon = pandas.read_csv(vicon_file[idx]).values
    exp = pandas.read_csv(exp_file[idx]).values

    # --data period synchronization
    vicon_num = vicon.shape[0]; exp_num = exp.shape[0]
    q = vicon_num // exp_num
    new_vicon = []
    for i in range(exp_num):
        new_vicon.append(vicon[q*i])
    new_vicon = np.array(new_vicon)

    # --make data sequence
    x_train = []; y_train = []
    for index in range(exp_num-time_step):
        x_train.append(np.array(exp[index:index+time_step]))
        y_train.append(np.array(new_vicon[index+time_step]))
    return (x_train, y_train)

def data_concatenate(file_num, time_step):
    (x_train, y_train) = data_sync(idx=0, time_step=time_step)

    m = np.mean(x_train,0); s = np.std(x_train,0)
    x_train = (x_train-m)
    for i in range(1,file_num):
        (x_t, y_t) = data_sync(idx=i, time_step=time_step)
        x_t= (x_t-m)
        x_train = np.concatenate([x_train,x_t])
        y_train = np.concatenate([y_train,y_t])
    x_train, y_train = np.array(x_train), np.array(y_train)

    return (x_train, y_train)

# data training
def model(x_train, y_train, time_step):
    model = Sequential()
    model.add( LSTM(units=16, activation='relu', input_shape=(time_step,10)) )
    model.add(Dense(8))
    model.add(Dense(2))

    model.compile(loss='mae', optimizer='adam', metrics=['mae'])
    early_stopping = EarlyStopping(monitor='loss', patience=10, mode='auto')
    print(model.summary())

    model.fit(x_train, y_train, 
            epochs=30, batch_size=64, verbose=1,
            validation_split=0.3,
            callbacks=[early_stopping])

    return model


if __name__ == "__main__":
    # hyperparameters
    path = '/home/ineogi2/Biorobotics/Data'
    date = '0726'
    time_step = 30
    os.chdir(path)

    vicon_file = []; exp_file = []
    for f_name in os.listdir(date):
        if f_name.startswith('angle'):
            vicon_file.append(f_name)
        if f_name.startswith('try'):
            exp_file.append(f_name)
    file_num = len(vicon_file)  # file 개수
    # print(file_num)
    os.chdir(date)

    (x_train, y_train) = data_concatenate(file_num, time_step)
    print(x_train.shape, y_train.shape)
    model = model(x_train, y_train, time_step)
    model.save('lstm.h5')
    # model.evaluate(x_train, y_train)