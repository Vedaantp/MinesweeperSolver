// ======================================================================
// FILE:        MyAI.cpp
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

#include "MyAI.hpp"

MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    rows = _rowDimension;
    cols = _colDimension;
    oldXPos = _agentX;
    oldYPos = _agentY;
    regClear = true;

    board = (int**)malloc(_rowDimension * sizeof(int*));
    realBoard = (int**)malloc(_rowDimension * sizeof(int*));

    for(int i = 0; i < _rowDimension; i++){
        board[i] = (int*)malloc(_colDimension * sizeof(int));
        realBoard[i] = (int*)malloc(_colDimension * sizeof(int));
    }

    for(int i = 0; i < _rowDimension; i++){
        for(int j = 0; j < _colDimension; j++){
            matrix.insert({{i,j}, false});
            board[i][j] = -2;
            realBoard[i][j] = -2;
            regMatrix.insert({{i,j}, false});
            flagmap.insert({{i,j}, false});
            visited.insert({{i,j}, false});
        }
    }
};

Agent::Action MyAI::getAction( int number )
{
    updateBoard(number);

    if(number == 0){
        pushAdjacentToZero();
    }

    else if(number > 0){
        reg.push({oldXPos, oldYPos});
        checkFlags();
    }

    if(!zeros.empty()){

        oldXPos = zeros.front().first;
        oldYPos = zeros.front().second;
        zeros.pop();
        matrix[{oldXPos, oldYPos}] = true;
        visited[{oldXPos, oldYPos}] = true;

        return {UNCOVER, oldXPos, oldYPos};

    }
    
    if(!flags.empty()){
        oldXPos = flags.front().first;
        oldYPos = flags.front().second;
        flags.pop();

        if(flagmap[{oldXPos, oldYPos}] == false){
            matrix[{oldXPos, oldYPos}] = true;
            flagmap[{oldXPos, oldYPos}] = true;
            updateTiles();
            visited[{oldXPos, oldYPos}] = true;
            return {FLAG, oldXPos, oldYPos};
        }
    }

    for(int i = 0; i < reg.size(); i++){
        oldXPos = reg.front().first;
        oldYPos = reg.front().second;
        reg.pop();

        checkCovered();

        if(covered == board[rows - oldYPos - 1][oldXPos]){
            pushToFlags();
            covered = 0;
            i++;

        }

        else{
            reg.push({oldXPos, oldYPos});
            covered = 0;
        }

        covered = 0;
    }

    if(!flags.empty()){
        oldXPos = flags.front().first;
        oldYPos = flags.front().second;
        flags.pop();

        if(flagmap[{oldXPos, oldYPos}] == false){
            matrix[{oldXPos, oldYPos}] = true;
            flagmap[{oldXPos, oldYPos}] = true;
            updateTiles();
            visited[{oldXPos, oldYPos}] = true;
            return {FLAG, oldXPos, oldYPos};
        }
    }

    if(regClear){
        while(!reg.empty()){
            reg.pop();
        }
    }

    tempMin = 1000;
    min = 1000;
    minProb[0] = -1;
    minProb[1] = -1;
    tempH = -1;
    h = -1;
    flagProb[0] = -1;
    flagProb[1] = -1;
    safe = false;


    for(int i = 0; i < cols; i++){
        for(int j = 0; j < rows; j++){
            if(visited[{i,j}] == false){
                tempMin = findProb(i,j);
                tempH = checkHits(i, j);
                if(tempMin < min && safe){
                    minProb[0] = i;
                    minProb[1] = j;
                    min = tempMin;
                }

                else if(tempH > h){
                    h = tempH;
                    flagProb[0] = i;
                    flagProb[1] = j;
                }
            }
            else if(regClear){
                reg.push({i, j});
                regClear = false;
                
            }
        }
    }

    oldXPos = minProb[0];
    oldYPos = minProb[1];

    if(oldXPos > -1 && oldYPos > -1){
        visited[{oldXPos, oldYPos}] = true;
        matrix[{oldXPos, oldYPos}] = true;
        return{UNCOVER, oldXPos, oldYPos};
    }

    else{
        oldXPos = flagProb[0];
        oldYPos = flagProb[1];

        if(oldXPos > -1 && oldYPos > -1){
            matrix[{oldXPos, oldYPos}] = true;
            flagmap[{oldXPos, oldYPos}] = true;
            visited[{oldXPos, oldYPos}] = true;
            updateTiles();
            checkPattern();
            return {FLAG, oldXPos, oldYPos};
        }

    }

    return {LEAVE,-1,-1};

}

