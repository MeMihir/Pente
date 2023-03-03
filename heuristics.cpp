#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

// returns random number between 0 and 500
int randomHeuristic()
{
    return rand() % 500;
}


int centralHeuristic(vector<vector<int> > board, int tile)
{
    int heuristic = 0;
    for(int i=0; i<5; i++)
    {
        for(int j=0; j<5; j++)
        {
            if(board[i][j] == tile)
            {
                heuristic += (3 - i)*(3 - j);
            }
        }
    }
    return heuristic;
}