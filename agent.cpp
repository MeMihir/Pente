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

public:
    agent(string path);
    ~agent();
    void printData();
};

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


// returns random number between 0 and 500
int randomHeuristic()
{
    return rand() % 500;
}

int main()
{
    agent a("input/input.txt");
    a.printData(); // LOG

    return 0;
}