void MyAI::updateBoard(int number){

    board[rows - oldYPos - 1][oldXPos] = number;
    realBoard[rows - oldYPos - 1][oldXPos] = number;
}

void MyAI::pushAdjacentToZero(){

    if(oldXPos + 1 != cols){

        if(rows - oldYPos != rows){

            if(matrix[{oldXPos + 1, oldYPos - 1}] == false){
                zeros.push({oldXPos + 1, oldYPos - 1});
                matrix[{oldXPos + 1, oldYPos - 1}] = true;
            }
        }
        if(rows - oldYPos - 2 != -1){

            if(matrix[{oldXPos + 1, oldYPos + 1}] == false){
                zeros.push({oldXPos + 1, oldYPos + 1});
                matrix[{oldXPos + 1, oldYPos + 1}] = true;
            }
        }

        if(matrix[{oldXPos + 1, oldYPos}] == false){
            zeros.push({oldXPos + 1, oldYPos});
            matrix[{oldXPos + 1, oldYPos}] = true;
        }     
    
    }

    if(rows - oldYPos - 2 != -1){

        if(matrix[{oldXPos, oldYPos + 1}] == false){
            zeros.push({oldXPos, oldYPos + 1});
            matrix[{oldXPos, oldYPos + 1}] = true;
        }
    }
    
    //  checking bottom middle
    if(rows - oldYPos != rows){

        if(matrix[{oldXPos, oldYPos - 1}] == false){
            zeros.push({oldXPos, oldYPos - 1});
            matrix[{oldXPos, oldYPos - 1}] = true;
        }
    }    

    if(oldXPos - 1 != -1){

        if(rows - oldYPos - 2 != -1){

            if(matrix[{oldXPos - 1, oldYPos + 1}] == false){
                zeros.push({oldXPos - 1, oldYPos + 1});
                matrix[{oldXPos - 1, oldYPos + 1}] = true;
            }
        }

        if(matrix[{oldXPos - 1, oldYPos}] == false){
            zeros.push({oldXPos - 1, oldYPos});
            matrix[{oldXPos - 1, oldYPos}] = true;
        }

        if(rows - oldYPos != rows){

            if(matrix[{oldXPos - 1, oldYPos - 1}] == false){
                zeros.push({oldXPos - 1, oldYPos - 1});
                matrix[{oldXPos - 1, oldYPos - 1}] = true;
            }
        }

    }
}

void MyAI::checkCovered(){
    if(oldXPos + 1 != cols){

        if(rows - oldYPos != rows){

            if(board[rows - oldYPos][oldXPos + 1] == -2){
                covered++;
            }
        }

        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos + 1] == -2){
                covered++;
            }
        }

        if(board[rows - oldYPos - 1][oldXPos + 1] == -2){
            covered++;
        }     
    
    }

    if(rows - oldYPos - 2 != -1){

        if(board[rows - oldYPos - 2][oldXPos] == -2){
            covered++;
        }
    }
    
    if(rows - oldYPos != rows){

        if(board[rows - oldYPos][oldXPos] == -2){
            covered++;
        }
    }    

    if(oldXPos - 1 != -1){

        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos - 1] == -2){
                covered++;
            }
        }

        if(board[rows - oldYPos - 1][oldXPos - 1] == -2){
            covered++;
        }

        if(rows - oldYPos != rows){

            if(board[rows - oldYPos][oldXPos - 1] == -2){
                covered++;
            }
        }

    }
}

void MyAI::pushToFlags(){

    if(oldXPos + 1 != cols){

        if(rows - oldYPos != rows){

            if(board[rows - oldYPos][oldXPos + 1] < -1){
                flags.push({oldXPos + 1, oldYPos - 1});
            }
        }

        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos + 1] < -1){
                flags.push({oldXPos + 1, oldYPos + 1});
            }
        }

        if(board[rows - oldYPos - 1][oldXPos + 1] < -1){
            flags.push({oldXPos + 1, oldYPos});
        }     
    
    }

    if(rows - oldYPos - 2 != -1){

        if(board[rows - oldYPos - 2][oldXPos] < -1){
            flags.push({oldXPos, oldYPos + 1});
        }
    }
    
    if(rows - oldYPos != rows){

        if(board[rows - oldYPos][oldXPos] < -1){
            flags.push({oldXPos, oldYPos - 1});
        }
    }    

    if(oldXPos - 1 != -1){

        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos - 1] < -1){
                flags.push({oldXPos - 1, oldYPos + 1});
            }
        }

        if(board[rows - oldYPos - 1][oldXPos - 1] < -1){
            flags.push({oldXPos - 1, oldYPos});
        }

        if(rows - oldYPos != rows){

            if(board[rows - oldYPos][oldXPos - 1] < -1){
                flags.push({oldXPos - 1, oldYPos - 1});
            }
        }

    }

}

