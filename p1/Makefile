CXX:=g++ --std=c++11 -g

includes = $(wildcard *.h)

all: tests main timing

tests: $(includes) tests.cpp
	$(CXX) -lefence -o tests tests.cpp

timing: $(includes) tests.cpp
	$(CXX) -O3 -DUSE_TIMERS -o timing tests.cpp

callgrind: timing
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./timing

input.txt:
	./makeinput.py

profile: main input.txt
	cat input.txt | valgrind --tool=callgrind --cache-sim=yes --dump-instr=yes ./main > output.txt

walltime: main input.txt
	time cat input.txt| ./main > output.txt

main: $(includes) main.cpp
	$(CXX) -O3 -o main main.cpp

clean:
	rm -f tests main timing callgrind.* input.txt output.txt
