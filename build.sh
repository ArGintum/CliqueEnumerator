
lst=$(python3-config --includes)
arr=($lst)
mkdir objs
g++ -g -fPIC ${arr[0]} -o ./objs/CliqueEnumerator.o -c cliques-1.cpp
g++ -fPIC -g -shared -o CliqueEnumerator.so ./objs/CliqueEnumerator.o -lboost_python3 -lboost_numpy3
