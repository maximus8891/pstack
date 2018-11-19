prog: main.o
	g++ -std=c++11 -g -O0 -Wall -fPIC main.o -o prog

main.o: main.cpp stack.hpp
	g++ -std=c++11 -c -g -O0 -Wall -fPIC $<

.PHONY: clean
clean:
	rm *.o *.a *.so
