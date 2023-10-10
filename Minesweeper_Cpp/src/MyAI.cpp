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
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================

    /*

        Board Info

        -2 = covered tile
        >= 0 = Uncovered tile
        -1 = flag
    
    */
    
    //  assigning old positions to class variable
    rows = _rowDimension;
    cols = _colDimension;
    oldXPos = _agentX;
    oldYPos = _agentY;
    regClear = true;

    //  mallocing the board
    board = (int**)malloc(_rowDimension * sizeof(int*));
    realBoard = (int**)malloc(_rowDimension * sizeof(int*));

    for(int i = 0; i < _rowDimension; i++){
        board[i] = (int*)malloc(_colDimension * sizeof(int));
        realBoard[i] = (int*)malloc(_colDimension * sizeof(int));
    }

    //init board
    // initialize the pairs in a map with val set to false for unvisited
    //  we can use this to check if we should uncover a tile or not later on
    //  True = visited
    //  False = unvisited
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

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

Agent::Action MyAI::getAction( int number )
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================

    //  update board
    updateBoard(number);

    
    //update the queue by adding uncovered tiles
    //pop the top and check if it is a zero
    //if it is a zero add all covered tiles into the zero queue
    if(number == 0){
        pushAdjacentToZero();
    }

    else if(number > 0){
        reg.push({oldXPos, oldYPos});
        checkFlags();
    }

    // if it was a zero uncover it
    if(!zeros.empty()){

        oldXPos = zeros.front().first;
        oldYPos = zeros.front().second;
        zeros.pop();
        matrix[{oldXPos, oldYPos}] = true;
        visited[{oldXPos, oldYPos}] = true;

        return {UNCOVER, oldXPos, oldYPos};

    }
    
    // if it needs to be flagged, flag it
    if(!flags.empty()){
        oldXPos = flags.front().first;
        oldYPos = flags.front().second;
        flags.pop();

        if(flagmap[{oldXPos, oldYPos}] == false){
            matrix[{oldXPos, oldYPos}] = true;
            flagmap[{oldXPos, oldYPos}] = true;
            //  decrements the surrounding tiles by 1
            //  if the tile turns into a 0 its adjacent covered tiles are added to 
            //  zero queue
            updateTiles();
            visited[{oldXPos, oldYPos}] = true;
            return {FLAG, oldXPos, oldYPos};
        }
    }

    //  for each numbered tile check if it has anything to flag
    for(int i = 0; i < reg.size(); i++){
        oldXPos = reg.front().first;
        oldYPos = reg.front().second;
        reg.pop();

        // gets the number of covered tiles adjacent to this tile
        checkCovered();

        //  if the number of covered tiles == the number on the tile
        if(covered == board[rows - oldYPos - 1][oldXPos]){
            //  push the covered adjacent tiles to the flag queue
            pushToFlags();
            covered = 0;
            i++;

        }

        //  otherwise push it back into the regular queue
        else{
            reg.push({oldXPos, oldYPos});
            covered = 0;
        }

        covered = 0;
    }

    // if it needs to be flagged, flag it
    if(!flags.empty()){
        oldXPos = flags.front().first;
        oldYPos = flags.front().second;
        flags.pop();

        if(flagmap[{oldXPos, oldYPos}] == false){
            matrix[{oldXPos, oldYPos}] = true;
            flagmap[{oldXPos, oldYPos}] = true;
            //  decrements the surrounding tiles by 1
            //  if the tile turns into a 0 its adjacent covered tiles are added to 
            //  zero queue
            updateTiles();
            visited[{oldXPos, oldYPos}] = true;
            return {FLAG, oldXPos, oldYPos};
        }
    }

    //  Guessing using probability find the tile with lowest probability of being a mine and uncover
    if(regClear){
        while(!reg.empty()){
            reg.pop();
        }
    }

    // bool tempC = false;
    // bool tempV = false;

    // for(int i = 0; i < cols; i++){
    //     for(int j = 0; j < rows; j++){
    //         if(visited[{i,j}] == false){
    //             tempV = checkVFrontier(i, j);
    //             if(tempV){
    //                 V.insert({i, j});
    //                 tempV = false;
    //             }
    //         }
    //         else{
    //             tempC = checkCFrontier(i, j);
    //             if(tempC){
    //                 C.insert({i,j});
    //                 tempC = false;
    //             }
    //         }
    //     }
    // }

    // cout << "V: " << V.size() << " C: " << C.size() << endl;

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

    // bool tempC = false;
    // bool tempV = false;

    // for(int i = 0; i < cols; i++){
    //     for(int j = 0; j < rows; j++){
    //         if(visited[{i,j}] == false){
    //             tempV = checkVFrontier(i, j);
    //             cout << "yes";
    //             if(tempV){
    //                 V.insert({i, j});
    //                 tempV = false;
    //                 cout << "YES";
    //             }
    //         }
    //         else{
    //             tempC = checkCFrontier(i, j);
    //             if(tempC){
    //                 C.insert({i,j});
    //                 tempC = false;
    //                 cout << "AND NO";
    //             }
    //         }
    //     }
    // }



    // cout << "V: " << V.size() << " C: " << C.size() << endl;

    return {LEAVE,-1,-1};

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================

}


