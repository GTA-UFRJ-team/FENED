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
# Usage: python pkt2csv.py <path_to_read> <path_to_write>

import sys
import pickle
import numpy as np
from scipy.fft import fft



f = open(sys.argv[1],"rb")
time_file = pickle.load(f)
fft_dict = {}


for key, value in time_file.items():
    value[0][0] = 0.0
    fft_dict[key] = [np.abs(fft(value[0])),np.abs(fft(value[1])),value[2]] 


f = open(sys.argv[2], "wb")
pickle.dump(fft_dict,f)
f.close()
