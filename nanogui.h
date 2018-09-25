#ifndef NGUI_BRIDGE_H
#define NGUI_BRIDGE_H

#include "bridge.h"
#include <nanogui/common.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/imageview.h>

class NanoGUI_Window : public Window, public nanogui::Window
{
public:
	NanoGUI_Window(Widget* parent, int w, int h) : nanogui::Window(parent, "") {}
	~NanoGUI_Window() {}
	virtual void end() {
        nanogui::Screen::performLayout();
	}
	virtual void label(const char* lbl) {
		nanogui::Screen::setCaption(lbl);
	}
	virtual void show(int argc, char *argv[]) {
		nanogui::Screen::setVisible(true);
		// Fl_Window::show(argc, argv);
	}
	virtual void showMessage(const char *msg) {
		fl_message(msg);
	}
};

class NanoGUI_MainLoop : public MainLoop
{
public: 
	virtual int run() {
		nanogui::mainloop();
        return 0;
	}
};

class NanoGUI_Image : public Image, public Fl_Shared_Image
{
public:
	NanoGUI_Image(const char *filename, int w, int h) {
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

class NanoGUI_ImageView : public ImageView, public nanogui::ImageView {
public: 
	NanoGUI_ImageView(int x, int y, int w, int h) : Fl_Box(x, y, w, h) {
        
    }

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

class NanoGUI_Creator : public Creator {
    nanogui::Screen *sc;
    nanogui::Window *curr_win;
	public:
    NanoGUI_Creator() {
        nanogui::init();
        sc = new nanogui::Screen(Eigen::Vector2i(1024, 768), "");
        curr_win = NULL;
    }
	virtual Window* makeWindow(int w, int h) {
		curr_win = new NanoGUI_Window(sc, w, h);
	}
	virtual ImageView* makeImageView(int x, int y, int w, int h) {
		return new NanoGUI_ImageView(curr_win, x, y, w, h);
	}
	virtual Image* makeImage(const char *filename, int w, int h) {
		return new NanoGUI_Image(filename, w, h);
	}
	virtual MainLoop* makeMainLoop() {
		return new NanoGUI_MainLoop();
	}
};

#endif // NGUI_BRIDGE_H
w