void MyAI::updateTiles(){

    if(oldXPos + 1 != cols){

        if(rows - oldYPos != rows){

            if(board[rows - oldYPos][oldXPos + 1] > 0){
                board[rows - oldYPos][oldXPos + 1]--;

                if(board[rows - oldYPos][oldXPos + 1] == 0){
                    pushToZero(oldXPos + 1, oldYPos - 1);
                }
            }
        }

        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos + 1] > 0){
                board[rows - oldYPos - 2][oldXPos + 1]--;

                if(board[rows - oldYPos - 2][oldXPos + 1] == 0){
                    pushToZero(oldXPos + 1, oldYPos + 1);
                }
            }
        }

        if(board[rows - oldYPos - 1][oldXPos + 1] > 0){
            board[rows - oldYPos - 1][oldXPos + 1]--;

            if(board[rows - oldYPos - 1][oldXPos + 1] == 0){
                pushToZero(oldXPos + 1, oldYPos);
            }
        }     
    
    }

    if(rows - oldYPos - 2 != -1){

        if(board[rows - oldYPos - 2][oldXPos] > 0){
            board[rows - oldYPos - 2][oldXPos]--;

            if(board[rows - oldYPos - 2][oldXPos] == 0){
                pushToZero(oldXPos, oldYPos + 1);
            }
        }
    }
    
    if(rows - oldYPos != rows){

        if(board[rows - oldYPos][oldXPos] > 0){
            board[rows - oldYPos][oldXPos]--;

            if(board[rows - oldYPos][oldXPos] == 0){
                pushToZero(oldXPos, oldYPos - 1);
            }
        }
    }    

    if(oldXPos - 1 != -1){

        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos - 1] > 0){
                board[rows - oldYPos - 2][oldXPos - 1]--;

                if(board[rows - oldYPos - 2][oldXPos - 1] == 0){
                    pushToZero(oldXPos - 1, oldYPos + 1);
                }
            }
        }

        if(board[rows - oldYPos - 1][oldXPos - 1] > 0){
            board[rows - oldYPos - 1][oldXPos - 1]--;

            if(board[rows - oldYPos - 1][oldXPos - 1] == 0){
                pushToZero(oldXPos - 1, oldYPos);
            }
        }

        if(rows - oldYPos != rows){

            if(board[rows - oldYPos][oldXPos - 1] > 0){
                board[rows - oldYPos][oldXPos - 1]--;

                if(board[rows - oldYPos][oldXPos - 1] == 0){
                    pushToZero(oldXPos - 1, oldYPos - 1);
                }
            }
        }

    }

}

void MyAI::pushToZero(int xPos, int yPos){

    if(xPos + 1 != cols){

        if(rows - yPos != rows){
            if(flagmap[{xPos + 1, yPos - 1}] == false && board[rows - yPos][xPos + 1] == -2){
                zeros.push({xPos + 1, yPos - 1});
                matrix[{xPos + 1, yPos - 1}] = true;
            }
        }

        if(rows - yPos - 2 != -1){
            if(flagmap[{xPos + 1, yPos + 1}] == false && board[rows - yPos - 2][xPos + 1] == -2){
                zeros.push({xPos + 1, yPos + 1});
                matrix[{xPos + 1, yPos + 1}] = true;
            }

        }

        if(flagmap[{xPos + 1, yPos}] == false && board[rows - yPos - 1][xPos + 1] == -2){
            zeros.push({xPos + 1, yPos});
            matrix[{xPos + 1, yPos}] = true;
        }
    
    }

    if(rows - yPos - 2 != -1){
        if(flagmap[{xPos, yPos + 1}] == false && board[rows - yPos - 2][xPos] == -2){
            zeros.push({xPos, yPos + 1});
            matrix[{xPos, yPos + 1}] = true;
        }
    }
    
    if(rows - yPos != rows){
        if(flagmap[{xPos, yPos - 1}] == false && board[rows - yPos][xPos] == -2){
            zeros.push({xPos, yPos - 1});
            matrix[{xPos, yPos - 1}] = true;
        }
    }    

    if(xPos - 1 != -1){

        if(rows - yPos - 2 != -1){
            if(flagmap[{xPos - 1, yPos + 1}] == false && board[rows - yPos - 2][xPos - 1] == -2){
                zeros.push({xPos - 1, yPos + 1});
                matrix[{xPos - 1, yPos + 1}] = true;
            }
        }

        if(flagmap[{xPos - 1, yPos}] == false && board[rows - yPos - 1][xPos - 1] == -2){
            zeros.push({xPos - 1, yPos});
            matrix[{xPos - 1, yPos}] = true;
        }

        if(rows - yPos != rows){
            if(flagmap[{xPos - 1, yPos - 1}] == false && board[rows - yPos][xPos - 1] == -2){
                zeros.push({xPos - 1, yPos - 1});
                matrix[{xPos - 1, yPos - 1}] = true;
            }
        }

    }

}

