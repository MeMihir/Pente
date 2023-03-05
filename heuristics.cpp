#include <iostream>
#include <vector>
#include <cstdlib>
#include <unordered_map>

using namespace std;
#include "helpers.h"
#include "heuristics.h"

#define infi 1000000000
#define ninfi -1000000000
#define pii pair<int, int>

// returns random number between 0 and 500
int randomHeuristic()
{
    return rand() % 500;
}

int centralHeuristic(vector<vector<int> > board, int tile)
{
    int heuristic = 0;
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (board[i][j] == tile)
            {
                heuristic += 100 - ((9 - i) * (9 - i) + (9 - j) * (9 - j));
            }
        }
    }
    return heuristic;
}

slidingHeuristic::slidingHeuristic(vector<vector<int> > board, int tile, int whiteCaptures, int blackCaptures)
{
    this->board = board;
    this->tile = tile;
    this->whiteCaptures = whiteCaptures;
    this->blackCaptures = blackCaptures;
    P = 5;

    int heusristicOpen4s = infi;
    initializeHeuristicMaps();
}

slidingHeuristic::~slidingHeuristic()
{
    // delete board;
}

void slidingHeuristic::initializeHeuristicMaps()
{
    // MAXIMIZE WHITE
    heuristicWindow5W[1950] = infi; // w w w w w
    heuristicWindow5W[1940] = 5000; // w w w w _
    heuristicWindow5W[700] = 5000; // _ w w w w
    heuristicWindow5W[690] = 1000; // _ w w w _
    heuristicWindow5W[980] = 10000; // b b b b w
    heuristicWindow5W[1600] = 10000; // w b b b b
    heuristicWindow5W[1595] = 2000; // w b b b _
    heuristicWindow5W[355] = 2000; // _ b b b w

    heuristicWindow4W[380] = 800; // w w w _
    heuristicWindow4W[138] = 800; // _ w w w
    heuristicWindow4W[358] = 300; // w w _ w
    heuristicWindow4W[288] = 300; // w _ w w
    heuristicWindow4W[315] = 500; // Dynamic // w b b _
    heuristicWindow4W[73] = 500; // Dynamic // _ b b w
    heuristicWindow4W[198] = 1000; // b b b w
    heuristicWindow4W[319] = 1000; // w b b b

    // MAXIMIZE BLACK
    heuristicWindow5B[975] = infi; // b b b b b
    heuristicWindow5B[970] = 5000; // b b b b _
    heuristicWindow5B[350] = 5000; // _ b b b b
    heuristicWindow5B[345] = 1000; // _ b b b _
    heuristicWindow5B[1945] = 10000; // w w w w b
    heuristicWindow5B[1325] = 10000; // b w w w w
    heuristicWindow5B[1315] = 2000; // b w w w _ 
    heuristicWindow5B[695] = 2000; // _ w w w b

    heuristicWindow4B[190] = 800; // b b b _
    heuristicWindow4B[69] = 800; // _ b b b
    heuristicWindow4B[179] = 300; // b b _ b
    heuristicWindow4B[144] = 300; // b _ b b
    heuristicWindow4B[255] = 500; // Dynamic // b w w _
    heuristicWindow4B[134] = 500; // Dynamic // _ w w b
    heuristicWindow4B[384] = 1000; // w w w b
    heuristicWindow4B[263] = 1000; // b w w w
}

vector<int> slidingHeuristic::hashWindow4(int i, int j)
{
    vector<int> hashes(4, 0);

    int hashX = 0;
    if (i + 4 <= 19)
    {
        for (int k = i; k < i + 4; k++)
        {
            hashX = hashX * P + (k + 1) * board[k][j];
        }
    }
    hashes[0] = hashX;

    int hashY = 0;
    if (j + 4 <= 19)
    {
        for (int k = j; k < j + 4; k++)
        {
            hashY = hashY * P + (k + 1) * board[i][k];
        }
    }
    hashes[1] = hashY;

    int hashD1 = 0;
    if (i + 4 <= 19 && j + 4 <= 19)
    {
        for (int k = 0; k < 4; k++)
        {
            hashD1 = hashD1 * P + (k + 1) * board[i + k][j + k];
        }
    }
    hashes[2] = hashD1;

    int hashD2 = 0;
    if (i + 4 <= 19 && j - 4 >= 0)
    {
        for (int k = 0; k < 4; k++)
        {
            hashD2 = hashD2 * P + (k + 1) * board[i + k][j - k];
        }
    }
    hashes[3] = hashD2;

    return hashes;
}

