all:
	g++  -o ./bin/main ./src/*.c   -Iinclude -lm 
full:
	g++ -Wall -o ./bin/main ./src/*.c   -Iinclude -lm

debug:
	g++ -g3 ./src/*.c -o ./bin/main -Iinclude -lm
fullDebug:
	g++  -Wall -g3 ./src/*.c -o ./bin/main -Iinclude -lm
	
run:
	./bin/main