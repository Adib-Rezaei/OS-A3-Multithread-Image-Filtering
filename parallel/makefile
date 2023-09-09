CC = g++ -std=c++11 -pthread -c
CL = g++ -std=c++11 -pthread

all: ImageFilters.out

ImageFilters.out: main.o
	$(CL) main.o -o ImageFilters.out

main.o: main.cpp
	$(CC) main.cpp -o main.o 


.PHONY: clean
clean:
	rm *.o 
	rm ImageFilters.out