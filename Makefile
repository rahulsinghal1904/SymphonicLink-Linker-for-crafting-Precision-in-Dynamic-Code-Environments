linker: linker.cpp
	g++ -g -std=c++14 linker.cpp -o linker
clean:
	rm -f linker *~