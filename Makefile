OBJ = shell
override CFLAGS += -Ofast -funroll-loops -flto -Wall
override DEVEL_FLAGS += -g -Wall -Wextra -pedantic-errors 
override DEBUG_FLAGS += -DDEBUG
override LFLAGS += 
CC = gcc
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin

%.o : %.cpp
	$(CC) $(DEVEL_FLAGS) -c $^

$(OBJ) : *.o
	$(CC) $(DEVEL_FLAGS) $^ -o $@ $(LFLAGS)

clean :
	rm -f *.o $(OBJ)

clear : clean

debug: clear
	$(CC) $(DEVEL_FLAGS) $(DEBUG_FLAGS) -c *.cpp
	$(CC) $(DEVEL_FLAGS) $(DEBUG_FLAGS) -o $(OBJ) *.o $(LFLAGS)

ofast : clear
	$(CC) $(CFLAGS) -c *.cpp
	$(CC) $(CFLAGS) -o $(OBJ) *.o $(LFLAGS)

install : $(OBJ)
	@echo "Installing program to $(DESTDIR)$(BINDIR) ..."
	@mkdir -p $(DESTDIR)$(BINDIR)
	@install -pm0755 $(OBJ) $(DESTDIR)$(BINDIR)/$(TARGET) || \
		echo "Failed. Try "make PREFIX=~ install" ?"

all : install
