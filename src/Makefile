# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall  -fPIC
# INCLUDES = -I
LFLAGS =
LIBS = -lpthread -lrt

# the build target:
TARGET = test1

SOURCES = test1.c gecko_interface.c list.c log.c serial_core.c serial_main.c timer_api.c gpio.c

OBJECTS = $(SOURCES:.c=.o) 



all:    $(TARGET) 
	@echo  $(TARGET) has been built 

$(TARGET): $(OBJECTS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJECTS) $(LFLAGS) $(LIBS) 

.c.o: $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(TARGET)


