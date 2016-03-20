all:
	g++ -std=c++11 -O3 -o Solver Main.cpp
debug:
	g++ -std=c++0x -D_GLIBCXX_DEBUG -fsanitize=address -o Solver_debug -O2 Main.cpp
clean:
	rm Solver
	rm Solver_debug
