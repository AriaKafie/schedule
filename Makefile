# CXX Make variable for compiler
CC=g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CCFLAGS = -fpermissive -std=c++17 -w -c

# object files
OBJS = types.o log.o main.o 

# Program name
PROGRAM = schedule

# The program depends upon its object files
$(PROGRAM) : $(OBJS)
	$(CC) -pthread -o $(PROGRAM) $(OBJS)

main.o : main.cpp types.h
	$(CC) $(CCFLAGS) main.cpp

log.o : log.cpp log.h
	$(CC) $(CCFLAGS) log.cpp

types.o : types.cpp types.h
	$(CC) $(CCFLAGS) types.cpp

# Once things work, people frequently delete their object files.
# If you use "make clean", this will do it for you.
# As we use gnuemacs which leaves auto save files termintating
# with ~, we will delete those as well.
clean :
	rm -f *.o *~ $(PROGRAM)

