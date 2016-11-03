test : test.exe
	./test/test.exe

test.exe: split.hpp test/test.cpp
	$(CXX) -I. -std=c++11 -o test/test.exe test/test.cpp

