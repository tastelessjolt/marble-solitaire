objects = fltk.o main.o
target = main
CXX = g++


$(target): $(objects)
	g++ $(objects)  -o $(target) -lfltk -lfltk_images

fltk.o: bridge.h fltk.h

main.o: bridge.h main.h


clean: 
	rm -f $(objects) $(target)