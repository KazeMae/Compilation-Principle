TARGET=Tcompiler.exe
CXX = g++
EXEFLAGS = -Wall -g -std=c++20
CXXFLAGS = -Wall -g -std=c++20 -c
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(EXEFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	del $(OBJS) $(TARGET) -rf

.PHONY: all clean
