/*
 * MIT License
 *
 * Copyright (c) 2016 Georg A. Friedrich
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef NCURSE_TEST_TICTACTOE_H
#define NCURSE_TEST_TICTACTOE_H

#include <ncurses.h>

class SearchTree;

class TicTacToe {
private:
    struct GUIboard;
    struct GAMEinfo;

    static void quit();
    static void draw();
    static void initBoard(int, int);
    static void drawBoard();
    static void resize(int);
    static void drawGame();
    static void drawFrame();


    static void drawCircle(WINDOW *, int, int, int, int);
    static void drawCross(WINDOW *, double, double, int, int);

    static void (*old_callback)(int);
    static WINDOW *game_field;
    static WINDOW *debug_field;
    static bool _debug;
    static GUIboard currentXY;
    static char current_board[9];
    static bool NOCOLOR;
    static GAMEinfo gameInfo;
    static SearchTree* currentNode;

    static char game_over;


public:
    TicTacToe();
    TicTacToe(bool);

    static char checkWon(char*, size_t);

};


#endif //NCURSE_TEST_TICTACTOE_H