vector<int> slidingHeuristic::hashWindow5(int i, int j)
{
    vector<int> hashes(4, 0);
    int hashX = 0;
    if (i + 5 < 19)
    {
        for (int k = 0; k < 5; k++)
        {
            hashX = hashX * P + (k + 1) * board[k+i][j];
        }
    }
    hashes[0] = hashX;

    int hashY = 0;
    if (j + 5 <= 19)
    {
        for (int k = 0; k < 5; k++)
        {
            hashY = hashY * P + (k + 1) * board[i][j+k];
        }
    }
    hashes[1] = hashY;

    int hashD1 = 0;
    if (i + 5 <= 19 && j + 5 <= 19)
    {
        for (int k = 0; k < 5; k++)
        {
            hashD1 = hashD1 * P + (k + 1) * board[i + k][j + k];
        }
    }
    hashes[2] = hashD1;

    int hashD2 = 0;
    if (i + 5 <= 19 && j - 5 >= 0)
    {
        for (int k = 0; k < 5; k++)
        {
            hashD2 = hashD2 * P + (k + 1) * board[i + k][j - k];
        }
    }
    hashes[3] = hashD2;

    return hashes;
}

int slidingHeuristic::checkOpen4s(int i, int j, int tile)
{
    int heuristic = 0;

    if (i + 5 < 19 && board[i][j] == 0 && board[i + 1][j] == tile && board[i + 2][j] == tile && board[i + 3][j] == tile && board[i + 4][j] == tile && board[i + 5][j] == 0)
    {
        return heusristicOpen4s;
    }
    if (j + 5 < 19 && board[i][j] == 0 && board[i][j + 1] == tile && board[i][j + 2] == tile && board[i][j + 3] == tile && board[i][j + 4] == tile && board[i][j + 5] == 0)
    {
        return heusristicOpen4s;
    }
    if (i + 5 < 19 && j + 5 < 19 && board[i][j] == 0 && board[i + 1][j + 1] == tile && board[i + 2][j + 2] == tile && board[i + 3][j + 3] == tile && board[i + 4][j + 4] == tile && board[i + 5][j + 5] == 0)
    {
        return heusristicOpen4s;
    }
    if (i + 5 < 19 && j - 5 >= 0 && board[i][j] == 0 && board[i + 1][j - 1] == tile && board[i + 2][j - 2] == tile && board[i + 3][j - 3] == tile && board[i + 4][j - 4] == tile && board[i + 5][j - 5] == 0)
    {
        return heusristicOpen4s;
    }

    return heuristic;
}

int slidingHeuristic::slidingWindowHeuristicFull()
{
    int heuristic = 0;
    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            vector<int> hash5s = hashWindow5(i, j);
            vector<int> hash4s = hashWindow4(i, j);
            for (int k = 0; k < 4; k++)
            {  
                if(tile == 1) {
                    heuristic += heuristicWindow5B[hash5s[k]] - heuristicWindow5W[hash5s[k]];
                    heuristic += heuristicWindow4B[hash4s[k]] - heuristicWindow4W[hash4s[k]];
                    heuristic += checkOpen4s(i, j, 1);
                }
                else {
                    heuristic += heuristicWindow5W[hash5s[k]] - heuristicWindow5B[hash5s[k]];
                    heuristic += heuristicWindow4W[hash4s[k]] - heuristicWindow4B[hash4s[k]];
                    heuristic += checkOpen4s(i, j, 2);
                }
            }
        }
    }
    if(tile == 1)
        return heuristic - whiteCaptures*1000 + blackCaptures*1000;
    else
        return heuristic - blackCaptures*1000 + whiteCaptures*1000;
}

int slidingHeuristic::slidingWindowHeuristicPartial(int i, int j, vector<pii>range)
{
    int heuristic = 0;
    for (int i = 0; i < 19; i++)
    {
        for (int j = range[i].first ; j < range[i].second; j++)
        {
            vector<int> hash5s = hashWindow5(i, j);
            vector<int> hash4s = hashWindow4(i, j);
            for (int k = 0; k < 4; k++)
            {  
                if(tile == 1) {
                    heuristic += heuristicWindow5B[hash5s[k]] - heuristicWindow5W[hash5s[k]];
                    heuristic += heuristicWindow4B[hash4s[k]] - heuristicWindow4W[hash4s[k]];
                    heuristic += checkOpen4s(i, j, 1);
                }
                else {
                    heuristic += heuristicWindow5W[hash5s[k]] - heuristicWindow5B[hash5s[k]];
                    heuristic += heuristicWindow4W[hash4s[k]] - heuristicWindow4B[hash4s[k]];
                    heuristic += checkOpen4s(i, j, 2);
                }
            }
        }
    }
    if(tile == 1)
        return heuristic - whiteCaptures*1000 + blackCaptures*1000;
    else
        return heuristic - blackCaptures*1000 + whiteCaptures*1000;
}