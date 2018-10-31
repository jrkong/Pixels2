OPENCV_LIB := $(shell pkg-config --cflags --libs opencv4)
CC=g++ 
main: main.cpp ./util/util.cpp ./reduceToScale/reduce.cpp ./util/loadpng.cpp
	$(CC) main.cpp ./util/util.cpp ./reduceToScale/reduce.cpp ./util/loadpng.cpp $(OPENCV_LIB)