// ======================================================================
// YOUR CODE BEGINS
// ======================================================================

//  updating the board by passing the position we uncovered and the number it revealed
//  displaying the board after updating
void MyAI::updateBoard(int number){

    board[rows - oldYPos - 1][oldXPos] = number;
    realBoard[rows - oldYPos - 1][oldXPos] = number;


    //  uncomment for visualization

    // for(int i = 0; i < rows; i++){
    //     for(int j = 0; j < cols; j++){
    //         cout << board[i][j] << ' ';
    //     }
        
    //     cout << endl;
    // }

    // cout << endl << endl;
}

void MyAI::pushAdjacentToZero(){

    //checking right side
    if(oldXPos + 1 != cols){

        //  checking bottom right
        if(rows - oldYPos != rows){

            if(matrix[{oldXPos + 1, oldYPos - 1}] == false){
                zeros.push({oldXPos + 1, oldYPos - 1});
                matrix[{oldXPos + 1, oldYPos - 1}] = true;
            }
        }

        //  checking top right
        if(rows - oldYPos - 2 != -1){

            if(matrix[{oldXPos + 1, oldYPos + 1}] == false){
                zeros.push({oldXPos + 1, oldYPos + 1});
                matrix[{oldXPos + 1, oldYPos + 1}] = true;
            }
        }

        //  checking middle right
        if(matrix[{oldXPos + 1, oldYPos}] == false){
            zeros.push({oldXPos + 1, oldYPos});
            matrix[{oldXPos + 1, oldYPos}] = true;
        }     
    
    }

    //  checking top middle
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

    //  checking left side
    if(oldXPos - 1 != -1){

        //  checking top left
        if(rows - oldYPos - 2 != -1){

            if(matrix[{oldXPos - 1, oldYPos + 1}] == false){
                zeros.push({oldXPos - 1, oldYPos + 1});
                matrix[{oldXPos - 1, oldYPos + 1}] = true;
            }
        }

        //  checking middle left
        if(matrix[{oldXPos - 1, oldYPos}] == false){
            zeros.push({oldXPos - 1, oldYPos});
            matrix[{oldXPos - 1, oldYPos}] = true;
        }

        //  checking bottom left
        if(rows - oldYPos != rows){

            if(matrix[{oldXPos - 1, oldYPos - 1}] == false){
                zeros.push({oldXPos - 1, oldYPos - 1});
                matrix[{oldXPos - 1, oldYPos - 1}] = true;
            }
        }

    }
}

