#include <iostream>
#include <vector>
#include <cstdlib>
#include <utility>

using namespace std;
#include "heuristics.h"
#include "helpers.h"

#define pii pair<int, int>

class agent
{
private:
    // Inputs
    vector<vector<int> > board; // 1 = black, 2 = white, 0 = empty
    int whiteCaptures;
    int blackCaptures;
    double time;
    int agentTile; // 1 = black, 2 = white

    // Hyperparameters
    int maxDepth;

public:
    agent(string path);
    ~agent();
    void printData();
    void playGame();
    int alphaBeta(vector<vector<int> > currBoard, int wCaps, int bCaps, int depth, int alpha, int beta, bool isMaximizing);
    int evaluateBoard(vector<vector<int> > currBoard, bool isMaximizing);
    vector<pii> getChildren(vector<vector<int> > currBoard);
    pair<vector<vector<int> >, int> checkCaptures(vector<vector<int> > currBoard, int row, int col, int color);
    bool checkWin(vector<vector<int> > currBoard, int wCaps, int bCaps, int color, int row, int col);
};

// CONSTRUCTOR
agent::agent(string path)
{
    freopen(path.c_str(), "r", stdin);
    string agentColor;
    string captures;

    cin >> agentColor;
    agentTile = agentColor == "BLACK" ? 1 : 2;
    cin >> time;
    cin >> captures;
    whiteCaptures = captures[0] - '0';
    blackCaptures = captures[2] - '0';
    board = vector<vector<int> >(19, vector<int>(19, 0));
    for (size_t i = 0; i < 19; i++)
    {
        for (size_t j = 0; j < 19; j++)
        {
            char c;
            cin >> c;
            if (c == 'w')
                board[i][j] = 2;
            else if (c == 'b')
                board[i][j] = 1;
        }
    }

    maxDepth = 3;
}

agent::~agent()
{
    fclose(stdin);
}

// ============================================================================================================================================
// TODO:PRINTING FUNCTIONS
// prints all data
void agent::printData()
{
    cout << "Agent Tile\t: " << agentTile << endl;
    cout << "Time\t\t: " << time << endl;
    cout << "White Captures\t: " << whiteCaptures << endl;
    cout << "Black Captures\t: " << blackCaptures << endl;
    cout << "Board: " << endl;
    printBoard(board);
}

// ============================================================================================================================================
// TODO:GAMEPLAY FUNCTIONS
// Eval Functions
int agent::evaluateBoard(vector<vector<int> > currBoard, bool isMaximizing)
{
    return centralHeuristic(currBoard, isMaximizing ? agentTile : agentTile == 1 ? 2
                                                                                 : 1);
    // return randomHeuristic();
}

// Capture Functions
pair<vector<vector<int> >, int> agent::checkCaptures(vector<vector<int> > currBoard, int row, int col, int color)
{
	int opponent = (color == 1) ? 2 : 1; // determine the opponent's color
    int numCaptures = 0;

	// check for captures horizontally
	if (col > 2 && currBoard[row][col - 1] == opponent && currBoard[row][col - 2] == opponent && currBoard[row][col - 3] == color)
	{
		currBoard[row][col - 1] = 0;
		currBoard[row][col - 2] = 0;
        numCaptures++;
	}
	if (col < 16 && currBoard[row][col + 1] == opponent && currBoard[row][col + 2] == opponent && currBoard[row][col + 3] == color)
	{
		currBoard[row][col + 1] = 0;
		currBoard[row][col + 2] = 0;
        numCaptures++;
	}

	// check for captures vertically
	if (row > 2 && currBoard[row - 1][col] == opponent && currBoard[row - 2][col] == opponent && currBoard[row - 3][col] == color)
	{
		currBoard[row - 1][col] = 0;
		currBoard[row - 2][col] = 0;
        numCaptures++;
	}
	if (row < 16 && currBoard[row + 1][col] == opponent && currBoard[row + 2][col] == opponent && currBoard[row + 3][col] == color)
	{
		currBoard[row + 1][col] = 0;
		currBoard[row + 2][col] = 0;
        numCaptures++;
	}

	// check for captures diagonally
	if (row > 2 && col > 2 && currBoard[row - 1][col - 1] == opponent && currBoard[row - 2][col - 2] == opponent && currBoard[row - 3][col - 3] == color)
	{
		currBoard[row - 1][col - 1] = 0;
		currBoard[row - 2][col - 2] = 0;
        numCaptures++;
	}
	if (row < 16 && col < 16 && currBoard[row + 1][col + 1] == opponent && currBoard[row + 2][col + 2] == opponent && currBoard[row + 3][col + 3] == color)
	{
		currBoard[row + 1][col + 1] = 0;
		currBoard[row + 2][col + 2] = 0;
        numCaptures++;
	}
	if (row > 2 && col < 16 && currBoard[row - 1][col + 1] == opponent && currBoard[row - 2][col + 2] == opponent && currBoard[row - 3][col + 3] == color)
	{
		currBoard[row - 1][col + 1] = 0;
		currBoard[row - 2][col + 2] = 0;
        numCaptures++;
	}
	if (row < 16 && col > 2 && currBoard[row + 1][col - 1] == opponent && currBoard[row + 2][col - 2] == opponent && currBoard[row + 3][col - 3] == color)
	{
		currBoard[row + 2][col - 2] = 0;
		currBoard[row + 3][col - 3] = 0;
        numCaptures++;
	}
    
    return make_pair(currBoard, numCaptures);
}

