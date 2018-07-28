#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Shared_Image.H>

#include "config.h"
#include "strings.h"

#include <vector>

using namespace std;

Fl_Image *blank_;
Fl_Image *marble_;
Fl_Image *marble_sel_;
Fl_Image *background_;

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

class Marble;
class Board;

class Marble : public Fl_Box
{
    Board *parent;
    Fl_Image *img;

  public:
    bool highlight;
    bool hidden;
    int xpos, ypos;
    Marble(Board *b, int xp, int yp, int x, int y);
    void select();
    void unselect();
    int handle(int);
    void show_img();
    void hide_img();
};

class Board : Fl_Box
{
    bool board_structure[7][7] = {
        {0, 0, 1, 1, 1, 0, 0},
        {0, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 0},
        {0, 0, 1, 1, 1, 0, 0},
    };
    Marble *marbles[7][7];
    vector<Marble*> highlighted;
    Marble *last_marble;

  protected:
    void swap(int x1, int y1, int x2, int y2);
    int check_status();
    int num_neighbours (int x, int y);

  public:
    int size;
    int n_steps;
    Board(int, int, int, int);
    int move(int, int);
    void randomize();
    void reset();
    ~Board();
};

Marble::Marble(Board *b, int xp, int yp, int x, int y) : 
    Fl_Box(x, y, MARBLE_SIZE_W, MARBLE_SIZE_H)
{
    image(marble_);
    align(FL_ALIGN_INSIDE);
    hidden = false;
    parent = b;
    xpos = xp;
    ypos = yp;
}

void Marble::show_img()
{
    image(marble_);
    redraw();
    hidden = false;
}

void Marble::hide_img()
{
    image(blank_);
    redraw();
    hidden = true;
}

int Marble::handle(int event)
{
    if (event == FL_PUSH)
    {
        parent->move(xpos, ypos);
    }
    return 1;
};

void Marble::select() {
    image(marble_sel_);
    redraw();
}

void Marble::unselect() {
    image (blank_);
    redraw();
    hidden = true;
}

Board::Board(int x0, int y0, int pw, int ph) : Fl_Box(x0, y0, pw, ph)
{
    image(background_);

    n_steps = 0;
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            if (board_structure[i][j]) {
                marbles[i][j] = new Marble(this, i, j, x0 + OFFSET_W + i * DISTANCE_BETWEEN_HOLECENTERS_W, y0 + OFFSET_H + j * DISTANCE_BETWEEN_HOLECENTERS_H);
            }
            else {
                marbles[i][j] = NULL;
            }
        }
    }
    marbles[3][3]->hide_img();
}

void Board::reset()
{
    n_steps = 0;
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            if (board_structure[i][j])
            {
                marbles[i][j]->show_img();
            }
        }
    }
    marbles[3][3]->hide_img();
}

Board::~Board()
{
    // do clean up
}

bool in_between (int a, int i, int j) {
    return a >= i && a < j;
}

int Board::move(int x, int y)
{
    // cout << "Clicked " << x << ", " << y << endl;
    int arr_x[] = {2, 0, -2, 0};
    int arr_y[] = {0, -2, 0, 2};

    // First click
    if ( highlighted.empty() ) {
        if (marbles[x][y]->hidden) {
            return 0;
        }

        Marble *target;
        Marble *middle;
        for (int i = 0; i < 4; i++) {
            if (in_between(x + arr_x[i], 0, 7) && in_between(y + arr_y[i], 0, 7)) {
                target = marbles[x + arr_x[i]][y + arr_y[i]];
                middle = marbles[x + arr_x[i]/2][y + arr_y[i]/2];
                if (middle != NULL && !middle->hidden) {
                    if (target != NULL && target->hidden) {
                        highlighted.push_back(target);
                        target->highlight = true;
                        target->select();
                    }
                }
            }
        }
        last_marble = marbles[x][y];
    }

    // Second Click
    else {
        for (Marble *mar: highlighted) {
            if (mar->xpos == x && mar->ypos == y) {
                if (mar->highlight) {
                    mar->highlight = false;
                    mar->show_img();

                    last_marble->hide_img();
                    marbles[(last_marble->xpos + x) / 2][(last_marble->ypos + y)/2]->hide_img();
                }
                else {
                    mar->unselect();
                }
            }
            else {
                mar->unselect();
            }
        }
        highlighted.clear();

        int stat = check_status();
        if (stat > 0 /* win */) {
            fl_message("Congrats! You finished it!");
        }
        else if (stat < 0 /* lose */) {
            fl_message("Sorry, you're out of moves :(");
            reset();
        }
    }
    
    return 0;
}

int Board::num_neighbours (int x, int y) {
    int arr_x[] = {1, 0, -1, 0};
    int arr_y[] = {0, -1, 0, 1};

    int count = 0;
    Marble *curr;
    for (int i = 0; i < 4; i++) {
        if (in_between(x + arr_x[i], 0, 7) && in_between(y + arr_y[i], 0, 7) ) {
            curr = marbles[x + arr_x[i]][y + arr_y[i]];
            if (curr != NULL && !curr->hidden)
                count++;
        }
    }
    return count;
}

// check if board is in solved state
int Board::check_status()
{
    int count = 0; 
    int adjacent_count = 0;
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            if (board_structure[i][j])
            {
                if (!marbles[i][j]->hidden) {
                    count ++;
                    adjacent_count += num_neighbours(i, j);
                }
            }
        }
    }

    adjacent_count /= 2;
    
    if (count == 1)
        return 1;
    
    if (adjacent_count == 0) 
        return -1;

    return 0;
}

void load_images () {
    blank_ = open_resize_to(blank, MARBLE_SIZE_W, MARBLE_SIZE_H);
    marble_ = open_resize_to(marble, MARBLE_SIZE_W, MARBLE_SIZE_H);
    marble_sel_ = open_resize_to(marble_selected, MARBLE_SIZE_W, MARBLE_SIZE_H);
    background_ = open_resize_to(background, BOARD_SIZE_W, BOARD_SIZE_H);
}

int main(int argc, char *argv[])
{
    load_images();

    Fl_Window *w = new Fl_Window(WINDOW_SIZE_W, WINDOW_SIZE_H);
    w->label("Marble Solitaire");

    Board board((WINDOW_SIZE_W - BOARD_SIZE_W)/2, (WINDOW_SIZE_H - BOARD_SIZE_H)/2, BOARD_SIZE_W, BOARD_SIZE_H);

    w->end();
    w->show(argc, argv);
    return Fl::run();
}
