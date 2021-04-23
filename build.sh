lst=$(python3-config --includes)
arr=($lst)
g++ -g -fPIC ${arr[0]} -o ./objs/CliqueEnumerator.o -c CliqueEnumerator.cpp
g++ -fPIC -g -shared -o ./lib/CliqueEnumerator.so ./objs/CliqueEnumerator.o -lboost_python3 -lboost_numpy3
