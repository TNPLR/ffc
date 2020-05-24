CXX=g++
CXXFLAGS=-Wall -Wextra -std=c++17 -Wpedantic -fstack-protector-all \
	-D_FORTIFY_SOURCE=2 -D_GLIBCXX_ASSERTIONS -fasynchronous-unwind-tables \
	-fexceptions -fPIE -Wl,-pie -fplugin=annobin \
	-fstack-clash-protection -Wl,-z,relro -O2 \
	-ftree-loop-vectorize -flto

OBJS=lexer.o parser.o ast.o main.o enumprint.o analyzer.o vartype.o

.PHONY: all clean
all: main
main: ${OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@
%.o: %.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@
clean:
	rm -f ${OBJS} main
