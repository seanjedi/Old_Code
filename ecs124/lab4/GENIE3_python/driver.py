from GENIE3 import *

data = loadtxt('Gene.txt',skiprows=1)
f = open('Gene.txt')
gene_names = f.readline()
f.close()
gene_names = gene_names.rstrip('\n').split('\t')
VIM = GENIE3(data)
get_link_list(VIM,gene_names=gene_names, file_name='ranking.txt')




