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

class Marble;
class Board;

class Marble : public Fl_Box
{
    Board *parent;

  public:
    int lbl;
    bool highlight;
    int xpos, ypos;
    Marble(Board *b, int xp, int yp, int x, int y);
    int handle(int);
};

class Board : Fl_Box
{
    bool board_structure[7][7] = {
        {0, 0, 1, 1, 1, 0, 0},
        {0, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 0, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 0},
        {0, 0, 1, 1, 1, 0, 0},
    };
    Marble *marbles[7][7];

  protected:
    void swap(int x1, int y1, int x2, int y2);
    bool win();

  public:
    int size;
    int n_steps;
    bool highlight;
    Board(int, int, int, int);
    int move(int, int, int);
    void randomize();
    void reset();
    ~Board();
};

Marble::Marble(Board *b, int xp, int yp, int x, int y) : 
    Fl_Box(x, y, MARBLE_SIZE_W, MARBLE_SIZE_H)
{
    image(open_resize_to(marble, MARBLE_SIZE_W, MARBLE_SIZE_H));
    parent = b;
    xpos = xp;
    ypos = yp;
}

int Marble::handle(int event)
{
    if (event == FL_PUSH)
    {
        parent->move(xpos, ypos, lbl);
    }
    return 1;
};

Board::Board(int x0, int y0, int pw, int ph) :
    Fl_Box(x0, y0, pw, ph)
{
    image(open_resize_to(background, BOARD_SIZE_W, BOARD_SIZE_H));

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
}

void Board::reset()
{
    int temp;
    n_steps = 0;
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 7; j++)
        {   

        }
    }
}

Board::~Board()
{
    // do clean up
}

void Board::swap(int x1, int y1, int x2, int y2)
{

}

int Board::move(int x, int y, int mylabl)
{
    /*
    // Get possible direction of movement
    Direction dir = getDirection(x, y);
    if (dir == INVALID)
    {
        cout << "Invalid move" << endl;
        return 1;
    }
    else
    {
        const char *arr[] = {"RIGHT", "UP", "LEFT", "DOWN"};
        cout << arr[(int)dir] << endl;
    }

    int arr_x[] = {+1, 0, -1, 0};
    int arr_y[] = {0, -1, 0, +1};
    Direction arr_dirs[] = {RIGHT, UP, LEFT, DOWN};

    // swap marbles
    swap(x, y, x + arr_x[dir], y + arr_y[dir]);
    n_steps++;

    if (win())
    {
        fl_message("You did well, you finished in %d steps", n_steps);
        cout << "You finished in " << n_steps << " steps" << endl;
        n_steps = 0;
    }
    */
    return 0;
}

// check if board is in solved state
bool Board::win()
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (marbles[i][j]->lbl != i + j * size + 1)
            {
                return false;
            }
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    Fl_Window *w = new Fl_Window(WINDOW_SIZE_W, WINDOW_SIZE_H);

    Board board((WINDOW_SIZE_W - BOARD_SIZE_W)/2, (WINDOW_SIZE_H - BOARD_SIZE_H)/2, BOARD_SIZE_W, BOARD_SIZE_H);

    w->end();
    w->show(argc, argv);
    return Fl::run();
}