void MyAI::checkFlags(){

    if(oldXPos + 1 != cols){

        if(rows - oldYPos != rows){

            if(board[rows - oldYPos][oldXPos + 1] == -1){
                if(board[rows - oldYPos - 1][oldXPos] > 0){
                    board[rows - oldYPos - 1][oldXPos]--;
                }

                if(board[rows - oldYPos - 1][oldXPos] == 0){
                    pushToZero(oldXPos, oldYPos);
                }
            }
        }

        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos + 1] == -1){
                if(board[rows - oldYPos - 1][oldXPos] > 0){
                    board[rows - oldYPos - 1][oldXPos]--;
                }

                if(board[rows - oldYPos - 1][oldXPos] == 0){
                    pushToZero(oldXPos, oldYPos);
                }
            }
        }

        if(board[rows - oldYPos - 1][oldXPos + 1] == -1){
           if(board[rows - oldYPos - 1][oldXPos] > 0){
                board[rows - oldYPos - 1][oldXPos]--;
            }

            if(board[rows - oldYPos - 1][oldXPos] == 0){
                pushToZero(oldXPos, oldYPos);
            }
        }     
    
    }

    if(rows - oldYPos - 2 != -1){

        if(board[rows - oldYPos - 2][oldXPos] == -1){
            if(board[rows - oldYPos - 1][oldXPos] > 0){
                board[rows - oldYPos - 1][oldXPos]--;
            }

            if(board[rows - oldYPos - 1][oldXPos] == 0){
                pushToZero(oldXPos, oldYPos);
            }
        }
    }
    
    if(rows - oldYPos != rows){

        if(board[rows - oldYPos][oldXPos] == -1){
            if(board[rows - oldYPos - 1][oldXPos] > 0){
                board[rows - oldYPos - 1][oldXPos]--;
            }

            if(board[rows - oldYPos - 1][oldXPos] == 0){
                pushToZero(oldXPos, oldYPos);
            }
        }
    }    

    if(oldXPos - 1 != -1){

        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos - 1] == -1){
                if(board[rows - oldYPos - 1][oldXPos] > 0){
                    board[rows - oldYPos - 1][oldXPos]--;
                }

                if(board[rows - oldYPos - 1][oldXPos] == 0){
                    pushToZero(oldXPos, oldYPos);
                }
            }
        }

        if(board[rows - oldYPos - 1][oldXPos - 1] == -1){
            if(board[rows - oldYPos - 1][oldXPos] > 0){
                board[rows - oldYPos - 1][oldXPos]--;
            }

            if(board[rows - oldYPos - 1][oldXPos] == 0){
                pushToZero(oldXPos, oldYPos);
            }
        }

        if(rows - oldYPos != rows){

            if(board[rows - oldYPos][oldXPos - 1] == -1){
                if(board[rows - oldYPos - 1][oldXPos] > 0){
                    board[rows - oldYPos - 1][oldXPos]--;
                }

                if(board[rows - oldYPos - 1][oldXPos] == 0){
                    pushToZero(oldXPos, oldYPos);
                }
            }
        }

    }
}

