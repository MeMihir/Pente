#include <iostream>
#include <vector>
#include <cstdlib>
#include <utility>
#include <chrono>
#include <random>
#include <string>
#include <tuple>
#include <queue>
#include <unordered_map>

using namespace std;
#include "globals.h"
#include "hashing.h"
#include "heuristics.h"
#include "helpers.h"
#include "gameAI.h"
#include "openingMoves.h"

#define pii pair<int, int>

unordered_map <uint64_t, long long int > transpositionTable;
// unordered_map <uint64_t, pair<long long int, pair <int, int> > > transpositionTable2;

class agent
{
private:
    // Inputs
    vector<vector<int> > board; // 1 = black, 2 = white, 0 = empty
    int whiteCaptures;
    int blackCaptures;
    double time;
    int agentTile; // 1 = black, 2 = white
    int nTurns;

    // Hyperparameters
    int maxDepth;
    float FwdPrunePercent;
    long long int MinHeuristic;

public:
    agent(string path);
    ~agent();
    void printData();
    void playGame();
    long long int evaluateBoard(vector<vector<int> > currBoard, bool isMaximizing);
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
    fclose(stdin);

    maxDepth = 2;
    FwdPrunePercent = 0.2;
    MinHeuristic = 1000;

    // read playdata.txt
    freopen("./playdata.txt", "r", stdin);
    if (cin.fail()) // file not found
    {
        freopen("./playdata.txt", "w", stdout);
        cout << "1" << endl << "0" << endl;
        fclose(stdout);
        freopen("./playdata.txt", "r", stdin);
    }

    int nHeuristics;
    cin >> nTurns;
    cin >> nHeuristics;
    for (int i = 0; i < nHeuristics; i++)
    {
        uint64_t hash;
        long long int heuristic;
        cin>>hash>>heuristic;
        transpositionTable[hash] = heuristic;
    }
    fclose(stdin);
 
    // cout<<nHeuristics<<transpositionTable.size()<<endl;

    // freopen("./playdata2.txt", "r", stdin);
    // if (cin.fail()) // file not found
    // {
    //     freopen("./playdata2.txt", "w", stdout);
    //     cout << "" << endl;
    //     fclose(stdout);
    //     freopen("./playdata2.txt", "r", stdin);
    // }
    // for (int i = 0; i < nHeuristics; i++)
    // {
    //     uint64_t hash;
    //     long long int heuristic;
    //     int x, y;
    //     cin>>hash>>heuristic>>x>>y;
    //     transpositionTable2[hash] = make_pair(heuristic, make_pair(x,y));
    // }
    // fclose(stdin);
}

agent::~agent()
{   
    freopen("./playdata.txt", "w", stdout);
    cout<<nTurns+1<<endl;
    cout<<transpositionTable.size();
    for(auto it:transpositionTable){
        cout<<endl<<it.first<<" "<<it.second;
    }
    fclose(stdout);
    // freopen("./playdata2.txt", "w", stdout);
    // for(auto it:transpositionTable2){
    //     cout<<endl<<it.first<<" "<<it.second.first<<" "<<it.second.second.first<<" "<<it.second.second.second;
    // }
    // fclose(stdout);
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
    cout << "Max Depth\t: " << maxDepth << endl;
    cout << "Board: " << endl;
    printBoard(board);
}

// ============================================================================================================================================
// TODO:GAMEPLAY FUNCTIONS
// Eval Functions
long long int agent::evaluateBoard(vector<vector<int> > currBoard, bool isMaximizing)
{
    // int heuristic = centralHeuristic(currBoard, isMaximizing ? agentTile : agentTile == 1 ? 2 : 1);
    // return isMaximizing ? heuristic : -heuristic;
    // return randomHeuristic();
    slidingHeuristic heuristic(currBoard, agentTile, whiteCaptures, blackCaptures);
	return isMaximizing ? heuristic.slidingWindowHeuristicFull() : -heuristic.slidingWindowHeuristicFull();
}

