import os, time
import numpy as np
import torch
import torch.nn as nn
import argparse
from torch.utils.data import TensorDataset, DataLoader
import matplotlib.pyplot as plt

""" only once """
# for data merging
def merge_data(trial_num):
    trial_num = str(trial_num)

    # read data
    rgb_data = np.loadtxt('Trial_'+trial_num+'_RGB.csv', delimiter=',')
    depth_data = np.loadtxt('Trial_'+trial_num+'_Depth.csv', delimiter=',')
    tension_data = np.loadtxt('Trial_'+trial_num+'_Tension.csv', delimiter=',')

    # synchronized data
    data_num = min(len(rgb_data), len(depth_data))
    rgb_data = rgb_data[:data_num].reshape(data_num, -1)
    depth_data = depth_data[:data_num].reshape(data_num, -1)
    tension_data = np.array([tension_data[int(len(tension_data)/data_num*i)] for i in range(data_num)]).reshape(data_num,-1)

    merged_data = np.concatenate((rgb_data[:,0:2], depth_data[:,0].reshape(-1,1), rgb_data[:,2:4], depth_data[:,1].reshape(-1,1),
                                    rgb_data[:,4:6], depth_data[:,2].reshape(-1,1), tension_data[:]), axis=1)
    return merged_data

# for true_y
def compress_vicon(trial_num):
    trial_num = str(trial_num)

    # read data
    trial_vicon = np.loadtxt('trial_angle_'+trial_num+'.csv', delimiter=',')
    trial_data = np.loadtxt('trial_'+trial_num+'.csv', delimiter=',')
    data_num = len(trial_data)

    # synchronized data
    trial_vicon = np.array([trial_vicon[int(len(trial_vicon)/data_num*i)] for i in range(data_num)]).reshape(data_num, 2)
    np.savetxt('vicon_'+trial_num+'.csv', trial_vicon, delimiter=',')

    print(f'{trial_num} vicon data saved.')

""""""

def data_preprocess(data, lookback=10):
    length = len(data)
    new_data = np.zeros((length-lookback, lookback, 10))
    for i in range(length-lookback):
        now = data[i]
        new_data[i] = data[i:i+lookback] - now

    return new_data



class GRUNet(nn.Module):
    def __init__(self, args):
        super(GRUNet, self).__init__()

        self.device = args['device']
        self.input_dim = args['input_dim']
        self.hidden_dim = args['hidden_dim']
        self.n_layers = args['n_layers']
        self.output_dim = args['output_dim']
        self.gru = nn.GRU(self.input_dim, self.hidden_dim, self.n_layers, batch_first=True, dropout=0.2)
        self.fc = nn.Linear(self.hidden_dim, self.output_dim)
        self.act_fn = torch.relu

    def forward(self, x, h):
        out, h = self.gru(x, h)
        out = self.fc(self.act_fn(out[:,-1]))
        return out, h

    def init_hidden(self, batch_size):
        weight = next(self.parameters()).data
        hidden = weight.new(self.n_layers, batch_size, self.hidden_dim).zero_().to(self.device)
        return hidden


def train(args):
    # training parameter
    lr = 1e-3
    epochs = 100
    lookback = args['lookback']
    batch_size = 64

    # train data
    trial_1 = np.loadtxt('trial_1.csv', delimiter=',')
    outputs = np.loadtxt('vicon_1.csv', delimiter=',')[lookback:]

    inputs = data_preprocess(trial_1, lookback)
    for trial_num in [2,3,5]:
        trial_ = np.loadtxt('trial_'+str(trial_num)+'.csv', delimiter=',')
        output_ = np.loadtxt('vicon_'+str(trial_num)+'.csv', delimiter=',')[lookback:]

        inputs = np.concatenate((inputs, data_preprocess(trial_, lookback)), axis=0)
        outputs = np.concatenate((outputs, output_), axis=0)

    inputs = inputs.reshape(-1, lookback, 10)
    outputs = outputs.reshape(-1, 2)

    train_data = TensorDataset(torch.from_numpy(inputs), torch.from_numpy(outputs))
    train_loader = DataLoader(train_data, shuffle=True, batch_size=batch_size, drop_last=True)

    # model
    model = GRUNet(args)
    if os.path.isfile('model.pt'):
        model.load_state_dict(torch.load('model.pt'))
        print('[Load] success.')
    else:
        print('[New] model')
    loss_func = nn.MSELoss()
    optim = torch.optim.Adam(model.parameters(), lr=lr)
    model.train()

    # training start
    print('Start Training.\n')
    epoch_times = []; epoch_avg_loss = []
    for epoch in range(1, epochs+1):
        start_time = time.time()
        h = model.init_hidden(batch_size)
        avg_loss = 0

        for x, true_y in train_loader:
            h = h.data
            model.zero_grad()

            out, h = model(x.to(args['device']).float(), h)
            loss = loss_func(out, true_y.to(args['device']).float())
            loss.backward()
            optim.step()
            avg_loss += loss.item()
        
        current_time = time.time()
        epoch_avg_loss.append(avg_loss/len(train_loader))
        print(f'Epoch {epoch} / {epochs} Done. Total loss : {avg_loss/len(train_loader)}')
        epoch_times.append(current_time - start_time)

    torch.save(model.state_dict(), 'model.pt')
    print('\n[Save] success.')
    print(f'Total Training Time : {sum(epoch_times)}')

    return model, epoch_avg_loss

