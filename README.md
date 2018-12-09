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
cd build && cat ../inputs/inputs.in | ./prj-ece650 -r 0 -o
```

To send errors and logs to file, use:
```bash
cd build && cat ../inputs/inputs.in | ./prj-ece650 2> log.log 
```

Flags for Operation:
- -o [write performance time results to file]
- -n value [prepend name to filename]
- -i [this flag bypasses the sat solver if trying inputs of large v]
- -l [enables write results to std::clog]