void MyAI::checkCovered(){
    //checking right side
    if(oldXPos + 1 != cols){

        //  checking bottom right
        if(rows - oldYPos != rows){

            if(board[rows - oldYPos][oldXPos + 1] == -2){
                covered++;
            }
        }

        //  checking top right
        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos + 1] == -2){
                covered++;
            }
        }

        //  checking middle right
        if(board[rows - oldYPos - 1][oldXPos + 1] == -2){
            covered++;
        }     
    
    }

    //  checking top middle
    if(rows - oldYPos - 2 != -1){

        if(board[rows - oldYPos - 2][oldXPos] == -2){
            covered++;
        }
    }
    
    //  checking bottom middle
    if(rows - oldYPos != rows){

        if(board[rows - oldYPos][oldXPos] == -2){
            covered++;
        }
    }    

    //  checking left side
    if(oldXPos - 1 != -1){

        //  checking top left
        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos - 1] == -2){
                covered++;
            }
        }

        //  checking middle left
        if(board[rows - oldYPos - 1][oldXPos - 1] == -2){
            covered++;
        }

        //  checking bottom left
        if(rows - oldYPos != rows){

            if(board[rows - oldYPos][oldXPos - 1] == -2){
                covered++;
            }
        }

    }
}

void MyAI::pushToFlags(){

    //checking right side
    if(oldXPos + 1 != cols){

        //  checking bottom right
        if(rows - oldYPos != rows){

            if(board[rows - oldYPos][oldXPos + 1] < -1){
                flags.push({oldXPos + 1, oldYPos - 1});
            }
        }

        //  checking top right
        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos + 1] < -1){
                flags.push({oldXPos + 1, oldYPos + 1});
            }
        }

        //  checking middle right
        if(board[rows - oldYPos - 1][oldXPos + 1] < -1){
            flags.push({oldXPos + 1, oldYPos});
        }     
    
    }

    //  checking top middle
    if(rows - oldYPos - 2 != -1){

        if(board[rows - oldYPos - 2][oldXPos] < -1){
            flags.push({oldXPos, oldYPos + 1});
        }
    }
    
    //  checking bottom middle
    if(rows - oldYPos != rows){

        if(board[rows - oldYPos][oldXPos] < -1){
            flags.push({oldXPos, oldYPos - 1});
        }
    }    

    //  checking left side
    if(oldXPos - 1 != -1){

        //  checking top left
        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos - 1] < -1){
                flags.push({oldXPos - 1, oldYPos + 1});
            }
        }

        //  checking middle left
        if(board[rows - oldYPos - 1][oldXPos - 1] < -1){
            flags.push({oldXPos - 1, oldYPos});
        }

        //  checking bottom left
        if(rows - oldYPos != rows){

            if(board[rows - oldYPos][oldXPos - 1] < -1){
                flags.push({oldXPos - 1, oldYPos - 1});
            }
        }

    }

}

