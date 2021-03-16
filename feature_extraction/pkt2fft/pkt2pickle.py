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
# Usage: python pkt2csv.py <pcap_path> <path_to_write> optinal: <pcap_class> -1 or 1

import pickle
import pyshark
import sys


pcap_file = pyshark.FileCapture(sys.argv[1])
flows = {}


for pkt in pcap_file:
    if 'IP' in pkt:
        if pkt['ip'].src+','+pkt['ip'].dst in flows.keys():
            flows[pkt['ip'].src+','+pkt['ip'].dst][0].append(float(pkt.sniff_timestamp) - flows[pkt['ip'].src+','+pkt['ip'].dst][0][0])
            flows[pkt['ip'].src+','+pkt['ip'].dst][1].append(int(pkt.length))
        else:
            if pkt['ip'].dst+','+pkt['ip'].src in flows.keys():
                flows[pkt['ip'].dst+','+pkt['ip'].src][0].append(float(pkt.sniff_timestamp) - flows[pkt['ip'].dst+','+pkt['ip'].src][0][0])
                flows[pkt['ip'].dst+','+pkt['ip'].src][1].append(-1*int(pkt.length))
       	    else:
                flows[pkt['ip'].src+','+pkt['ip'].dst] = [[float(pkt.sniff_timestamp)],[int(pkt.length)]]

if sys.argv[3] == "1":
    for key in flows.keys():
        flows[key].append(1)
elif sys.argv[3] == "-1":
    for key in flows.keys():
        flows[key].append(-1)
else:
    pass

f = open(sys.argv[2], "wb")
pickle.dump(flows,f)
f.close()

