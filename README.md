# 16-Bit Interpreter
This repository implements an [interpreter](https://www.techopedia.com/definition/7793/interpreter) for a 16-bit machine; it was my introduction to low-level programming.

The bulk of the project is found in the 'pullet16interpreter' class and header; all other classes (other than the main) are for readability/organization/simplification. The main class' sole purpose is to handle input/output and execution. 

To execute this, the below commands are used:
```
$ make -f ./makefile
$ ./Aprog adotout4 zzin.txt output_name.txt log_name.txt
```
  *Notice the lack of a file extension on the argv[1] argument.
  
### Credits
Not all of this repository is my own, original thought. The framework to this code was written by Dr. Duncan A. Buell from the Unversity of South Carolina. The substance to the code is my own. 


    
