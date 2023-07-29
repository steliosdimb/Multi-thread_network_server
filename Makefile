# List the names of your C++ source files here (without the .cpp extension)
SRCS := server master_thread workers_threads global

# Specify the g++ compiler
CXX := g++

# Compile flags (none in this case)
CXXFLAGS := -pthread

# List the names of your header files (with .hpp extension)
HEADERS := server.hpp workers_threads.hpp global.hpp

# List the names of your object files (generated from your source files)
OBJS := $(SRCS:=.o)

# Name of your executable (change this to whatever you want)
EXEC := poller

# Default target
all: $(EXEC)

# Rule to link the object files into an executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXEC)

# Rule to compile each source file into an object file
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Remove generated files
clean:
	rm -f $(OBJS) $(EXEC)
