#ifndef FLKT_BRIDGE_H
#define FLKT_BRIDGE_H

#include "bridge.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Shared_Image.H>

class FlTk_Window : public Window, public Fl_Window
{
public:
	FlTk_Window(int w, int h) : Fl_Window(w, h) {}
	~FlTk_Window() {}
	virtual void end() {
		Fl_Window::end();
	}
	virtual void label(const char* lbl) {
		Fl_Window::label(lbl);
	}
	virtual void show(int argc, char *argv[]) {
		Fl_Window::show(argc, argv);
	}
	virtual void showMessage(const char *msg) {
		fl_message(msg);
	}
};

class FLTk_MainLoop : public MainLoop
{
public: 
	virtual int run() {
		return Fl::run();
	}
};

class FlTk_Image : public Image, public Fl_Shared_Image
{
public:
	FlTk_Image(const char *filename, int w, int h) {
		Fl_Shared_Image* im = Fl_Shared_Image::get(new Fl_PNG_Image(filename));
		Fl_Image *temp;
		if (im->w() > w || im->h() > h)
		{
		    if (im->w() > im->h())
		    {
		        temp = im->copy(w, h * im->h() / im->w());
		    }
		    else
		    {
		        temp = im->copy(w * im->w() / im->h(), h);
		    }
		    im->release();
		    im = (Fl_Shared_Image *)temp;
		}
		img = (void *) im;
	}
};

class FlTk_ImageView : public ImageView, public Fl_Box {
public: 
	FlTk_ImageView(int x, int y, int w, int h) : Fl_Box(x, y, w, h) {}

	virtual int handle(int e)  {
		if (_handler != NULL) {
			return _handler->handle(e);
		}
		return 0;
	}
	virtual void redraw() {
		Fl_Box::redraw();
	}
	virtual void image(Image *img) {
		Fl_Box::image((Fl_Shared_Image*) img->getPlatformImage());
	}
};

class FlTk_Creator : public Creator {
	public:
	virtual Window* makeWindow(int w, int h) {
		return new FlTk_Window(w, h);
	}
	virtual ImageView* makeImageView(int x, int y, int w, int h) {
		return new FlTk_ImageView(x, y, w, h);
	}
	virtual Image* makeImage(const char *filename, int w, int h) {
		return new FlTk_Image(filename, w, h);
	}
	virtual MainLoop* makeMainLoop() {
		return new FLTk_MainLoop();
	}
};

#endif // FLKT_BRIDGE_H
