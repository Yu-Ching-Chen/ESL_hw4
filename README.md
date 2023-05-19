# ESL_hw4
NTHU EE6470 ESL homework 4

### Build the platform. <br>
In the top most directory.
```bash 
$ cp -r vp/src/platform/basic-acc [location of riscv-vp]/vp/src/platform
$ cd [location of riscv-vp]/vp/build
$ cmake ..
$ make install
```
### Compile the testbench and run the simulation. <br>
In the top most directory.
```bash 
$ cp -r sw/basic-filter [location of riscv-vp]/sw
$ cd [location of riscv-vp]/sw/basic-filter
$ make
$ make sim
```