float MyAI::findProb(int x, int y){

    float cov = 0.0;
    float uncov = 0.0;
    float mines = 0.0;

    if(x + 1 != cols){

        if(rows - y != rows){

            if(board[rows - y][x + 1] < -1){
                cov++;
            }

            else if(board[rows - y][x + 1] == -1){
                mines++;
            }                                                                                                                                                                                                                                                                                                      

            else{
                uncov++;
            }
        }

        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x + 1] < -1){
                cov++;
            }

            else if(board[rows - y - 2][x + 1] == -1){
                mines++;
            }

            else{
                uncov++;
            }
        }

        if(board[rows - y - 1][x + 1] < -1){
            cov++;
        }     

        else if(board[rows - y - 1][x + 1] == -1){
            mines++;
        }

        else{
            uncov++;
        }
    
    }

    if(rows - y - 2 != -1){

        if(board[rows - y - 2][x] < -1){
            cov++;
        }

        else if(board[rows - y - 2][x] == -1){
            mines++;
        }
        
        else{
            uncov++;
        }
    }
    
    if(rows - y != rows){

        if(board[rows - y][x] < -1){
            cov++;
        }

        else if(board[rows - y][x] == -1){
            mines++;
        }

        else{
            uncov++;
        }
    }    

    if(x - 1 != -1){

        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x - 1] < -1){
                cov++;
            }

            else if(board[rows - y - 2][x - 1] == -1){
                mines++;
            }

            else{
                uncov++;
            }
        }

        if(board[rows - y - 1][x - 1] < -1){
            cov++;
        }

        else if(board[rows - y - 1][x - 1] == -1){
            mines++;
        }
        
        else{
            uncov++;
        }

        if(rows - y != rows){

            if(board[rows - y][x - 1] < -1){
                cov++;
            }

            else if(board[rows - y][x - 1] == -1){
                mines++;
            }

            else{
                uncov++;
            }
        }

    }

    return ((uncov - mines) / cov);

}

int MyAI::checkHits(int x, int y){

    int uncov = 0;
    int hits = 0;

    if(x + 1 != cols){

        if(rows - y != rows){

            if(board[rows - y][x + 1] == 0){
                uncov++;
            }

            else if(board[rows - y][x + 1] > 0){
                uncov++;
                hits++;
            }                                                                                                                                                                                                                                                                                                      
        }

        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x + 1] == 0){
                uncov++;
            }

            else if(board[rows - y - 2][x + 1] > 0){
                uncov++;
                hits++;
            }

        }

        if(board[rows - y - 1][x + 1] == 0){
            uncov++;
        }     

        else if(board[rows - y - 1][x + 1] > 0){
            uncov++;
            hits++;
        }

    }

    if(rows - y - 2 != -1){

        if(board[rows - y - 2][x] == 0){
            uncov++;
        }

        else if(board[rows - y - 2][x] > 0){
            uncov++;
            hits++;
        }
        
    }
    
    if(rows - y != rows){

        if(board[rows - y][x] == 0){
            uncov++;
        }

        else if(board[rows - y][x] > 0){
            uncov++;
            hits++;
        }

    }    

    if(x - 1 != -1){

        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x - 1] == 0){
                uncov++;
            }

            else if(board[rows - y - 2][x - 1] > 0){
                uncov++;
                hits++;
            }

        }

        if(board[rows - y - 1][x - 1] == 0){
            uncov++;
        }

        else if(board[rows - y - 1][x - 1] > 0){
            uncov++;
            hits++;
        }

        if(rows - y != rows){

            if(board[rows - y][x - 1] == 0){
                uncov++;
            }

            else if(board[rows - y][x - 1] > 0){
                uncov++;
                hits++;
            }

        }

    }

    if(hits == uncov){
        safe = false;
        return hits;
    }
    
    safe = true;
    return 0;

}

