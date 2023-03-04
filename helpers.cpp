#include <iostream>
#include <vector>

using namespace std;

// print 2d vector
void printBoard(vector<vector<int> > board)
{
    for (size_t i = 0; i < board.size(); i++)
    {
        for (size_t j = 0; j < board[i].size(); j++)
        {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
    cout<<endl;
}

// print array of pairs
void printPairs(vector<pair<int, int> > moves)
{
    for (size_t i = 0; i < moves.size(); i++)
    {
        cout << moves[i].first << " " << moves[i].second << endl;
    }
}

// print 1d vector
void printVector(vector<int> moves)
{
    for (size_t i = 0; i < moves.size(); i++)
    {
        cout << moves[i] << " ";
    }
    cout << endl;
}