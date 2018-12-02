# ECE650 : Project


To build the project use the commands

```bash
mkdir build && cd build && cmake ../ && make
```

to generate various graph inputs run the python script. 
From project root:
```bash
cd inputs && ./gen_inputs.py
```

To run project with generated inputs:
```bash
cd build && cat ../inputs/inputs.in | ./prj-ece650
```

To send errors and logs to file, use:
```bash
cd build && cat ../inputs/inputs.in | ./prj-ece650 2> log.log 
```

