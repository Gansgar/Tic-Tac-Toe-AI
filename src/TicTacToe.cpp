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

#include <ncurses.h>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "TicTacToe.h"
#include "SearchTree.h"

#define DEGTORAD(deg) (deg*(M_PI/180.0))
#define lengthChar(arr) ( sizeof(arr)/sizeof(char) )
#define SIGN(d) ( d/std::abs(d) )
#define PRINT(in) ((in)?"T":"F")


#define CROSS 1
#define CIRCLE -1
#define EMPTY 0
#define TIE -2


struct TicTacToe::GUIboard {
    int x_seq, y_seq;
    int w,h;
    int x,y;
};

struct TicTacToe::GAMEinfo {
    int selected;
    char turn;
    char player;
};

bool TicTacToe::_debug;
WINDOW *TicTacToe::game_field;
WINDOW *TicTacToe::debug_field;
void (*TicTacToe::old_callback)(int);
TicTacToe::GUIboard TicTacToe::currentXY;
TicTacToe::GAMEinfo TicTacToe::gameInfo;
char TicTacToe::current_board[9] = {0,0,0,0,0,0,0,0,0};
bool TicTacToe::NOCOLOR = false;
char TicTacToe::game_over = 0;

SearchTree* TicTacToe::currentNode;


// METHODS

void TicTacToe::quit() {
    delwin(game_field);
    endwin();
}

void TicTacToe::resize(int a) {
    old_callback(a);
    draw();
}

void TicTacToe::draw() {
    clear();

    int x, y;
    getmaxyx(stdscr, y, x);
    if (x < 18 || y < 10) {
        mvaddstr(0, 0, "Your terminal window is to small!");
        refresh();
    } else {
        initBoard(y, x);

        if( _debug ) {
            mvprintw(0,0, "[%d,%d]", y,x);
            getmaxyx(game_field, y, x);
            mvprintw(1,0, "[%d,%d]", y,x);
        }

        //drawGame();

        refresh();
        drawFrame();
    }
}

void TicTacToe::initBoard(int my, int mx) {
    int w = std::max((int)(mx/1.5), 18);
    int h = std::max((int)(my/1.5), 9);
    if( w/2<h ) {
        h=w/2;
    } else {
        w=h*2;
    }
    w-=(w%6) - 1;
    h-=h%3 - 1;


    int x_seg = (w/3);
    int y_seg = (h/3);

    game_field = newwin(h ,w, (my-h)/2, (mx-w)/2 );
    debug_field = newwin(my, 30, 0,0);

    GUIboard tmp = {.y_seq=y_seg, .x_seq=x_seg, .y = h-1, .x = (w-1)/2+5, .w=w, .h=h};
    currentXY = tmp;
}

void TicTacToe::drawBoard() {
    box(game_field, 0,0);
    for (int j = 1; j < 3; ++j) {
        mvwhline(game_field, currentXY.y_seq*j,0, 0, currentXY.w);
        mvwvline(game_field, 0,currentXY.x_seq*j, 0, currentXY.h);
    }
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            int y = currentXY.y_seq*i;
            int x = currentXY.x_seq*j;
            chtype ch = ACS_PLUS;

            if(j==0)
                ch = ACS_LTEE;
            else if(j==3)
                ch = ACS_RTEE;
            else if(i==0)
                ch = ACS_TTEE;
            else if(i==3)
                ch = ACS_BTEE;

            if( (i>0 || j>0) && (i<3 || j<3) && (i>0 || j<3) && (i<3 || j>0) )
                mvwaddch(game_field, y, x, ch);
        }
    }

    if(game_over == 0)
        mvwaddstr(game_field, currentXY.h-1,(currentXY.w-1)/2-6,  "Turn for: '-'");
    else {
        mvwaddstr(game_field, currentXY.h-1, (currentXY.w-1)/2-6, "AGAIN?  [Y/n]");
        if(!NOCOLOR)
            mvwchgat(game_field,currentXY.h-1, (currentXY.w-1)/2-6,  13, A_BOLD, 2, NULL);
    }
}

