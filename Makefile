
X3Merge : X3FwMerge.o Main.o
	gcc X3FwMerge.o Main.o -o X3Merge

X3FwMerge.o : X3FwMerge.c X3FwMerge.h
	gcc -c X3FwMerge.c

Main.o : Main.c X3FwMerge.h
	gcc -c Main.c