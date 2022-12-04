import os, time
import numpy as np
import torch
import torch.nn as nn
import argparse
from torch.utils.data import TensorDataset, DataLoader
import matplotlib.pyplot as plt


# for data merging - only once
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

    merged_data = np.concatenate((rgb_data[:,0:2], depth_data[:,0].reshape(-1,1), rgb_data[:,2:4], depth_data[:,1].reshape(-1,1), rgb_data[:,4:6], depth_data[:,2].reshape(-1,1), tension_data[:]), axis=1)
    return merged_data

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


def train():
    # hyperparameters
    args = {
        'device' : torch.device('cuda' if torch.cuda.is_available() else 'cpu'),
        'input_dim' : 10,
        'hidden_dim' : 32,
        'n_layers' : 3,
        'output_dim' : 2
    }
    lr = 1e-3
    epochs = 5
    lookback = 10
    batch_size = 64

    # train data
    trial_1 = np.loadtxt('trial_1.csv', delimiter=',')
    # outputs = np.loadtxt('vicon_1.csv', delimiter=',')[lookback:]

    inputs = data_preprocess(trial_1, lookback)
    for trial_num in [2,3,5]:
        trial_ = np.loadtxt('trial_'+str(trial_num)+'.csv', delimiter=',')
        # output_ = np.loadtxt('vicon_'+str(trial_num)+'.csv', delimiter=',')[lookback:]

        inputs = np.concatenate((inputs, data_preprocess(trial_, lookback)), axis=0)
        # outputs = np.concatenate((outputs, output_), axis=0)

    inputs = inputs.reshape(-1, lookback, 10)
    # outputs = outputs.reshape(-1, 2)
    outputs = np.ones((len(inputs), 2))

    train_data = TensorDataset(torch.from_numpy(inputs), torch.from_numpy(outputs))
    train_loader = DataLoader(train_data, shuffle=True, batch_size=batch_size, drop_last=True)

    # model
    model = GRUNet(args)
    if os.path.isfile('model.pt'):
        model.load_state_dict(torch.load('model.pt'))
        print('[Load] success.')
    loss_func = nn.MSELoss()
    optim = torch.optim.Adam(model.parameters(), lr=1e-3)
    model.train()

    # training start
    print('Starting Training.\n')
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


def test():
    # test data
    test_data = np.loadtxt('trial_6.csv', delimiter=',')
    test_data = data_preprocess(test_data)

if __name__ == '__main__':
    path = '/home/ineogi2/Biorobotics/Data'
    os.chdir(path)

    parser = argparse.ArgumentParser(description='Data Processing - Biorobotics')
    parser.add_argument('--merge', action='store_true', help='For merging data')
    parser.add_argument('--test', action='store_true', help='For testing model')
    main_args = parser.parse_args()

    if main_args.merge:
        for trial_num in [1,2,3,5,6]:
            merged_data = merge_data(trial_num)
            np.savetxt('trial_'+str(trial_num)+'.csv', merged_data, delimiter=',')

    else:
        model, loss_list = train()
        plt.plot(loss_list)
        plt.show()

        if main_args.test:
            test()