void MyAI::updateTiles(){

    //checking right side
    if(oldXPos + 1 != cols){

        //  checking bottom right
        if(rows - oldYPos != rows){

            if(board[rows - oldYPos][oldXPos + 1] > 0){
                board[rows - oldYPos][oldXPos + 1]--;

                if(board[rows - oldYPos][oldXPos + 1] == 0){
                    pushToZero(oldXPos + 1, oldYPos - 1);
                }
            }
        }

        //  checking top right
        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos + 1] > 0){
                board[rows - oldYPos - 2][oldXPos + 1]--;

                if(board[rows - oldYPos - 2][oldXPos + 1] == 0){
                    pushToZero(oldXPos + 1, oldYPos + 1);
                }
            }
        }

        //  checking middle right
        if(board[rows - oldYPos - 1][oldXPos + 1] > 0){
            board[rows - oldYPos - 1][oldXPos + 1]--;

            if(board[rows - oldYPos - 1][oldXPos + 1] == 0){
                pushToZero(oldXPos + 1, oldYPos);
            }
        }     
    
    }

    //  checking top middle
    if(rows - oldYPos - 2 != -1){

        if(board[rows - oldYPos - 2][oldXPos] > 0){
            board[rows - oldYPos - 2][oldXPos]--;

            if(board[rows - oldYPos - 2][oldXPos] == 0){
                pushToZero(oldXPos, oldYPos + 1);
            }
        }
    }
    
    //  checking bottom middle
    if(rows - oldYPos != rows){

        if(board[rows - oldYPos][oldXPos] > 0){
            board[rows - oldYPos][oldXPos]--;

            if(board[rows - oldYPos][oldXPos] == 0){
                pushToZero(oldXPos, oldYPos - 1);
            }
        }
    }    

    //  checking left side
    if(oldXPos - 1 != -1){

        //  checking top left
        if(rows - oldYPos - 2 != -1){

            if(board[rows - oldYPos - 2][oldXPos - 1] > 0){
                board[rows - oldYPos - 2][oldXPos - 1]--;

                if(board[rows - oldYPos - 2][oldXPos - 1] == 0){
                    pushToZero(oldXPos - 1, oldYPos + 1);
                }
            }
        }

        //  checking middle left
        if(board[rows - oldYPos - 1][oldXPos - 1] > 0){
            board[rows - oldYPos - 1][oldXPos - 1]--;

            if(board[rows - oldYPos - 1][oldXPos - 1] == 0){
                pushToZero(oldXPos - 1, oldYPos);
            }
        }

        //  checking bottom left
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

        //  checking bottom right
        if(rows - yPos != rows){
            if(flagmap[{xPos + 1, yPos - 1}] == false && board[rows - yPos][xPos + 1] == -2){
                zeros.push({xPos + 1, yPos - 1});
                matrix[{xPos + 1, yPos - 1}] = true;
            }
        }

        //  checking top right
        if(rows - yPos - 2 != -1){
            if(flagmap[{xPos + 1, yPos + 1}] == false && board[rows - yPos - 2][xPos + 1] == -2){
                zeros.push({xPos + 1, yPos + 1});
                matrix[{xPos + 1, yPos + 1}] = true;
            }

        }

        //  checking middle right
        if(flagmap[{xPos + 1, yPos}] == false && board[rows - yPos - 1][xPos + 1] == -2){
            zeros.push({xPos + 1, yPos});
            matrix[{xPos + 1, yPos}] = true;
        }
    
    }

    //  checking top middle
    if(rows - yPos - 2 != -1){
        if(flagmap[{xPos, yPos + 1}] == false && board[rows - yPos - 2][xPos] == -2){
            zeros.push({xPos, yPos + 1});
            matrix[{xPos, yPos + 1}] = true;
        }
    }
    
    //  checking bottom middle
    if(rows - yPos != rows){
        if(flagmap[{xPos, yPos - 1}] == false && board[rows - yPos][xPos] == -2){
            zeros.push({xPos, yPos - 1});
            matrix[{xPos, yPos - 1}] = true;
        }
    }    

    //  checking left side
    if(xPos - 1 != -1){

        //  checking top left
        if(rows - yPos - 2 != -1){
            if(flagmap[{xPos - 1, yPos + 1}] == false && board[rows - yPos - 2][xPos - 1] == -2){
                zeros.push({xPos - 1, yPos + 1});
                matrix[{xPos - 1, yPos + 1}] = true;
            }
        }

        //  checking middle left
        if(flagmap[{xPos - 1, yPos}] == false && board[rows - yPos - 1][xPos - 1] == -2){
            zeros.push({xPos - 1, yPos});
            matrix[{xPos - 1, yPos}] = true;
        }

        //  checking bottom left
        if(rows - yPos != rows){
            if(flagmap[{xPos - 1, yPos - 1}] == false && board[rows - yPos][xPos - 1] == -2){
                zeros.push({xPos - 1, yPos - 1});
                matrix[{xPos - 1, yPos - 1}] = true;
            }
        }

    }

}

