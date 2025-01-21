g++ -c Client.cpp -I ..\include\nlohmann -I ..\include -o Client.o -std=c++17 -lws2_32
ar rcs libClient.lib *.o
move libClient.lib ..\lib
del *.o