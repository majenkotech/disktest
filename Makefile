all: dfill dtest

dfill: disktest.o
	cc -o dfill disktest.o

dtest: disktest.o
	cc -o dtest disktest.o