void MyAI::checkPattern(){

    bool top = false;
    bool bottom = false;

    if(oldXPos + 1 != cols){
        if(board[rows - oldYPos - 1][oldXPos + 1] == 1){
            if(rows - oldYPos != rows){
                if(board[rows - oldYPos][oldXPos + 1] == 1){
                    if(rows - oldYPos - 2 != -1){
                        if(board[rows - oldYPos - 2][oldXPos + 1] == 1){
                            if(rows - oldYPos - 3 != -1){
                                top = true;
                            }
                            if(rows - oldYPos + 1 != rows){
                                bottom = true;
                            }
                        }     
                    }
                }
            }
        }
    }

    if(top || bottom){
        if(top && !bottom){
            oldYPos = oldYPos - 1;
            return;
        }
        else if(!top && bottom){
            oldYPos = oldYPos + 1;
            return;
        }
        else{
            return;
        }
    }

    if(oldXPos - 1 != -1){
        if(board[rows - oldYPos - 1][oldXPos - 1] == 1){
            if(rows - oldYPos != rows){
                if(board[rows - oldYPos][oldXPos - 1] == 1){
                    if(rows - oldYPos - 2 != -1){
                        if(board[rows - oldYPos - 2][oldXPos - 1] == 1){
                            if(rows - oldYPos - 3 != -1){
                                top = true;
                            }
                            if(rows - oldYPos + 1 != rows){
                                bottom = true;
                            }
                        }     
                    }
                }
            }
        }
    }

    if(top || bottom){
        if(top && !bottom){
            oldYPos = oldYPos - 1;
            return;
        }
        else if(!top && bottom){
            oldYPos = oldYPos + 1;
            return;
        }
        else{
            return;
        }
    }

    if(rows - oldYPos - 2 != -1){
        if(board[rows - oldYPos - 2][oldXPos] == 1){
            if(oldXPos + 1 != cols){
                if(board[rows - oldYPos - 2][oldXPos + 1] == 1){
                    if(oldXPos - 1 != -1){
                        if(board[rows - oldYPos - 2][oldXPos - 1] == 1){
                            if(oldXPos + 2 != cols){
                                top = true;
                            }
                            if(oldXPos - 2 != -1){
                                bottom = true;
                            }
                        }     
                    }
                }
            }
        }
    }

    if(top || bottom){
        if(top && !bottom){
            oldXPos = oldXPos - 1;
            return;
        }
        else if(!top && bottom){
            oldXPos = oldXPos + 1;
            return;
        }
        else{
            return;
        }
    }

    if(rows - oldYPos != rows){
        if(board[rows - oldYPos][oldXPos] == 1){
            if(oldXPos + 1 != cols){
                if(board[rows - oldYPos][oldXPos + 1] == 1){
                    if(oldXPos - 1 != -1){
                        if(board[rows - oldYPos][oldXPos - 1] == 1){
                            if(oldXPos + 2 != cols){
                                top = true;
                            }
                            if(oldXPos - 2 != -1){
                                bottom = true;
                            }
                        }     
                    }
                }
            }
        }
    }

    if(top || bottom){
        if(top && !bottom){
            oldXPos = oldXPos - 1;
            return;
        }
        else if(!top && bottom){
            oldXPos = oldXPos + 1;
            return;
        }
        else{
            return;
        }
    }

}

bool MyAI::checkCFrontier(int x, int y){
    if(x + 1 != cols){

        if(rows - y != rows){
            if(board[rows - y][x + 1] == -2){
                return true;
            }                                                                                                                                                                                                                                                                
        }

        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x + 1] == -2){
                return true;
            }


        }

        if(board[rows - y - 1][x + 1] == -2){
            return true;
        }     
    }

    if(rows - y - 2 != -1){

        if(board[rows - y - 2][x] == -2){
            return true;
        }
        
    }
    
    if(rows - y != rows){

        if(board[rows - y][x] == -2){
            return true;
        }

    }    

    if(x - 1 != -1){

        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x - 1] == -2){
                return true;
            }

        }

        if(board[rows - y - 1][x - 1] == -2){
            return true;
        }

        if(rows - y != rows){

            if(board[rows - y][x - 1] == -2){
                return true;
            }
        }

    }

    return false;
}

bool MyAI::checkVFrontier(int x, int y){
    
    
    if(x + 1 != cols){

        if(rows - y != rows){
            if(board[rows - y][x + 1] >= 0){
                return true;
            }                                                                                                                                                                                                                                                                
        }

        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x + 1] >= 0){
                return true;
            }


        }

        if(board[rows - y - 1][x + 1] >= 0){
            return true;
        }     
    }

    if(rows - y - 2 != -1){

        if(board[rows - y - 2][x] >= 0){
            return true;
        }
        
    }
    
    if(rows - y != rows){

        if(board[rows - y][x] >= 0){
            return true;
        }

    }    

    if(x - 1 != -1){

        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x - 1] >= 0){
                return true;
            }

        }

        if(board[rows - y - 1][x - 1] >= 0){
            return true;
        }

        if(rows - y != rows){

            if(board[rows - y][x - 1] >= 0){
                return true;
            }
        }

    }

    return false;
}
