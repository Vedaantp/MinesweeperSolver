// ======================================================================
// FILE:        MyAI.hpp
//
// AUTHOR:      Jian Li
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#ifndef MINE_SWEEPER_CPP_SHELL_MYAI_HPP
#define MINE_SWEEPER_CPP_SHELL_MYAI_HPP

#include "Agent.hpp"
#include <iostream> // temporary use
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <queue>
#include <stack>

using namespace std;

class MyAI : public Agent
{

private:

    int oldXPos = -1;
    int oldYPos = -1;
    int rows = 0;
    int cols = 0;
    int **board;
    int **realBoard;
    int covered = 0;

    queue<pair<int,int>> zeros;
    queue<pair<int,int>> flags;
    queue<pair<int,int>> reg;
    map<pair<int,int>, bool> matrix;  
    map<pair<int,int>, bool> regMatrix;
    map<pair<int,int>, bool> flagmap;
    map<pair<int,int>, bool> visited;
    set<pair<int,int>> V;
    set<pair<int,int>> C;
    float min;
    float tempMin;
    int minProb[2];
    int tempH;
    int h;
    int flagProb[2];
    bool safe;
    bool regClear;


public:
    MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY );

    Action getAction ( int number ) override;


    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================

    void updateBoard(int number);

    void pushAdjacentToZero();
    void checkCovered();
    void pushToFlags();
    void updateTiles();
    void pushToZero(int xPos, int yPos);
    void checkFlags();
    float findProb(int x, int y);
    int checkHits(int x, int y);
    void checkPattern();
    bool checkVFrontier(int x, int y);
    bool checkCFrontier(int x, int y);


    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
