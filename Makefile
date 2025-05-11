CXX = g++
CXXFLAGS = -std=c++11 
LDFLAGS = -lssl -lcrypto

SRC_DIR = src
BIN_DIR = bin

SRCS = $(SRC_DIR)/validation.cpp $(SRC_DIR)/merkle.cpp $(SRC_DIR)/hash.cpp 
OBJS = $(SRCS:.cpp=.o)
TARGET = $(BIN_DIR)/validation

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

run:
	./$(TARGET)

clean:
	rm -f $(BIN_DIR)/validation
