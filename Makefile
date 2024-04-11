CC=g++
CFLAGS=-O3 -I ./include

main: main.cpp bin/component.o bin/gates.o bin/inputs.o bin/outputs.o bin/memory.o bin/simulator.o
	$(CC) $(CFLAGS) -o bin/main main.cpp bin/component.o bin/gates.o bin/inputs.o bin/outputs.o bin/simulator.o bin/memory.o

run: bin/main
	./bin/main

bin/component.o: src/component.cpp include/component.hpp
	$(CC) $(CFLAGS) -c -o bin/component.o src/component.cpp

bin/gates.o: src/gates.cpp include/gates.hpp
	$(CC) $(CFLAGS) -c -o bin/gates.o src/gates.cpp

bin/inputs.o: src/inputs.cpp include/inputs.hpp
	$(CC) $(CFLAGS) -c -o bin/inputs.o src/inputs.cpp

bin/outputs.o: src/outputs.cpp include/outputs.hpp
	$(CC) $(CFLAGS) -c -o bin/outputs.o src/outputs.cpp

bin/memory.o: src/memory.cpp include/memory.hpp
	$(CC) $(CFLAGS) -c -o bin/memory.o src/memory.cpp

bin/simulator.o: src/simulator.cpp include/simulator.hpp
	$(CC) $(CFLAGS) -c -o bin/simulator.o src/simulator.cpp

clean:
	rm -f bin/*