// PLAY GAME
void agent::playGame()
{
    long long int bestValue = ninfi;
    int bestRow = -1;
    int bestCol = -1;

    ZobristHash zobristHash(board);
    // zobristHash.printTable(); // DEBUG
    // cout<<zobristHash.hash()<<endl; // DEBUG

    // cout<<centralHeuristic(board, agentTile)<<endl; // DEBUG
    if(nTurns <= 2) {
        initializeOpeningMoves();
        cout << getOpeningMove(nTurns, agentTile, zobristHash.hash(), board) << endl;
        return;
    }

    vector<pii> children;
    vector<pii> range;
    priority_queue<pair<long long int, pii> > moveOrder;
	tie(children, range) = getChildren(board);
    // cout << children.size() << endl; // DEBUG

    moveOrder = moveOrderingMax(children, board, agentTile, true, whiteCaptures, blackCaptures, zobristHash, range);
    int numChildren = children.size();
	int fwdPruningChildren = numChildren*FwdPrunePercent;
    long long int maxHeuristic = moveOrder.top().first;

    while(!moveOrder.empty())
    {
        long long int heuristic = moveOrder.top().first;
        pii child = moveOrder.top().second;
        moveOrder.pop();
        cout << heuristic << " " << child.first << " " << child.second << endl; // DEBUG

        if(moveOrder.size() < fwdPruningChildren)
			break;
        // if(heuristic < maxHeuristic/MinHeuristic)
        //     break;

        board[child.first][child.second] = agentTile;
        vector<vector<int> > currBoard = board;
        int numCaps;

        uint64_t oldHash = zobristHash.hash();
        zobristHash.updateHash(child.first, child.second, agentTile); // update hash for move
        uint64_t hash;
        pair<int, uint64_t> temp;

        tie(currBoard, temp) = checkCaptures(currBoard, child.first, child.second, agentTile, zobristHash);
        tie(numCaps, hash) = temp;
        agentTile == 1 ? blackCaptures += numCaps : whiteCaptures += numCaps;
        if(checkWin(currBoard, whiteCaptures, blackCaptures, agentTile, child.first, child.second))
        {
            cout << child.first << " " << child.second << endl;
            return;
        }
            
        // printBoard(board); // DEBUG
        
        zobristHash.updateHash(hash); // update hash for captures
        // cout<<zobristHash.hash()<<" "; // DEBUG
        
        long long int value = alphaBeta(currBoard, whiteCaptures, blackCaptures, maxDepth, INT_MIN, INT_MAX, false, agentTile, zobristHash, FwdPrunePercent, MinHeuristic);
        
        board[child.first][child.second] = 0;
        agentTile == 1 ? blackCaptures -= numCaps : whiteCaptures -= numCaps;
        zobristHash.updateHash(oldHash); // update hash for undoing move
        // cout<<zobristHash.hash()<<endl; // DEBUG

        if (value > bestValue)
        {
            bestValue = value;
            bestRow = child.first;
            bestCol = child.second;
        }
    }
    cout << indices_to_position(bestRow, bestCol) << endl; // DEBUG
    // cout << bestRow << " " << bestCol << endl; // DEBUG
    board[bestRow][bestCol] = agentTile; // DEBUG
    printBoard(board); // DEBUG
}

// ============================================================================================================================================
// TODO:MAIN
int main()
{
    agent a("input/input.txt");
    // a.printData(); // DEBUG
    auto start = chrono::high_resolution_clock::now();
    a.playGame();
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    int minutes = chrono::duration_cast<chrono::minutes>(duration).count();
    int seconds = chrono::duration_cast<chrono::seconds>(duration).count() - minutes * 60;
    int milliseconds = chrono::duration_cast<chrono::milliseconds>(duration).count() - minutes * 60 * 1000 - seconds * 1000;
    cout << "Time taken: " << minutes << " minutes " << seconds << " seconds " << milliseconds << " milliseconds" << endl;

    return 0;
}