void TicTacToe::drawGame() {
    int x_seq = currentXY.x_seq;
    int y_seq = currentXY.y_seq;

    if(_debug) {
        std::stringstream ss;
        ss << currentNode->printBoard(true, current_board, 3);
        ss << currentNode->printBoard(true) << currentNode->printChilds(true) << "\n";

        int k = 0;
        for(std::string line; std::getline(ss, line);) {
            mvwprintw(debug_field, 7+k, 0, "%s", line.c_str() );
            k++;
        }
    }

    int color = 0;
    for (int i = 0; i < lengthChar(current_board); ++i) {
        int cor = (x_seq > 14) ? 5 : 3;
        if (current_board[i] == CROSS) {
            color = COLOR_PAIR(3) | A_BOLD;

            if(!NOCOLOR)
                wattron(game_field, color );
            drawCross(game_field, y_seq / 2.0 + y_seq * (i / 3), x_seq / 2.0 + x_seq * (i % 3), y_seq - cor,
                      x_seq - cor * 2);

        } else if (current_board[i] == CIRCLE) {
            color = COLOR_PAIR(4) | A_BOLD;
            if(!NOCOLOR)
                wattron(game_field, color );
            drawCircle(game_field, y_seq / 2 + y_seq * (i / 3), x_seq / 2 + x_seq * (i % 3), y_seq - cor, x_seq - cor * 2);
        }

        if(!NOCOLOR)
            wattroff(game_field, color);
    }
}

void TicTacToe::drawFrame() {
    int c = gameInfo.selected;
    char t = gameInfo.turn;

    if(_debug)
        wclear(debug_field);
    wclear(game_field);

    drawBoard();
    drawGame();

    int y_seq = currentXY.y_seq;
    int x_seq = currentXY.x_seq;

    int color = -1;

    if(game_over == 0)
        mvwaddch(game_field, currentXY.y, currentXY.x, ((t==CROSS)?'X':'O'));


    if(!NOCOLOR) {
        if( current_board[c] == EMPTY && game_over==0 )
            color = COLOR_PAIR(2) | A_BOLD ;
        else
            color = COLOR_PAIR(1) | A_BOLD;
        wattron(game_field, color );
    }

    if(game_over == 0) {
        int cor = (x_seq > 14) ? 5 : 3;
        if (t == CROSS)
            drawCross(game_field, y_seq / 2.0 + y_seq * (c / 3), x_seq / 2.0 + x_seq * (c % 3),
                      y_seq - cor, x_seq - cor * 2);
        else if (t == CIRCLE)
            drawCircle(game_field, y_seq / 2 + y_seq * (c / 3), x_seq / 2 + x_seq * (c % 3),
                       y_seq - cor, x_seq - cor * 2);
    } else {
        if( game_over!=TIE ) {
            mvwprintw(game_field, 0,0, "Player '%s' won!", (char*)((game_over==CROSS)?"X":"O"));
        } else {
            mvwprintw(game_field, 0,0, "It's a tie!!");
        }
    }

    if(!NOCOLOR)
        wattroff(game_field, color );


    if(_debug)
        wrefresh(debug_field);
    wrefresh(game_field);
}


/// GAME METHODS

char TicTacToe::checkWon(char* field, size_t a) {
    char *sumV = (char*)calloc(a, sizeof(char));
    char *sumH = (char*)calloc(a, sizeof(char));

    for(int i=0;i<a*a;i++) {
        *(sumV + i%a) += *(field+i);
        *(sumH + i/a) += *(field+i);
    }

    for (int j = 0; j<a;++j) {
        if( std::abs(*(sumV+j)) == 3 )
            return (char)SIGN(*(sumV+j));
        if( std::abs(*(sumH+j)) == 3 )
            return (char)SIGN(*(sumH+j));
    }

    char sumD[2] = {0,0};
    for (int j = 0; j < a; ++j) {
        sumD[0] += *(field+(j*(1+a)));
        sumD[1] += *(field+(j+(a-j-1)*a));
    }



    free(sumV);
    free(sumH);

    if( sumD[0]/(long)a !=0)
        return (char)(sumD[0]/(long)a);

    return  (char)(sumD[1]/(long)a);
}

/// GRAPHICS FUNCTIONS

void TicTacToe::drawCircle(WINDOW *win, int cy, int cx, int h, int w) {
    double ry = std::max(h/2.0,0.0);
    double rx = std::max(w/2.0,0.0);

    if( (int)ry==0 || (int)rx==0 )
        mvwaddch(win, cy, cx, 'o');
    else {
        for(int d=0; d<360; ++d) {
            int y = cy + (int)(ry* sin(DEGTORAD(d) ));
            int x = cx + (int)(rx* cos(DEGTORAD(d) ));
            mvwaddch(win, y, x, 'o');
        }
    }
}

