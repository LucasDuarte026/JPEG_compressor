all:
	gcc  -o ./bin/main ./src/*.c ./include/*.h 
full:
	gcc -Wall -o ./bin/main ./src/*.c ./include/*.h 


debug:
	gcc  -g3 ./src/*.c -o ./bin/main ./src/*.c ./include/*.h 
fullDebug :
	gcc  -Wall -g3 ./src/*.c -o ./bin/main ./src/*.c ./include/*.h 
	
run:
	./bin/main