mp1_node : main.o Node.o helpers.o message.o
	g++ main.o Node.o helpers.o message.o -pthread -std=c++1y -lm -o mp1_node
main.o : main.cpp
	g++ -std=c++1y -c -g -O0 -Wall -Wextra -pedantic main.cpp
Node.o : Node.h Node.cpp
	g++ -std=c++1y -c -g -O0 -Wextra -pedantic Node.cpp
helpers.o : helpers.h helpers.cpp
	g++ -std=c++1y -c -g -O0 -Wextra -pedantic helpers.cpp
message.o : message.h message.cpp
	g++ -std=c++1y -c -g -O0 -Wextra -pedantic message.cpp

clean:
	rm -rf *.o map test