void TicTacToe::drawCross(WINDOW *win, double cy, double cx, int h, int w) {
    int max_step = std::max(h,w);
    double ry = std::max(h/2.0,0.0);
    double rx = std::max(w/2.0,0.0);

    if( (int)ry==0 || (int)rx==0 )
        mvwaddch(win, (int)cy, (int)cx, 'x');
    else {
        int y,x;

        for( int p=0; p<
                max_step; ++p) {
            if(h>w){
                y=p; x=(int)((double)w/h*y);
            } else {
                x=p; y=(int)((double)h/w*x);
            }

            mvwaddch(win, (int)round((cy-ry)+y), (int)round((cx-rx)+x), '\\');
            mvwaddch(win, (int)((cy-ry)+(h-y)), (int)((cx-rx)+x), '/');
        }
    }
}

/// NON STATIC ////

TicTacToe::TicTacToe() {
    initscr();
    atexit(quit);
    curs_set(0);
    if(has_colors() == FALSE)
        NOCOLOR = true;
    else {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_BLUE, COLOR_BLACK);
        init_pair(4, COLOR_GREEN, COLOR_BLACK);
    }

    keypad(stdscr, TRUE);

    std::cout << "Init binary search tree... ";
    std::flush(std::cout);
    SearchTree* firstNode = new SearchTree(current_board, 3, 0, 9);
    std::cout << "Done" << std::endl;

    gameInfo = {.selected=0, .player=CROSS};
    rerun:
        gameInfo.selected=0;
        gameInfo.turn = gameInfo.player;
    game_over = 0;
    currentNode = firstNode;

    for(int i=0; i<lengthChar(current_board);i++)
        current_board[i] = 0;


    old_callback = signal(SIGWINCH, resize);
    draw();


    int ch;
    bool everyM = false;

    while( (ch = getch()) != 'q') {
        switch (ch) {
            case KEY_UP:
                gameInfo.selected -= 3;
                break;
            case KEY_DOWN:
                gameInfo.selected += 3;
                break;
            case KEY_LEFT:
                --gameInfo.selected;
                break;
            case KEY_RIGHT:
                ++gameInfo.selected;
                break;
            case 10:
                if(current_board[gameInfo.selected] == EMPTY) {
                    if(gameInfo.turn == gameInfo.player || everyM)
                        current_board[gameInfo.selected] = gameInfo.turn;


                    if(game_over==0) {
                        if (gameInfo.turn == gameInfo.player || everyM) {
                            currentNode = &(currentNode->findMove(current_board, 9));

                        }


                        if(currentNode->getNChilds() != 0 && !everyM) {
                            currentNode = &(currentNode->getMove(  (char)((gameInfo.player==CROSS)?CIRCLE:CROSS) ));
                            currentNode->getBoard(current_board, 9);
                        }
                    }

                    if (everyM)
                        gameInfo.turn = (char)((gameInfo.turn==CROSS)?CIRCLE:CROSS);



                    game_over = checkWon((char*)&(current_board), 3);

                    if(currentNode->getNChilds() == 0 && game_over==0)
                        game_over = TIE;
                } else
                    beep();


                break;
            case 'p':
                gameInfo.player = (char)((gameInfo.player==CROSS)?CIRCLE:CROSS);
                if(!everyM)
                    gameInfo.turn = gameInfo.player;
                break;
            case 's':
                everyM = !everyM;
                break;
            case 'e':
                if(_debug) {
                    quit();
                    std::cout << "WON? " << PRINT(checkWon((char*)&(current_board), 3)) << std::endl;
                    std::cout << currentNode->printBoard(true) << std::endl;
                    std::cout << currentNode->printChilds(true) << std::endl;
                    exit(0);
                }
            case 'd':
                _debug = !_debug;
                wclear(debug_field);
                wrefresh(debug_field);
                break;
            default:
                continue;
        }

        if(gameInfo.selected<0)
            gameInfo.selected = 9+gameInfo.selected;
        gameInfo.selected%=9;
        drawFrame();

        if(game_over != 0)
            break;
    }
    ch = getch();
    quit();
    if( ch != 'n' && ch != 'N')
        goto rerun;

    free(firstNode);
}

TicTacToe::TicTacToe(bool d) {
    _debug = d;
    TicTacToe();
}