#include <iostream>
#include <vector>
#include <string>

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


string indices_to_position(int row, int column) {
    if(column >= 8) column++;
    char letter = static_cast<char>(column + 'A');
    string number = to_string(19 - row);
    return string(1, letter) + number;
}
