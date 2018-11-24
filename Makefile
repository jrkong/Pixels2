OPENCV_LIB := $(shell pkg-config --cflags --libs opencv4)
CC=g++ 
main: main.cpp ./util/util.cpp ./reduceToScale/reduce.cpp
	$(CC) main.cpp ./util/util.cpp ./reduceToScale/reduce.cpp -fopenmp -g -pg -o output/scaleImage $(OPENCV_LIB)

release: main.cpp ./util/util.cpp ./reduceToScale/reduce.cpp
	$(CC) main.cpp ./util/util.cpp ./reduceToScale/reduce.cpp -O2 -fopenmp -o output/scaleImage_release $(OPENCV_LIB)
