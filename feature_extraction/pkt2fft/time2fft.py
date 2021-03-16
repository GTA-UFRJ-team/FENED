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
# Usage: python pkt2csv.py <path_to_read> <path_to_write> <pkt_window>

import sys
import pickle
import numpy as np
from scipy.fft import fft

size = int(sys.argv[3])

f = open(sys.argv[1],"rb")
time_file = pickle.load(f)
writer = []
aux = []

for line in time_file:
    if len(line[0]) > size:
        aux.append([line[0][:size],1])
    elif len(line[0]) < size:
        for zeros in range(size - len(line[0])):
            line[0].append(0)
        writer.append([line[0],1])
    else:        
        writer.append([line[0],1])
    


for value in aux:
    writer.append([np.abs(fft(value[0])),value[1]])    


f = open(sys.argv[2], "wb")
pickle.dump(writer,f)
f.close()
