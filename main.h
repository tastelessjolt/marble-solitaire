#ifndef MAIN_H
#define MAIN_H

#include <vector>
#include "bridge.h"

using namespace std;

class Marble;
class Board;

class Marble : public Handler
{
    Board *parent;
    ImageView *imgView;
    Image *img;


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

class Board : public Handler
{
    Window *w;
    ImageView *imgView;
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
    
    // Selected marble
    Marble *last_marble;
    
    // Possible moves for the current marble
    vector<Marble*> highlighted;

  protected:
    void swap(int x1, int y1, int x2, int y2);
    int check_status();
    int num_neighbours (int x, int y);

  public:
    int size;
    int n_steps;
    Board(Window*, int, int, int, int);
    int move(int, int);
    void reset();
    ~Board();
};

#endif // MAIN_H
