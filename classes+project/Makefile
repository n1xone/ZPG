CXX = g++
CPPFLAGS = -Iglad/include
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDLIBS = -lglfw -lGL -ldl

TARGET = app

SRC = ObjLoader.cpp Main.cpp Application.cpp Window.cpp ShaderProgram.cpp Model.cpp Scene.cpp SceneRender.cpp SceneInteraction.cpp DrawableObject.cpp Transformation.cpp Camera.cpp Texture2D.cpp Cubemap.cpp

HEADERS = ObjLoader.h Light.h Application.h Window.h ShaderProgram.h Model.h Scene.h DrawableObject.h Transformation.h Camera.h $(wildcard models/*.h) glad/include/glad/gl.h Texture2D.h Cubemap.h
SHADERS = $(wildcard shaders/*)
all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS) $(SHADERS) Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

.PHONY: all run