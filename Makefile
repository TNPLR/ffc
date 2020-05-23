CXX=g++
CXXFLAGS=-Wall -Wextra -std=c++17 -Wpedantic -fstack-protector-all

OBJS=lexer.o parser.o ast.o main.o enumprint.o analyzer.o

.PHONY: all clean
all: main
main: ${OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@
%.o: %.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@
clean:
	rm -f ${OBJS} main
