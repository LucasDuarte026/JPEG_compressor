all:
	gcc  -o ./bin/main ./src/*.c   -Iinclude -lm 
full:
	gcc -Wall -o ./bin/main ./src/*.c   -Iinclude -lm

debug:
	gcc -g3 ./src/*.c -o ./bin/main -Iinclude -lm
fullDebug:
	gcc  -Wall -g3 ./src/*.c -o ./bin/main -Iinclude -lm
	
run:
	./bin/main