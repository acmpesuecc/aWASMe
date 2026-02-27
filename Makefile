build: 
	g++ -o main -I./include/engine src/engine/main.cpp src/engine/vm.cpp src/engine/value.cpp src/engine/errors.cpp src/engine/instruction.cpp src/engine/funcs.cpp

run:
	./main
