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
# usage: python join_csv.py <binary_path> <save_path>


import sys
import os
import pickle

dataset = []
path = sys.argv[1]
list_names = os.listdir(path)

for name in list_names:
    f = open(path+"/"+name,"rb")
    data = pickle.load(f)
    for line in data:
        dataset.append(line)
    f.close()

w = open(sys.argv[2],"wb")
pickle.dump(dataset,w)
w.close()
