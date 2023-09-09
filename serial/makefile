CC = g++ -std=c++11 -c
CL = g++ -std=c++11

all: ImageFilters.out

ImageFilters.out: main.o
	$(CL) main.o -o ImageFilters.out

main.o: main.cpp
	$(CC) main.cpp -o main.o 


.PHONY: clean
clean:
	rm *.o 
	rm ImageFilters.out