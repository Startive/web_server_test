default:
	g++ -o server -lpthread -Wall -Werror -fstack-protector-strong src/*.cpp

debug:
	g++ -o server_debug -lpthread -Wall -Werror -g -fstack-protector-strong src/*.cpp

run:
	./server_debug