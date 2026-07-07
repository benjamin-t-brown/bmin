CXX ?= g++
CXXFLAGS = -Wall -std=c++20 -O0 -g

SRCS = main.cpp bench_workers.cpp worker1.cpp worker2.cpp worker3.cpp worker4.cpp worker5.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = bench

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXE) $(EXE).exe
