CXX ?= g++
CXXFLAGS = -Wall -std=c++23 -O0 -g
BMIN_DIR = ../../bmin
INCLUDES = -I$(BMIN_DIR)/include
LIBS = -L$(BMIN_DIR)/lib -lbmin

SRCS = main.cpp bench_workers.cpp worker1.cpp worker2.cpp worker3.cpp worker4.cpp worker5.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = bench

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXE) $(EXE).exe
