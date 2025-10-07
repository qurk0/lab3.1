.PHONY: all run clean

all: main.cpp
	g++ main.cpp -o result

run: result
	./result

clean:
	rm -f result