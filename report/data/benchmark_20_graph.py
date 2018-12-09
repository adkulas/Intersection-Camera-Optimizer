#!/usr/bin/python3

import subprocess
import time

processes = []
for run in range(3):
    if run == 0:
        p = subprocess.Popen("cat ../../inputs/20v_graph.in | ../../build/prj-ece650 -o -n V20_Run_{0} -l > 20_results.txt".format(run), shell=True)
    else:
        p = subprocess.Popen("cat ../../inputs/20v_graph.in | ../../build/prj-ece650 -o -n V20_Run_{0} -l".format(run), shell=True)
    processes.append(p)

for p in processes:
    p.wait()

processes = []
for run in range(10):
    if run == 0:
        p = subprocess.Popen("cat ../../inputs/5v_to_2000v_graphs.in | ../../build/prj-ece650 -o -n V2000 -l -i".format(run), shell=True)
    else:
        p = subprocess.Popen("cat ../../inputs/5v_to_2000v_graphs.in | ../../build/prj-ece650 -o -n V2000 -l -i".format(run), shell=True)
    p.wait()