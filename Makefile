# King Cobra 빌드: make → ./kingcobra   /   make clean
CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -pedantic -O2
TARGET  = kingcobra
SOURCES = main.c Game.c Snake.c Food.c Screen.c LinkedList.c

$(TARGET): $(SOURCES) $(wildcard *.h)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe
