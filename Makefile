X3Merge : X3FwMerge.o Main.o fiio_checksum_appender.o
	gcc X3FwMerge.o Main.o fiio_checksum_appender.o -o X3Merge

fiio_checksum_appender.o : fiio_checksum_appender.c
	gcc -c fiio_checksum_appender.c

X3FwMerge.o : X3FwMerge.c X3FwMerge.h
	gcc -c X3FwMerge.c

Main.o : Main.c X3FwMerge.h
	gcc -c Main.c
