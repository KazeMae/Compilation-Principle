TARGET=Tcompiler.exe
CXX = g++
EXEFLAGS = -Wall -g -std=c++20
CXXFLAGS = -Wall -g -std=c++20 -c
SRCS = $(wildcard *.cpp)
BUILD_DIR = build
OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(EXEFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	del -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean