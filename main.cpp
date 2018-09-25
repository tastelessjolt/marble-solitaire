#include <iostream>

#include <Eigen/Dense>

#include "config.h"
#include "strings.h"
#include "main.h"
#include "fltk.h"
#include "nanogui.h"

#include <vector>

using namespace std;

Creator *creator;

Image *blank_;
Image *marble_;
Image *marble_sel_;
Image *background_;


Marble::Marble(Board *b, int xp, int yp, int x, int y)
{
    imgView = creator->makeImageView(x, y, MARBLE_SIZE_W, MARBLE_SIZE_H);
    imgView->setHandler((Handler*)this);
    imgView->image(marble_);
    // align(FL_ALIGN_INSIDE);
    hidden = false;
    parent = b;
    xpos = xp;
    ypos = yp;
}

// Shows the Marble Image
void Marble::show_img()
{
    imgView->image(marble_);
    imgView->redraw();
    hidden = false;
}

// Shows the Marble Hole Image
void Marble::hide_img()
{
    imgView->image(blank_);
    imgView->redraw();
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

// Shows the Selected Marble Hole Image
void Marble::select() {
    imgView->image(marble_sel_);
    imgView->redraw();
}

// Shows the Marble Hole Image
void Marble::unselect() {
    imgView->image (blank_);
    imgView->redraw();
    hidden = true;
}

Board::Board(Window *w, int x0, int y0, int pw, int ph)
{
    // Board Image load
    this->w = w;
    imgView = creator->makeImageView(x0, y0, pw, ph);
    imgView->image(background_);

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

// Resets board to initial position
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
    highlighted.clear();
}

Board::~Board()
{
    // do clean up
}

// helper function
bool in_between (int a, int i, int j) {
    return a >= i && a < j;
}

int Board::move(int x, int y)
{
    // cout << "Clicked " << x << ", " << y << endl;
    int arr_x[] = {2, 0, -2, 0};
    int arr_y[] = {0, -2, 0, 2};

    // First click
    // Stores the possible next positions
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
    // Checks if the possible position is clicked
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
            w->showMessage("Congrats! You finished it!");
            reset();
        }
        else if (stat < 0 /* lose */) {
            w->showMessage("Sorry, you're out of moves :(. Try again. Fighting! :)");
            reset();
        }
    }
    
    return 0;
}

// helper function for calcutating number of adjacent pairs
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

// preload all images used
void load_images () {
    blank_ = creator->makeImage(blank, MARBLE_SIZE_W, MARBLE_SIZE_H);
    marble_ = creator->makeImage(marble, MARBLE_SIZE_W, MARBLE_SIZE_H);
    marble_sel_ = creator->makeImage(marble_selected, MARBLE_SIZE_W, MARBLE_SIZE_H);
    background_ = creator->makeImage(background, BOARD_SIZE_W, BOARD_SIZE_H);
}

int main(int argc, char *argv[])
{
    // creator = new FlTk_Creator();
    creator = new NanoGUI_Creator();
    load_images();

    Window *w = creator->makeWindow(WINDOW_SIZE_W, WINDOW_SIZE_H);
    w->label("Marble Solitaire");

    Board *board = new Board(w, (WINDOW_SIZE_W - BOARD_SIZE_W)/2, (WINDOW_SIZE_H - BOARD_SIZE_H)/2, BOARD_SIZE_W, BOARD_SIZE_H);

    w->end();
    w->show(argc, argv);

    MainLoop *mainloop  = creator->makeMainLoop();
    return mainloop->run();
}
