#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Shared_Image.H>

#include "config.h"
#include "strings.h"

using namespace std;

Fl_Shared_Image *open_resize_to(const char *filename, int w, int h)
{
    Fl_Shared_Image *img = Fl_Shared_Image::get(new Fl_PNG_Image(filename));
    Fl_Image *temp;
    if (img->w() > w || img->h() > h)
    {
        if (img->w() > img->h())
        {
            temp = img->copy(w, h * img->h() / img->w());
        }
        else
        {
            temp = img->copy(w * img->w() / img->h(), h);
        }
        img->release();
        img = (Fl_Shared_Image *)temp;
    }
    return img;
}

int main(int argc, char *argv[])
{
    Fl_Window *w = new Fl_Window(WINDOW_SIZE_W, WINDOW_SIZE_H);

    Fl_Box box((WINDOW_SIZE_W - BOARD_SIZE_W)/2, (WINDOW_SIZE_H - BOARD_SIZE_H)/2, BOARD_SIZE_W, BOARD_SIZE_H);
    box.image(open_resize_to(background, BOARD_SIZE_W, BOARD_SIZE_H));

    w->end();
    w->show(argc, argv);
    return Fl::run();
}
