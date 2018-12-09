#!/usr/bin/python3

import os
from collections import defaultdict
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
plt.style.use('ggplot')

path = os.path.join('data', '5-10-15_CNF-SAT-VC.csv')
df = pd.read_csv(path, index_col=None, )

computed_sizes = defaultdict(list)


# Get the size of the results from the 5-10-15 graphs

path = os.path.join('data', 'ALL_results.txt')
with open(path, 'r') as f:
    for line in f:
        algo, res = [x.strip() for x in line.split(':')]
        res = res.split(',')
        computed_sizes[algo].append(len(res))   
print(computed_sizes)

# Get the size of the results from the 17V graphs
    #INSERT HERE
    
# Get the size of the results from the 20V graphs
    #INSERT HERE


cnf_res = np.array(computed_sizes['CNF-SAT-VC'])
approx_1_res = np.array(computed_sizes['APPROX-VC-1'])
approx_2_res = np.array(computed_sizes['APPROX-VC-2'])
        
cnf_AR = cnf_res / cnf_res
vc1_AR = approx_1_res / cnf_res
vc2_AR = approx_2_res / cnf_res


plt.plot(vc1_AR, 'r')
plt.plot(list(range(len(vc2_AR))), vc2_AR, 'b')
plt.plot(list(range(len(cnf_res))), cnf_AR, 'g')

plt.title('Approximation Ratio')
plt.ylabel('Approxmation Ratio')
plt.xlabel('Graph Input')
plt.legend(['Approx-VC-1', 'Approx-VC-2', 'CNF-SAT-VC'])
#plt.savefig(f'images/statall/{name}.pdf', format='pdf', dpi=1200)
#plt.savefig(f'images/statall/{name}.svg', format='svg', dpi=1200)
#plt.savefig(f'images/statall/{name}.png', format='png', dpi=1200)
plt.show()

data = {'AC1': vc1_AR, 'AC2': vc2_AR, 'CNF': cnf_AR}
df2 = pd.DataFrame(data=data)