# -o outpute_file_name
# -c files_to_compile
# @echo "SOME_TEXT" - print text

CC = gcc
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./dependencies/ -I./src/include -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl

OBJECTS = main.o glad.o shader.o camera.o

cglab: $(OBJECTS)
	@echo "Building Computer Graphics lab."
	$(CXX) $(CXXFLAGS) -o cg build/main.o build/glad.o build/shader.o build/camera.o

main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o build/main.o

glad.o: src/glad.c
	$(CXX) $(CXXFLAGS) -c src/glad.c -o build/glad.o

shader.o: src/shader.cpp src/include/shader.hpp
	$(CXX) $(CXXFLAGS) -c src/shader.cpp -o build/shader.o

camera.o: src/camera.cpp src/include/camera.hpp
	$(CXX) $(CXXFLAGS) -c src/camera.cpp -o build/camera.o


# .PHONY: clean
# clean:
# 	@echo "Cleaning object files"
# 	rm $(OBJECTS)