CXX = g++
CPPFLAGS = -Iglad/include
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDLIBS = -lglfw -lGL -ldl

TARGET = app

SRC = Main.cpp Application.cpp Window.cpp ShaderProgram.cpp Model.cpp Scene.cpp DrawableObject.cpp Transformation.cpp Camera.cpp

HEADERS = Light.h Application.h Window.h ShaderProgram.h Model.h Scene.h DrawableObject.h Transformation.h Camera.h $(wildcard models/*.h) glad/include/glad/gl.h
all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS) Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

.PHONY: all run