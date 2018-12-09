#!/usr/bin/python3

import subprocess
import time

processes = []
for run in range(10):
    if run == 0:
        p = subprocess.Popen("cat ../../inputs/5v_to_2000v_graphs.in | ../../build/prj-ece650 -o -n V2000 -l -i".format(run), shell=True)
    else:
        p = subprocess.Popen("cat ../../inputs/5v_to_2000v_graphs.in | ../../build/prj-ece650 -o -n V2000 -l -i".format(run), shell=True)
    p.wait()