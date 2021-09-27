CXX = g++
CXXFLAGS = -g -Wall -std=c++11
GTEST_LL = -I /usr/include/gtest/ -l gtest -l gtest_main -pthread

bayes: bayes.o
	$(CXX) $(CXXFLAGS) $^ -o $@ 
bayes.o: bayes.cpp  
	$(CXX) $(CXXFLAGS) $(DEFS) -o $@ -c bayes.cpp

clean:
	rm -f *.o bayes