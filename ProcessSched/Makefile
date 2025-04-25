CXX = g++
CXXFLAGS = -std=c++14 -g
INCLUDES = -I.

SRCS = master.cpp Algorithms.cpp EventControl.cpp Randomizer.cpp
OBJS = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

final: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -rf final $(OBJS)