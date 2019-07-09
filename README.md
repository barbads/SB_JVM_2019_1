# SB_JVM_2019_1

Repository of JVM for Software Básico, Professor Marcelo Ladeira.

14/0162241 - Samuel Venzi Lima Monteiro de Oliveira

18/0137816 - Manoel Vieira Coelho Neto

15/0122969 - David Potolski Lafeta

13/0015083 - Matheus Barbosa de Miranda

13/0126861 - Matheus Vieira Fernandes

## Compile and Run the program

To compile the program we provided a `./build.sh` that calls a `make` command that compiles it. If you want to compile it yourself make sure that you link all the needed files correctly. We strongly advise you to use the provided `./build.sh` or the `cmake`.

To run the program you can call it in 3 ways:

- `./sb-2019 program.class` will show both the parsed class file and the execution of the bytecode.
- `./sb-2019 program.class -l` will show only .class file information.
- `./sb-2019 program.class -i` will show only the executed bytecode.

## Main Classes

The 3 main classes used in the program are.

- ClassFile
- JVM
- ContextEntry

So long and thanks for all the fishes
