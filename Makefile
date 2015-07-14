Target = mustvanq
Sources = src/*.cpp

WARNINGS = -Wall -Wno-write-strings
CXXFLAGS = ${WARNINGS} -g

default:
	${CXX} ${CXXFLAGS} ${Sources} -o ${Target}
