CXX = g++
CXXFLAGS = -Wall -g -std=c++11
LIBS = -l sqlite3
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp = .o)
DEPS = cardmatic_sql.h cardmatic_dataconverter.h
TARGET = cardmaticsql

# create executable from object files
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)
	
# compile object files if there is a change
%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: clean
clean:
	$(RM) *.o $(TARGET)
