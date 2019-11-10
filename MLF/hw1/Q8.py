import numpy
import sys
import random
from matplotlib import pyplot as plt 

def sign(a):
    return 1 if a > 0 else -1

def train_set(file):
    set = open(file)
    _X = numpy.zeros((500, 5))
    _y = numpy.zeros((500, 1))
    x = []
    random_d = []
    cnt = 0
    for line in set:
        x.append(1)
        for str in line.split(' '):
            if len(str.split('\t')) == 1:
                x.append(float(str))
            else:
                x.append(float(str.split('\t')[0]))
                x.append(int(str.split('\t')[1].strip()))
        random_d.append(x)
        x = []
        cnt += 1
    random.shuffle(random_d)
    for i in range(cnt):
        for j in range(5):
            _X[i,j] = random_d[i][j]
        _y[i,0] = random_d[i][5]
    return _X, _y

def test_set(file):
    set = open(file)
    _X = numpy.zeros((500, 5))
    _y = numpy.zeros((500, 1))
    x = []
    cnt = 0
    for line in set:
        x.append(1)
        for str in line.split(' '):
            if len(str.split('\t')) == 1:
                x.append(float(str))
            else:
                x.append(float(str.split('\t')[0]))
                _y[cnt,0] = int(str.split('\t')[1].strip())
        _X[cnt,:] = x
        x = []
        cnt += 1
    return _X, _y
 
def pla(file):
    count = 0
    X, y = train_set(file)
    w = numpy.zeros((5, 1))

    while (count < 100):
        for i in range(len(X)):
            if sign(numpy.dot(X[i,:],w)[0]) != sign(y[i, 0]):
                w += y[i, 0] * X[i,:].reshape(5, 1)
                count += 1
            if count == 100:
                break
    return w


def error(train,test):
    cnt = 0
    w = pla(train)
    X, y = test_set(test)
    for i in range(len(X)):
        if sign(numpy.dot(X[i,:], w)[0]) != (y[i, 0]):
            cnt += 1
    return cnt/len(X)

 
def main():
    train_file = sys.argv[1]
    test_file = sys.argv[2]
    ave = 0
    arr = []
    for i in range(1126):
        ave += error(train_file,test_file)
        arr.append(error(train_file,test_file))
    
    print(ave/1126.0)
    nparr = numpy.array(arr)
    
    plt.hist(nparr) 
    plt.title("error rate:"+str(ave/1126.0)) 
    plt.show()
    return 0


if __name__ == '__main__':
    main()

