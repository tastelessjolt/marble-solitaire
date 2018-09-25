objects = fltk.o nanogui.o main.o
target = main
CXX = g++


$(target): $(objects)
	g++ $(objects) -o $(target) -lfltk -lfltk_images

fltk.o: bridge.h fltk.h
nanogui.o: bridge.h nanogui.h
main.o: bridge.h main.h
	g++ -I/usr/include/eigen3/ main.cpp -o main.o


clean: 
	rm -f $(objects) $(target)