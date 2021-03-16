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
# usage: python get_class.py <read_path> <write_path> <class>



import sys

reader = open(sys.argv[1],"r")
writer = open(sys.argv[2],"w")
for line in reader:
    w = line[:-1]+","+sys.argv[3]+"\n"
    writer.writelines(w)
