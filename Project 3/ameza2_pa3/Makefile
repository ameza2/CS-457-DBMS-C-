TARGET = db
LIBS = -lm # Math Library, just a placeholder
SRCS = db.cpp
OBJECTS := $(patsubst %.cpp,%.o,$(SRCS))
CXX = g++
CXX_FLAGS = -Wall -std=c++11 # C++11 just for reference, not necessary

.PHONY: default all clean

all: $(TARGET)
	$(CXX) $(OBJECTS) -o $(TARGET) $(CXX_FLAGS)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(CXX_FLAGS) $(OBJECTS) $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
	