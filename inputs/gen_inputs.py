#!/usr/bin/python3

import subprocess
subprocess.Popen("> inputs.in", shell=True)

graphs = [5] * 10 + [10] * 10
for v in graphs:
    subprocess.Popen("./graphGen {0} >> inputs.in".format(v), shell=True)