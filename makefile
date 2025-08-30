all:
	g++  -o ./bin/main ./src/*.c   -Iinclude
full:
	g++ -Wall -o ./bin/main ./src/*.c   -Iinclude


debug:
	g++  -g3 ./src/*.c  -o ./bin/main ./src/*.c   -Iinclude
fullDebug :
	g++  -Wall -g3 ./src/*.c  -o ./bin/main ./src/*.c   -Iinclude
	
run:
	./bin/main