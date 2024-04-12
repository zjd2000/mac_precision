CXX = g++  
CXXFLAGS = -Wall -g -std=c++17  
SRCS = main.cpp  
OBJS = $(patsubst %.cpp,build/%.o,$(SRCS))  
TARGET = build/main  
  
all: $(TARGET)  
  
$(TARGET): $(OBJS)  
	$(CXX) $(CXXFLAGS) -o $@ $^  
  
build/%.o: %.cpp | build  
	$(CXX) $(CXXFLAGS) -c $< -o $@  
  
build:  
	mkdir -p $@  
  
.PHONY: all clean  
  
clean:  
	rm -rf build