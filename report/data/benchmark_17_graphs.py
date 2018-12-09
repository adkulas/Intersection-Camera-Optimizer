#!/usr/bin/python3

import subprocess
import time

processes = []
for run in range(3):
    print('Starting Run: ', run)
    print('================================================================\n\n')
    if run == 0:
        p = subprocess.Popen("cat ../../inputs/17v_graphs.in | ../../build/prj-ece650 -o -n 17 -l > 17_results.txt".format(run), shell=True)
    else:
        p = subprocess.Popen("cat ../../inputs/17v_graphs.in | ../../build/prj-ece650 -o -n 17 -l".format(run), shell=True)
    processes.append(p)
    p.wait()