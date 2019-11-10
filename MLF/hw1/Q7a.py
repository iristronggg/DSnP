import numpy
import random
import copy

class Pocket(object):
    def __init__(self, dimension, train_count, test_count):
        self.__dimension = dimension
        self.__train_count = train_count
        self.__test_count = test_count

    def random_Matrix(self, path):
        training_set = open(path)
        randomLst = []
        x = []
        x_count = 0
        for line in training_set:
            x.append(1)
            for str in line.split(' '):
                if len(str.split('\t')) == 1:
                    x.append(float(str))
                else:
                    x.append(float(str.split('\t')[0]))
                    x.append(int(str.split('\t')[1].strip()))
            randomLst.append(x)
            x = []
            x_count += 1
        return randomLst

    def train_Maxtrix(self, path):
        X_train = numpy.zeros((self.__train_count, self.__dimension))
        y_train = numpy.zeros((self.__train_count, 1))
        randomLst = self.random_Matrix(path)
        random.shuffle(randomLst)
        for i in range(self.__train_count):
            for j in range(self.__dimension):
                X_train[i, j] = randomLst[i][j]
            y_train[i, 0] = randomLst[i][self.__dimension]
        return X_train, y_train


    def interationW(self, path):
        count = 0
        X_train, y_train = self.train_Maxtrix(path)
        w = numpy.zeros((self.__dimension, 1))
        bestCount = self.__train_count
        bestW = numpy.zeros((self.__dimension, 1))

        while True:
            for i in range(self.__train_count):
                if numpy.dot(X_train[i, :], w)[0] * y_train[i, 0] <= 0:
                    w += 0.5 * y_train[i, 0] * X_train[i,:].reshape(5, 1)
                    count += 1
                    num = 0
                    for j in range(self.__train_count):
                        if numpy.dot(X_train[j, :], w)[0] * y_train[j, 0] <= 0:
                            num += 1
                    if num < bestCount:
                        bestCount = num
                        bestW = copy.deepcopy(w)
                    if count == 50:
                        break
            if count == 50:
                break
        return bestW

    def test_Matrix(self, test_path):
        X_test = numpy.zeros((self.__test_count, self.__dimension))
        y_test = numpy.zeros((self.__test_count, 1))
        test_set = open(test_path)
        x = []
        x_count = 0
        for line in test_set:
            x.append(1)
            for str in line.split(' '):
                if len(str.split('\t')) == 1:
                    x.append(float(str))
                else:
                    x.append(float(str.split('\t')[0]))
                    y_test[x_count, 0] = (int(str.split('\t')[1].strip()))
            X_test[x_count, :] = x
            x = []
            x_count += 1
        return X_test, y_test

    def testError(self, train_path, test_path):
        w = self.interationW(train_path)
        X_test, y_test = self.test_Matrix(test_path)
        count = 0.0
        for i in range(self.__test_count):
            if numpy.dot(X_test[i, :], w)[0] * y_test[i, 0] <= 0:
                count += 1
        return count / self.__test_count

average_error_rate = 0
for i in range(2000):
    my_Pocket = Pocket(5, 500, 500)
    average_error_rate += my_Pocket.testError('hw1_7_train.dat.txt', 'hw1_7_test.dat.txt')
print (average_error_rate / 2000.0)