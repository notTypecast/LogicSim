CC=g++
CFLAGS=-O3 -I ./include

main: main.cpp bin/gates.o bin/inputs.o bin/simulator.o
	$(CC) $(CFLAGS) -o bin/main main.cpp bin/gates.o bin/inputs.o bin/simulator.o

run: bin/main
	./bin/main

bin/gates.o: src/gates.cpp include/gates.hpp
	$(CC) $(CFLAGS) -c -o bin/gates.o src/gates.cpp

bin/inputs.o: src/inputs.cpp include/inputs.hpp
	$(CC) $(CFLAGS) -c -o bin/inputs.o src/inputs.cpp

bin/simulator.o: src/simulator.cpp include/simulator.hpp
	$(CC) $(CFLAGS) -c -o bin/simulator.o src/simulator.cpp

clean:
	rm -f bin/*