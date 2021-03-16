# coding:utf-8
# 
# Author: Lucas Airam Castro de Souza
# Laboratory: Grupo de Teleinformática e Automação
# University: Universidade Federal do Rio de Janeiro
#
#
#
#
#
#
#
#
# usage: python random_forest.py <dataset_path> <training_percentage> 



from sklearn.ensemble import RandomForestClassifier
import sys
from random import randint
from sklearn.metrics import accuracy_score

f = open(sys.argv[1],"rb")
dataset = f.readlines()
size = float(sys.argv[2])
sample_size = len(dataset)*size  # percentage of traning sample size

 
X_training = []
Y_training = []
X_test = []
Y_test = []

while(len(dataset) > sample_size):
    seed = randint(0,len(dataset)-1)
    pre_data = dataset.pop(seed)
    data = str(pre_data).split(",")
    X_test.append(data[5:-1])
    Y_test.append(data[-1][:-1])


for pre_data in dataset:
    data = str(pre_data).split(",")
    X_training.append(data[5:-1]) 
    Y_training.append(data[-1][:-1]) 


clf = RandomForestClassifier()
clf = clf.fit(X_training, Y_training)
result = clf.predict(X_test)
print(accuracy_score(Y_test, result))
