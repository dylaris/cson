all: eg1 eg2 eg3 eg4

eg1: eg1.c
	gcc -Wall -Wextra -std=c99 -I.. -o eg1 eg1.c

eg2: eg2.c
	gcc -Wall -Wextra -std=c99 -I.. -o eg2 eg2.c

eg3: eg3.cc
	g++ -Wall -Wextra -std=c++14 -I.. -o eg3 eg3.cc

eg4: eg4.cc
	g++ -Wall -Wextra -std=c++14 -I.. -o eg4 eg4.cc

clean:
	rm -f eg1 eg2 eg3 eg4 person.json

.PHONY: all clean