void MyAI::checkFlags(){

    //checking right side
    if(oldXPos + 1 != cols){

        //  checking bottom right
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

        //  checking top right
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

        //  checking middle right
        if(board[rows - oldYPos - 1][oldXPos + 1] == -1){
           if(board[rows - oldYPos - 1][oldXPos] > 0){
                board[rows - oldYPos - 1][oldXPos]--;
            }

            if(board[rows - oldYPos - 1][oldXPos] == 0){
                pushToZero(oldXPos, oldYPos);
            }
        }     
    
    }

    //  checking top middle
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
    
    //  checking bottom middle
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

    //  checking left side
    if(oldXPos - 1 != -1){

        //  checking top left
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

        //  checking middle left
        if(board[rows - oldYPos - 1][oldXPos - 1] == -1){
            if(board[rows - oldYPos - 1][oldXPos] > 0){
                board[rows - oldYPos - 1][oldXPos]--;
            }

            if(board[rows - oldYPos - 1][oldXPos] == 0){
                pushToZero(oldXPos, oldYPos);
            }
        }

        //  checking bottom left
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

    //checking right side
    if(x + 1 != cols){

        //  checking bottom right
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

        //  checking top right
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

        //  checking middle right
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

    //  checking top middle
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
    
    //  checking bottom middle
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

    //  checking left side
    if(x - 1 != -1){

        //  checking top left
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

        //  checking middle left
        if(board[rows - y - 1][x - 1] < -1){
            cov++;
        }

        else if(board[rows - y - 1][x - 1] == -1){
            mines++;
        }
        
        else{
            uncov++;
        }

        //  checking bottom left
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

    //checking right side
    if(x + 1 != cols){

        //  checking bottom right
        if(rows - y != rows){

            if(board[rows - y][x + 1] == 0){
                uncov++;
            }

            else if(board[rows - y][x + 1] > 0){
                uncov++;
                hits++;
            }                                                                                                                                                                                                                                                                                                      
        }

        //  checking top right
        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x + 1] == 0){
                uncov++;
            }

            else if(board[rows - y - 2][x + 1] > 0){
                uncov++;
                hits++;
            }

        }

        //  checking middle right
        if(board[rows - y - 1][x + 1] == 0){
            uncov++;
        }     

        else if(board[rows - y - 1][x + 1] > 0){
            uncov++;
            hits++;
        }

    }

    //  checking top middle
    if(rows - y - 2 != -1){

        if(board[rows - y - 2][x] == 0){
            uncov++;
        }

        else if(board[rows - y - 2][x] > 0){
            uncov++;
            hits++;
        }
        
    }
    
    //  checking bottom middle
    if(rows - y != rows){

        if(board[rows - y][x] == 0){
            uncov++;
        }

        else if(board[rows - y][x] > 0){
            uncov++;
            hits++;
        }

    }    

    //  checking left side
    if(x - 1 != -1){

        //  checking top left
        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x - 1] == 0){
                uncov++;
            }

            else if(board[rows - y - 2][x - 1] > 0){
                uncov++;
                hits++;
            }

        }

        //  checking middle left
        if(board[rows - y - 1][x - 1] == 0){
            uncov++;
        }

        else if(board[rows - y - 1][x - 1] > 0){
            uncov++;
            hits++;
        }

        //  checking bottom left
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

    // if it has 3 1's adjacent to it in a row

    bool top = false;
    bool bottom = false;

    // 1's on right side
    if(oldXPos + 1 != cols){
        if(board[rows - oldYPos - 1][oldXPos + 1] == 1){
            if(rows - oldYPos != rows){
                if(board[rows - oldYPos][oldXPos + 1] == 1){
                    if(rows - oldYPos - 2 != -1){
                        if(board[rows - oldYPos - 2][oldXPos + 1] == 1){
                            // check next one on top right
                            if(rows - oldYPos - 3 != -1){
                                top = true;
                            }
                            // check next one on bottom right
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

    // 1's on left side
    if(oldXPos - 1 != -1){
        if(board[rows - oldYPos - 1][oldXPos - 1] == 1){
            if(rows - oldYPos != rows){
                if(board[rows - oldYPos][oldXPos - 1] == 1){
                    if(rows - oldYPos - 2 != -1){
                        if(board[rows - oldYPos - 2][oldXPos - 1] == 1){
                            // check next one on top right
                            if(rows - oldYPos - 3 != -1){
                                top = true;
                            }
                            // check next one on bottom right
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

    // 1's on top side
    if(rows - oldYPos - 2 != -1){
        if(board[rows - oldYPos - 2][oldXPos] == 1){
            if(oldXPos + 1 != cols){
                if(board[rows - oldYPos - 2][oldXPos + 1] == 1){
                    if(oldXPos - 1 != -1){
                        if(board[rows - oldYPos - 2][oldXPos - 1] == 1){
                            // check next one on top right
                            if(oldXPos + 2 != cols){
                                top = true;
                            }
                            // check next one on bottom right
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

    // 1's on bottom side
    if(rows - oldYPos != rows){
        if(board[rows - oldYPos][oldXPos] == 1){
            if(oldXPos + 1 != cols){
                if(board[rows - oldYPos][oldXPos + 1] == 1){
                    if(oldXPos - 1 != -1){
                        if(board[rows - oldYPos][oldXPos - 1] == 1){
                            // check next one on top right
                            if(oldXPos + 2 != cols){
                                top = true;
                            }
                            // check next one on bottom right
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

        //  checking bottom right
        if(rows - y != rows){
            if(board[rows - y][x + 1] == -2){
                return true;
            }                                                                                                                                                                                                                                                                
        }

        //  checking top right
        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x + 1] == -2){
                return true;
            }


        }

        //  checking middle right
        if(board[rows - y - 1][x + 1] == -2){
            return true;
        }     
    }

    //  checking top middle
    if(rows - y - 2 != -1){

        if(board[rows - y - 2][x] == -2){
            return true;
        }
        
    }
    
    //  checking bottom middle
    if(rows - y != rows){

        if(board[rows - y][x] == -2){
            return true;
        }

    }    

    //  checking left side
    if(x - 1 != -1){

        //  checking top left
        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x - 1] == -2){
                return true;
            }

        }

        //  checking middle left
        if(board[rows - y - 1][x - 1] == -2){
            return true;
        }

        //  checking bottom left
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

        //  checking bottom right
        if(rows - y != rows){
            if(board[rows - y][x + 1] >= 0){
                return true;
            }                                                                                                                                                                                                                                                                
        }

        //  checking top right
        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x + 1] >= 0){
                return true;
            }


        }

        //  checking middle right
        if(board[rows - y - 1][x + 1] >= 0){
            return true;
        }     
    }

    //  checking top middle
    if(rows - y - 2 != -1){

        if(board[rows - y - 2][x] >= 0){
            return true;
        }
        
    }
    
    //  checking bottom middle
    if(rows - y != rows){

        if(board[rows - y][x] >= 0){
            return true;
        }

    }    

    //  checking left side
    if(x - 1 != -1){

        //  checking top left
        if(rows - y - 2 != -1){

            if(board[rows - y - 2][x - 1] >= 0){
                return true;
            }

        }

        //  checking middle left
        if(board[rows - y - 1][x - 1] >= 0){
            return true;
        }

        //  checking bottom left
        if(rows - y != rows){

            if(board[rows - y][x - 1] >= 0){
                return true;
            }
        }

    }

    return false;
}
// ======================================================================
// YOUR CODE ENDS
// ======================================================================
