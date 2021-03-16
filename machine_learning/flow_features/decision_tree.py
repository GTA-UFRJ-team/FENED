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
# usage: python decision_tree.py <dataset_path> <training_percentage> 



from sklearn import tree
import sys
from sklearn.metrics import classification_report
from random import randint
from sklearn.metrics import accuracy_score
from sklearn.metrics import precision_score
from sklearn.metrics import recall_score
from sklearn.metrics import f1_score
from sklearn import metrics
from sklearn.model_selection import cross_validate

f = open(sys.argv[1],"rb")
dataset = f.readlines()
size = float(sys.argv[2])
sample_size = len(dataset)*size

 
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

X = X_test + X_training 
y = Y_test + Y_training 

clf = tree.DecisionTreeClassifier(max_features="auto",max_depth=23)
scoring = ['precision_macro', 'recall_macro','f1_macro','accuracy']
scores = cross_validate(clf, X, y, cv=10, scoring=scoring)

print("%0.2f recall with a standard deviation of %0.2f" % (scores['test_recall_macro'].mean(), scores['test_recall_macro'].std()))
print("%0.2f precision with a standard deviation of %0.2f" % (scores['test_precision_macro'].mean(), scores['test_precision_macro'].std()))
print("%0.2f f1 score with a standard deviation of %0.2f" % (scores['test_f1_macro'].mean(), scores['test_f1_macro'].std()))
print("%0.2f accuracy with a standard deviation of %0.2f" % (scores['test_accuracy'].mean(), scores['test_accuracy'].std()))

