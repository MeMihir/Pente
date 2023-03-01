#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

class agent
{
private:
    // Inputs
    vector<vector<int>> board; // 1 = black, 2 = white, 0 = empty
    int whiteCaptures;
    int blackCaptures;
    double time;
    int agentTile; // 1 = black, 2 = white

    // Hyperparameters
    int maxDepth = 3;
public:
    agent(string path);
    ~agent();
    void printData();
    void playGame();
    int alphaBeta(vector<vector<int>> currBoard, int depth, int alpha, int beta, bool isMaximizing);
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
    board = vector<vector<int>>(19, vector<int>(19, 0));
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
}

agent::~agent()
{
    fclose(stdin);
}


// PRINTING FUNCTIONS
// prints all data
void agent::printData()
{
    cout << "Agent Tile\t: " << agentTile << endl;
    cout << "Time\t\t: " << time << endl;
    cout << "White Captures\t: " << whiteCaptures << endl;
    cout << "Black Captures\t: " << blackCaptures << endl;
    cout << "Board: " << endl;
    for (size_t i = 0; i < 19; i++)
    {
        for (size_t j = 0; j < 19; j++)
        {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
}

void agent::playGame()
{
    while (true)
    {
        int bestValue = -1000;
        int bestRow = -1;
        int bestCol = -1;
        for (size_t i = 0; i < 19; i++)
        {
            for (size_t j = 0; j < 19; j++)
            {
                if (board[i][j] == 0)
                {
                    board[i][j] = agentTile;
                    int value = alphaBeta(board, maxDepth, -1000, 1000, false);
                    board[i][j] = 0;
                    if (value > bestValue)
                    {
                        bestValue = value;
                        bestRow = i;
                        bestCol = j;
                    }
                }
            }
        }
        cout << bestRow << " " << bestCol << endl;
    }
}

// ALPHA BETA
int agent::alphaBeta(vector<vector<int>> currBoard, int depth, int alpha, int beta, bool isMaximizing)
{
    if (depth == 0)
        return randomHeuristic();

    if (isMaximizing)
    {
        int bestValue = -1000;
        for (size_t i = 0; i < 19; i++)
        {
            for (size_t j = 0; j < 19; j++)
            {
                if (currBoard[i][j] == 0)
                {
                    currBoard[i][j] = agentTile;
                    int value = alphaBeta(currBoard, depth - 1, alpha, beta, false);
                    currBoard[i][j] = 0;
                    bestValue = max(bestValue, value);
                    alpha = max(alpha, bestValue);
                    if (beta <= alpha)
                        break;
                }
            }
        }
        return bestValue;
    }
    else
    {
        int bestValue = 1000;
        for (size_t i = 0; i < 19; i++)
        {
            for (size_t j = 0; j < 19; j++)
            {
                if (currBoard[i][j] == 0)
                {
                    currBoard[i][j] = agentTile == 1 ? 2 : 1;
                    int value = alphaBeta(currBoard, depth - 1, alpha, beta, true);
                    currBoard[i][j] = 0;
                    bestValue = min(bestValue, value);
                    beta = min(beta, bestValue);
                    if (beta <= alpha)
                        break;
                }
            }
        }
        return bestValue;
    }
}

// returns random number between 0 and 500
int randomHeuristic()
{
    return rand() % 500;
}

// MAIN
int main()
{
    agent a("input/input.txt");
    a.printData(); // LOG
    a.playGame();

    return 0;
}