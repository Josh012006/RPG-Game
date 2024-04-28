all: compile link

compile:
	g++ -Isrc/include -c coding/*.cpp

link:
	g++ *.o -o main -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lsfml-audio