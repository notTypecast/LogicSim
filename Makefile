CC=g++
CFLAGS=-O3 -I ./include
ALL_SRCS=$(wildcard src/*.cpp)
ALL_OBJS=$(patsubst src/%.cpp,bin/%.o, $(ALL_SRCS))
ALL_TESTS=$(wildcard tests/*.cpp)
ALL_TESTS_BIN=$(patsubst tests/%.cpp,bin/%, $(ALL_TESTS))

build: main.cpp $(ALL_OBJS)
	$(CC) $(CFLAGS) -o bin/main main.cpp $(ALL_OBJS)

run: bin/main
	./bin/main

build-tests: $(ALL_TESTS_BIN)

$(ALL_TESTS_BIN): bin/%: tests/%.cpp $(ALL_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(ALL_OBJS)

bin/%.o: src/%.cpp include/%.hpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f bin/*