def test(args):
    # test data
    test_x = np.loadtxt('trial_6.csv', delimiter=',')
    test_x = data_preprocess(test_x, args['lookback'])
    test_y = np.loadtxt('vicon_6.csv', delimiter=',')[args['lookback']:]

    # load model
    model = GRUNet(args)
    if os.path.isfile('model.pt'):
        model.load_state_dict(torch.load('model.pt'))
        print('[Load] success.')
    else:
        raise Exception

    # start test
    print('Start testing.\n')
    model.eval()
    outputs = []; print_idx = np.linspace(0, len(test_x), 10, dtype=int)
    for i in range(len(test_x)):
        input = torch.from_numpy(test_x[i]).reshape(-1, args['lookback'], 10)
        h = model.init_hidden(1)
        out, h = model(input.to(args['device']).float(), h)
        outputs.append(out.cpu().detach().numpy().reshape(-1))
        if i in print_idx:
            print(f'{i+1} / {len(test_x)} testing.')

    outputs = np.array(outputs).reshape(-1, 2)
    sMAPE_index = 0
    sMAPE_middle = 0
    for i in range(len(test_x)):
        sMAPE_index += np.mean(abs(outputs[i][0]-test_y[i][0])/(test_y[i][0]+outputs[i][0])/2)/len(outputs)
        sMAPE_middle += np.mean(abs(outputs[i][1]-test_y[i][1])/(test_y[i][1]+outputs[i][1])/2)/len(outputs)

    print(f'sMAPE for index : {round(sMAPE_index*100, 3)}%')
    print(f'sMAPE for middle : {round(sMAPE_middle*100, 3)}%')

    return outputs, test_y, sMAPE_index, sMAPE_middle

if __name__ == '__main__':
    path = '/home/ineogi2/Biorobotics/Data'
    os.chdir(path)

    parser = argparse.ArgumentParser(description='Data Processing - Biorobotics')
    parser.add_argument('--merge', action='store_true', help='For merging data')
    parser.add_argument('--test', action='store_true', help='For testing model')
    parser.add_argument('--train', action='store_true', help='For training model')
    parser.add_argument('--vicon', action='store_true', help='For processing vicon data')
    parser.add_argument('--plot', action='store_true', help='For plot test data')
    main_args = parser.parse_args()

    # hyperparameters
    args = {
        'device' : torch.device('cuda' if torch.cuda.is_available() else 'cpu'),
        'input_dim' : 10,
        'hidden_dim' : 32,
        'n_layers' : 3,
        'output_dim' : 2,
        'lookback' : 10
    }

    if main_args.merge:
        for trial_num in [1,2,3,5,6]:
            merged_data = merge_data(trial_num)
            np.savetxt('trial_'+str(trial_num)+'.csv', merged_data, delimiter=',')

    elif main_args.vicon:
        for trial_num in [1,2,3,5,6]:
            compress_vicon(trial_num)

    elif main_args.train:
        model, loss_list = train(args)
        plt.plot(loss_list)
        plt.show()

    elif main_args.test:
        outputs, targets, sMAPE_index, sMAPE_middle = test(args)
        np.savetxt('outputs.csv', outputs, delimiter=',')

    elif main_args.plot:
        outputs = np.loadtxt('outputs.csv', delimiter=',')
        targets = np.loadtxt('vicon_6.csv', delimiter=',')
        range_list = np.linspace(0,len(outputs),11, dtype=int)
        for i in range(10):
            plot_range = range(range_list[i],range_list[i+1])

            plt.figure(figsize=(14,10))
            plt.subplot(2,1,1)
            plt.plot(plot_range, outputs[plot_range,0], "-o", color="g", label="Predicted")
            plt.plot(plot_range, targets[plot_range,0], color="b", label="Actual")
            plt.ylabel('Angle of index joint.')
            plt.legend()

            plt.subplot(2,1,2)
            plt.plot(plot_range, outputs[plot_range,1], "-o", color="g", label="Predicted")
            plt.plot(plot_range, targets[plot_range,1], color="b", label="Actual")
            plt.ylabel('Angle of middle joint.')
            plt.legend()

            plt.savefig(f'test_{plot_range[0]}_{plot_range[-1]+1}.png', dpi=200)
            # plt.show()
        