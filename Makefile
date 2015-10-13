CC = g++
CFLAGS = -Wall -g
SOURCES = src/main.cpp src/sieve.cpp src/Client.cpp
OBJECTS = $(SOURCES:.cpp=.o)
	LDFLAGS = -lboost_system-mt -lboost_thread-mt

all: main
	  
%.o: %.c
	  $(CC) $(CFLAGS) -o $@ -c $< $(LDFLAGS)
		
main: $(OBJECTS)
	  $(CC) $(OBJECTS) -o main $(CFLAGS) $(LDFLAGS)
		
clean:
	  @echo "Cleaning..."
		@rm -f $(OBJECTS) main
		  @echo "Done!"
			
.PHONY: clean deploy