// Check Win Function
bool agent::checkWin(vector<vector<int> > currBoard, int wCaps, int bCaps, int color, int row, int col)
{

	// check for capture wins
	if (wCaps == 5 || bCaps == 5)
		return true;

	// check for horizontal wins
	int count = 0;
	for (int c = max(0, col - 4); c <= min(18, col + 4); c++)
	{
		if (board[row][c] == color)
		{
			count++;
			if (count >= 5)
			{
				return true;
			}
		}
		else
		{
			count = 0;
		}
	}

	// check for vertical wins
	count = 0;
	for (int r = max(0, row - 4); r <= min(18, row + 4); r++)
	{
		if (board[r][col] == color)
		{
			count++;
			if (count >= 5)
			{
				return true;
			}
		}
		else
		{
			count = 0;
		}
	}

	// check for diagonal wins (top-left to bottom-right)
	count = 0;
	for (int r = max(0, row - 4), c = max(0, col - 4); r <= min(18, row + 4) && c <= min(18, col + 4); r++, c++)
	{
		if (board[r][c] == color)
		{
			count++;
			if (count >= 5)
			{
				return true;
			}
		}
		else
		{
			count = 0;
		}
	}

	// check for diagonal wins (bottom-left to top-right)
	count = 0;
	for (int r = min(18, row + 4), c = max(0, col - 4); r >= max(0, row - 4) && c <= min(18, col + 4); r--, c++)
	{
		if (board[r][c] == color)
		{
			count++;
			if (count >= 5)
			{
				return true;
			}
		}
		else
		{
			count = 0;
		}
	}

	// if no win was found, return false
	return false;
}

// Get Child Board positions
vector<pii> agent::getChildren(vector<vector<int> > currBoard)
{
    vector<pii> children;
    vector<pii> range(19, pair<int, int>(19, -1)); // [start, end] inclusive

    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            if (currBoard[i][j] != 0)
            {
                int startR = max(0, i - 4);
                int endR = min(18, i + 4);

                for (int k = startR; k <= endR; k++)
                {
                    range[k].first = max(0, min(range[k].first, j - 4));
                    range[k].second = min(18, max(range[k].second, j + 4));
                }
            }
        }
    }

    // printPairs(range); // DEBUG

    for (int i = 0; i < 19; i++)
    {
        for (int j = range[i].first; j <= range[i].second; j++)
        {
            if (currBoard[i][j] == 0)
                children.push_back(make_pair(i, j));
        }
    }

    return children;
}

// ALPHA BETA
int agent::alphaBeta(vector<vector<int> > currBoard, int wCaps, int bCaps, int depth, int alpha, int beta, bool isMaximizing)
{
    if (depth == 0)
        return evaluateBoard(currBoard, isMaximizing);

    if (isMaximizing)
    {
        int bestValue = -1000;
        vector<pii> children = getChildren(currBoard);
        // cout << children.size() << endl; // DEBUG

        for (size_t i = 0; i < children.size(); i++)
        {
            currBoard[children[i].first][children[i].second] = agentTile;
            pair<vector<vector<int> >, int> result = checkCaptures(currBoard, children[i].first, children[i].second, agentTile);
            if(checkWin(board, wCaps, bCaps, agentTile, children[i].first, children[i].second))
                return 1000;
            agentTile == 1 ? bCaps += result.second : wCaps += result.second;
            // printBoard(board); // DEBUG

            int value = alphaBeta(currBoard, wCaps, bCaps, depth - 1, alpha, beta, false);
            currBoard[children[i].first][children[i].second] = 0;
            bestValue = max(bestValue, value);
            alpha = max(alpha, bestValue);
            if (beta <= alpha)
                break;
        }

        return bestValue;
    }
    else
    {
        int bestValue = 1000;
        vector<pii> children = getChildren(currBoard);
        // cout << children.size() << endl; // DEBUG

        for (size_t i = 0; i < children.size(); i++)
        {
            currBoard[children[i].first][children[i].second] = agentTile == 1 ? 2 : 1;
            pair<vector<vector<int> >, int> result = checkCaptures(currBoard, children[i].first, children[i].second, agentTile == 1 ? 2 : 1);
            agentTile == 1 ? wCaps += result.second : bCaps += result.second;
            if(checkWin(board, wCaps, bCaps, agentTile == 1 ? 2 : 1, children[i].first, children[i].second))
                return -1000;
            // printBoard(board); // DEBUG

            int value = alphaBeta(currBoard, wCaps, bCaps, depth - 1, alpha, beta, true);
            currBoard[children[i].first][children[i].second] = 0;
            bestValue = max(bestValue, value);
            beta = min(beta, bestValue);
            if (beta <= alpha)
                break;
        }
        return bestValue;
    }
}

// PLAY GAME
void agent::playGame()
{
    int bestValue = -1000;
    int bestRow = -1;
    int bestCol = -1;

    vector<pii> children = getChildren(board);
    // cout << children.size() << endl; // DEBUG

    for (size_t i = 0; i < children.size(); i++)
    {
        board[children[i].first][children[i].second] = agentTile;
        // printBoard(board); // DEBUG
        int value = alphaBeta(board, whiteCaptures, blackCaptures, maxDepth, -1000, 1000, false);
        board[children[i].first][children[i].second] = 0;
        if (value > bestValue)
        {
            bestValue = value;
            bestRow = children[i].first;
            bestCol = children[i].second;
        }
    }

    cout << bestRow << " " << bestCol << endl; // DEBUG
}

// ============================================================================================================================================
// TODO:MAIN
int main()
{
    agent a("input/input.txt");
    a.printData(); // DEBUG
    a.playGame();

    return 0;
}