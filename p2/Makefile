CXX:=g++ --std=c++11 -g

includes = $(wildcard *.h)
cppfiles = ITCH5Reader.cpp StatsCounter.cpp L2Book.cpp

all: tests main timing decode

tests: $(includes) $(cppfiles) tests.cpp
	$(CXX) -lefence -DUSE_COUNTERS -o tests tests.cpp $(cppfiles)

timing: $(includes) $(cppfiles) main.cpp
	$(CXX) -O3 -DUSE_CALLGRIND -DUSE_TIMERS -o timing main.cpp $(cppfiles)

callgrind: timing
	valgrind --tool=callgrind --cache-sim=yes --instr-atstart=no --dump-instr=yes ./timing AMZN data/12302015.NASDAQ_ITCH50 out.csv

main: $(includes) $(cppfiles) main.cpp
	$(CXX) -O3 -o main main.cpp $(cppfiles)

decode: $(includes) $(cppfiles) main.cpp
	$(CXX) -O3 -DWRITE_FEED -DUSE_COUNTERS -o decode main.cpp $(cppfiles)

clean:
	rm -f tests main timing decode callgrind.* *.csv *.log

tar: clean
	cd .. && tar zcvf p2.tar.gz ./p2 --exclude=data --exclude zip
