#include <iostream>
#include <vector>
#include <cstdlib>
#include <unordered_map>

using namespace std;
#include "globals.h"
#include "helpers.h"
#include "heuristics.h"

#define pii pair<int, int>

// returns random number between 0 and 500
int randomHeuristic()
{
    return rand() % 500;
}

int centralHeuristic(vector<vector<int> > board, int tile)
{
    int heuristic = 0;
    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            if (board[i][j] == tile)
            {
                heuristic += 200 - ((9 - i) * (9 - i) + (9 - j) * (9 - j));
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

    long long int heusristicOpen4s = infi;
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
    heuristicWindow5W[1940] = 10000000; // w w w w _
    heuristicWindow5W[700] = 10000000; // _ w w w w
    heuristicWindow5W[690] = 100000; // _ w w w _
    heuristicWindow5W[980] = 1000000000; // b b b b w
    heuristicWindow5W[1600] = 1000000000; // w b b b b
    heuristicWindow5W[1595] = 200000; // w b b b _
    heuristicWindow5W[355] = 200000; // _ b b b w

    heuristicWindow4W[380] = 1000; // w w w _
    heuristicWindow4W[138] = 1000; // _ w w w
    heuristicWindow4W[358] = 30; // w w _ w
    heuristicWindow4W[288] = 30; // w _ w w
    heuristicWindow4W[315] = 50; // Dynamic // w b b _
    heuristicWindow4W[73] = 50; // Dynamic // _ b b w
    // heuristicWindow4W[198] = 100000; // b b b w
    // heuristicWindow4W[319] = 100000; // w b b b

    // MAXIMIZE BLACK
    heuristicWindow5B[975] = infi; // b b b b b
    heuristicWindow5B[970] = 10000000; // b b b b _
    heuristicWindow5B[350] = 10000000; // _ b b b b
    heuristicWindow5B[345] = 100000; // _ b b b _
    heuristicWindow5B[1945] = 1000000000; // w w w w b
    heuristicWindow5B[1325] = 1000000000; // b w w w w
    heuristicWindow5B[1315] = 200000; // b w w w _ 
    heuristicWindow5B[695] = 200000; // _ w w w b

    heuristicWindow4B[190] = 1000; // b b b _
    heuristicWindow4B[69] = 1000; // _ b b b
    heuristicWindow4B[179] = 30; // b b _ b
    heuristicWindow4B[144] = 30; // b _ b b
    heuristicWindow4B[255] = 50; // Dynamic // b w w _
    heuristicWindow4B[134] = 50; // Dynamic // _ w w b
    // heuristicWindow4B[384] = 100000; // w w w b
    // heuristicWindow4B[263] = 100000; // b w w w
}

vector<long long int> slidingHeuristic::hashWindow4(int i, int j)
{
    vector<long long int> hashes(4, 0);

    long long int hashX = 0;
    if (i + 4 <= 19)
    {
        for (int k = i; k < i + 4; k++)
        {
            hashX = hashX * P + (k + 1) * board[k][j];
        }
    }
    hashes[0] = hashX;

    long long int hashY = 0;
    if (j + 4 <= 19)
    {
        for (int k = j; k < j + 4; k++)
        {
            hashY = hashY * P + (k + 1) * board[i][k];
        }
    }
    hashes[1] = hashY;

    long long int hashD1 = 0;
    if (i + 4 <= 19 && j + 4 <= 19)
    {
        for (int k = 0; k < 4; k++)
        {
            hashD1 = hashD1 * P + (k + 1) * board[i + k][j + k];
        }
    }
    hashes[2] = hashD1;

    long long int hashD2 = 0;
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

vector<long long int> slidingHeuristic::hashWindow5(int i, int j)
{
    vector<long long int> hashes(4, 0);
    long long int hashX = 0;
    if (i + 5 < 19)
    {
        for (int k = 0; k < 5; k++)
        {
            hashX = hashX * P + (k + 1) * board[k+i][j];
        }
    }
    hashes[0] = hashX;

    long long int hashY = 0;
    if (j + 5 <= 19)
    {
        for (int k = 0; k < 5; k++)
        {
            hashY = hashY * P + (k + 1) * board[i][j+k];
        }
    }
    hashes[1] = hashY;

    long long int hashD1 = 0;
    if (i + 5 <= 19 && j + 5 <= 19)
    {
        for (int k = 0; k < 5; k++)
        {
            hashD1 = hashD1 * P + (k + 1) * board[i + k][j + k];
        }
    }
    hashes[2] = hashD1;

    long long int hashD2 = 0;
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

long long int slidingHeuristic::checkOpen4s(int i, int j, int tile)
{
    long long int heuristic = 0;

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

long long int slidingHeuristic::slidingWindowHeuristicFull()
{
    long long int heuristic = 0;
    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            vector<long long int> hash5s = hashWindow5(i, j);
            vector<long long int> hash4s = hashWindow4(i, j);
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
        return heuristic - whiteCaptures*1000000 + blackCaptures*1000000;
    else
        return heuristic - blackCaptures*1000000 + whiteCaptures*1000000;
}

int slidingHeuristic::slidingWindowHeuristicPartial(int i, int j, vector<pii>range)
{
    int heuristic = 0;
    for (int i = 0; i < 19; i++)
    {
        for (int j = range[i].first ; j < range[i].second; j++)
        {
            vector<long long int> hash5s = hashWindow5(i, j);
            vector<long long int> hash4s = hashWindow4(i, j);
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