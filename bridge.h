#ifndef BRIDGE_H
#define BRIDGE_H
#include <iostream>

class Window
{
public:
	virtual void end() = 0;
	virtual void label(const char* lbl) = 0;
	virtual void show(int argc, char *argv[]) = 0;
	virtual void showMessage(const char *msg) = 0;
};

class Handler
{
	public:
	virtual int handle(int) {return 0;}
};

class Image
{
protected:
	void *img;
public:
	Image() {}
	~Image() {}
	void* getPlatformImage() {
		return img;
	}
};

class ImageView
{
protected:
	Handler *_handler;
public:
	ImageView() {
		_handler = NULL;
	}
	~ImageView() {}

	void setHandler(Handler *h) {
		this->_handler = h;
	}

	virtual void redraw() = 0;
	virtual void image(Image *img) = 0;
};

class MainLoop
{
public:
	virtual int run() = 0;
};

class Creator
{
	public:
	virtual Window* makeWindow(int w, int h) = 0;
	virtual ImageView* makeImageView(int x, int y, int w, int h) = 0;
	virtual Image* makeImage(const char *filename, int w, int h) = 0;
	virtual MainLoop* makeMainLoop() = 0;
};

#endif // BRIDGE_H