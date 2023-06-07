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

    heusristicOpen4s = infi;
    heuristic3toWin = 100; 
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
    heuristicWindow5W[1910] = 10000000; // w w w _ w
    heuristicWindow5W[1800] = 100000; // w w _ w w
    heuristicWindow5W[1450] = 10000000; // w _ w w w
    heuristicWindow5W[700] = 10000000; // _ w w w w
    heuristicWindow5W[690] = 1000000; // _ w w w _
    heuristicWindow5W[980] = 1000000000; // b b b b w
    heuristicWindow5W[995] = 1000000000; // b b b w b
    heuristicWindow5W[1050] = 1000000000; // b b w b b
    heuristicWindow5W[1225] = 1000000000; // b w b b b
    heuristicWindow5W[1600] = 1000000000; // w b b b b
    heuristicWindow5W[1595] = 200000; // w b b b _
    heuristicWindow5W[355] = 200000; // _ b b b w

    heuristicWindow4W[380] = 10000; // w w w _
    heuristicWindow4W[138] = 10000; // _ w w w
    heuristicWindow4W[358] = 8000; // w w _ w
    heuristicWindow4W[288] = 8000; // w _ w w
    heuristicWindow4W[315] = 5000; // Dynamic // w b b _
    heuristicWindow4W[73] = 5000; // Dynamic // _ b b w
    heuristicWindow4W[198] = 100000; // b b b w
    heuristicWindow4W[319] = 100000; // w b b b

    heuristicWindow4W[280] = 5000; // w _ w _
    heuristicWindow4B[108] = 5000; // _ w _ w
 
    // MAXIMIZE BLACK
    heuristicWindow5B[975] = infi; // b b b b b
    heuristicWindow5B[970] = 10000000; // b b b b _
    heuristicWindow5B[350] = 10000000; // _ b b b b
    heuristicWindow5W[955] = 10000000; // b b b _ b
    heuristicWindow5W[900] = 100000; // b b _ b b
    heuristicWindow5W[725] = 10000000; // b _ b b b
    heuristicWindow5B[345] = 1000000; // _ b b b _
    heuristicWindow5B[1945] = 1000000000; // w w w w b
    heuristicWindow5W[1930] = 1000000000; // w w w b w
    heuristicWindow5W[1875] = 1000000000; // w w b w w
    heuristicWindow5W[1700] = 1000000000; // w b w w w
    heuristicWindow5B[1325] = 1000000000; // b w w w w
    heuristicWindow5B[1315] = 200000; // b w w w _ 
    heuristicWindow5B[695] = 200000; // _ w w w b

    heuristicWindow4B[190] = 10000; // b b b _
    heuristicWindow4B[69] = 10000; // _ b b b
    heuristicWindow4B[179] = 8000; // b b _ b
    heuristicWindow4B[144] = 8000; // b _ b b
    heuristicWindow4B[255] = 5000; // Dynamic // b w w _
    heuristicWindow4B[134] = 5000; // Dynamic // _ w w b
    heuristicWindow4B[384] = 100000; // w w w b
    heuristicWindow4B[263] = 100000; // b w w w

    heuristicWindow4B[140] = 5000; // b _ b _
    heuristicWindow4B[54] = 5000; // _ b _ b
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

pii slidingHeuristic::toWin3(int i, int j)
{
    int toWin31 = 0;
    int toWin32 = 0;

    if (i + 5 < 19)
    {
        int count1 = 0;
        int count2 = 0;
        for (int k = 0; k < 5; k++)
        {
            if(board[i+k][j] == 1) count1++;
            if(board[i+k][j] == 2) count2++;
        }
        if(count1 == 0) toWin32++;
        if(count2 == 0) toWin31++;
    }
    if (j + 5 < 19)
    {
        int count1 = 0;
        int count2 = 0;
        for (int k = 0; k < 5; k++)
        {
            if(board[i][j+k] == 1) count1++;
            if(board[i][j+k] == 2) count2++;
        }
        if(count1 == 0) toWin32++;
        if(count2 == 0) toWin31++;
    }
    if (i + 5 < 19 && j + 5 < 19)
    {
        int count1 = 0;
        int count2 = 0;
        for (int k = 0; k < 5; k++)
        {
            if(board[i+k][j+k] == 1) count1++;
            if(board[i+k][j+k] == 2) count2++;
        }
        if(count1 == 0) toWin32++;
        if(count2 == 0) toWin31++;
    }
    if (i + 5 < 19 && j - 5 >= 0)
    {
        int count1 = 0;
        int count2 = 0;
        for (int k = 0; k < 5; k++)
        {
            if(board[i+k][j-k] == 1) count1++;
            if(board[i+k][j-k] == 2) count2++;
        }
        if(count1 == 0) toWin32++;
        if(count2 == 0) toWin31++;
    }

    return make_pair(toWin31, toWin32);
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
            pair<int, int> threeToWin = toWin3(i, j);
            if(tile == 1) {
                heuristic += threeToWin.first*heuristic3toWin;
                heuristic -= threeToWin.second*heuristic3toWin;
            }
            else {
                heuristic -= threeToWin.first*heuristic3toWin;
                heuristic += threeToWin.second*heuristic3toWin;
            }
        }
    }
    if(tile == 1)
        return heuristic - whiteCaptures*10000000 + blackCaptures*10000000;
    else
        return heuristic - blackCaptures*10000000 + whiteCaptures*10000000;
}

int slidingHeuristic::slidingWindowHeuristicPartial(vector<pii>range)
{   
    long long int heuristic = 0;
    if(range.size() == 0) return slidingWindowHeuristicFull();
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
            pair<int, int> threeToWin = toWin3(i, j);
            if(tile == 1) {
                heuristic += threeToWin.first*heuristic3toWin;
                heuristic -= threeToWin.second*heuristic3toWin;
            }
            else {
                heuristic -= threeToWin.first*heuristic3toWin;
                heuristic += threeToWin.second*heuristic3toWin;
            }
        }
    }
    if(tile == 1)
        return heuristic - whiteCaptures*1000000 + blackCaptures*1000000;
    else
        return heuristic - blackCaptures*1000000 + whiteCaptures*1000000;
}

// int main()
// {
//     vector<vector<int> > board(19, vector<int>(19, 0));
//     int tile, whiteCaptures, blackCaptures;

//     for(int i = 0; i < 19; i++) {
//         for(int j = 0; j < 19; j++) {
//             cin >> board[i][j];
//         }
//     }
//     cin >> tile >> whiteCaptures >> blackCaptures;
    
//     slidingHeuristic sh(board, tile, whiteCaptures, blackCaptures);
    
//     cout << sh.slidingWindowHeuristicFull() << endl;
//     return 0;
// }