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

#ifndef NCURSE_TEST_SEARCHTREE_H
#define NCURSE_TEST_SEARCHTREE_H


#include <vector>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include "TicTacToe.h"

#define PRINT(in) ((in)?"T":"F")

#define EMPTY 0
#define CROSS 1
#define CIRCLE -1


class SearchTree {
private:
    size_t w;
    int value;
    char *board;
    bool directWin;
    bool dangerous;

    std::vector<SearchTree> childs;
    int n_child;



public:

    SearchTree(char *mboard , size_t mw, int depth, size_t arrlen) {
        board = mboard;
        w = mw;
        n_child = 0;
        value = TicTacToe::checkWon(board, w);
        directWin = value!=0;
        dangerous = false;

        if(value == 0) {
            for (int i = 0; i < arrlen; ++i) {
                if( *(board+i) == EMPTY ) {
                    char *n_board = (char*)malloc(w*w);
                    memcpy(n_board, board, sizeof(char)*w*w);

                    *(n_board+i) = (char)(((depth%2)*2-1)*-1);

                    SearchTree cChild = SearchTree(n_board, mw, depth+1, arrlen);
                    dangerous |= cChild.directWin && cChild.value==CROSS;

                    childs.push_back( cChild );
                    n_child++;

                    value += cChild.value;
                    n_child+= cChild.n_child;
                }
            }
        }
    }

    SearchTree& findMove(char *lboard, size_t arrlen) {
        bool found;

        //printBoard(true, lboard, w);
        //printChilds(true);


        for (unsigned int i = 0; i < childs.size(); ++i) {
            found = true;
            for (int j = 0; j < arrlen; ++j) {
                if( *(lboard+j) !=  *( childs.at(i).board +j)) {
                    found = false;
                    break;
                }
            }
            if (found)
                return childs.at(i);
        }
        throw std::runtime_error("Error while finding board in 'findMove'");
    }

    SearchTree& getMove(char player) {
        char maxP = -1;
        int maxV = -1000000 * player;
        bool isDang = false;

        rerun:
        for ( unsigned char i=0; i<childs.size(); ++i) {
            if(!isDang && childs.at(i).dangerous) {
                isDang |= childs.at(i).dangerous;
                goto rerun;
            }

            if( childs.at(i).value < maxV ) {
                maxP = i;
                maxV = childs.at(i).getValue();
            }
            if(childs.at(i).directWin || (!childs.at(i).dangerous && isDang) ) {
                maxP = i;
                break;
            }
        }

        if(maxP < 0)
            throw std::runtime_error("Error while finding new move. maxP==-1 in 'getMove'");
        return childs.at((unsigned char)maxP);
    }

    int getValue() {return value;}
    int getNChilds() {return n_child;}

    void getBoard(char* in, int arrlen) {
        for (int i = 0; i < arrlen; ++i) {
            *(in+i) = *(board+i);
        }
    }

    static std::string printBoard(bool symbol, char* mboard, size_t w) {
        std::stringstream ss;

        for(int i=0; i<w*w; i++) {
            if(i%w==0 && i!=0)
                ss << "\n";

            if(symbol) {
                char sym = '-';
                if(*(mboard+i)==CROSS)
                    sym = 'x';
                else if( *(mboard+i) == CIRCLE )
                    sym = 'o';
                ss << sym << " ";
            } else
                ss << (int)(*(mboard+i)) << " ";
        }
        ss << "\n\n";
        return ss.str();
    }

    std::string printBoard(bool symbol) {
        return printBoard(symbol, board, w);
    }


    std::string printChilds(bool sym) {
        std::stringstream ss;
        for(unsigned int i=0;i<childs.size(); ++i) {
            ss << "Child: " << i << "| v:" << childs.at(i).getValue() << " | " << PRINT(childs.at(i).directWin) << " | " << PRINT(childs.at(i).dangerous) << "\n";
            ss << childs.at(i).printBoard(sym);
        }
        return ss.str();
    }
};


#endif //NCURSE_TEST_SEARCHTREE_H
