#!/usr/bin/python3

import subprocess
import time

for run in range(0,10):
    print('Starting Run: ', run)
    print('================================================================\n\n')
    if run == 0:
        p = subprocess.Popen("cat ../../inputs/5-10-15_graphs.in | ../../build/prj-ece650 -o -n 5-10-15 -l > 5-15-20_results.txt", shell=True)
    else:
        p = subprocess.Popen("cat ../../inputs/5-10-15_graphs.in | ../../build/prj-ece650 -o -n 5-10-15", shell=True)
    p.wait()
    
