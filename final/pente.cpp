#include <iostream>
#include <vector>
#include <cstdlib>
#include <utility>
#include <chrono>
#include <random>
#include <string>
#include <tuple>
#include <queue>
#include <fstream>
#include <climits>
#include <unordered_map>

using namespace std;

#define infi 10000000000000
#define ninfi -10000000000000
#define pii pair<int, int>
#define MAX_DEPTH 3
#define FWD_PRUNE_PERCENT 0.5

unordered_map <uint64_t, long long int > oldTranspositionTable;
unordered_map <uint64_t, long long int > newTranspositionTable;
unordered_map <uint64_t, string > openingMoves;

// TODO =====================================================================================================
// TODO: ZOBIST HASHING
class ZobristHash
{
private:
    vector<vector<vector<uint64_t> > > hashTable;
    int SEED;
    uint64_t boardHash;
public:
    ZobristHash(vector<vector<int> > board);
    ~ZobristHash();
    uint64_t rand_uint64();
    void updateHash(int row, int col, int tile);
    void updateHash(uint64_t val);
    void printTable();
    uint64_t hash();
};

ZobristHash::ZobristHash(vector<vector<int> > board) {
    hashTable = vector<vector<vector<uint64_t> > >(19, vector<vector<uint64_t> >(19, vector<uint64_t>(3)));
    boardHash = 0;
    SEED = 123456789;

    for (size_t i = 0; i < 19; i++)
    {
        for (size_t j = 0; j < 19; j++)
        {
            for (size_t k = 0; k < 3; k++)
            {
                hashTable[i][j][k] = rand_uint64();
            }
        }
    }

    for (size_t i = 0; i < 19; i++)
    {
        for (size_t j = 0; j < 19; j++)
        {
            boardHash ^= hashTable[i][j][board[i][j]];
        }
    }
}

ZobristHash::~ZobristHash() {
// delete hash table
}

uint64_t ZobristHash::rand_uint64() {
    static mt19937_64 rng(SEED);
    return rng();
}

    // update hash after a move
void ZobristHash::updateHash(int row, int col, int tile) {
    boardHash ^= hashTable[row][col][tile];
}

void ZobristHash::updateHash(uint64_t val) {
    boardHash = val;
}

uint64_t ZobristHash::hash() {
    return boardHash;
}

// TODO =====================================================================================================
// TODO: SLIDING WINDOW HEURISTIC
class slidingHeuristic
{
private:
    vector<vector<int> > board;
    int tile;
    int whiteCaptures;
    int blackCaptures;
    int P;

    long long int heusristicOpen4s;
    long long int heuristic3toWin;
    unordered_map<int, long long int> heuristicWindow4W;
    unordered_map<int, long long int> heuristicWindow5W;
    unordered_map<int, long long int> heuristicWindow4B;
    unordered_map<int, long long int> heuristicWindow5B;

public:
    slidingHeuristic(vector<vector<int> > board, int tile, int whiteCaptures, int blackCaptures);
    // ~slidingHeuristic();
    void initializeHeuristicMaps();
    vector<long long int> hashWindow4(int i, int j);
    vector<long long int> hashWindow5(int i, int j);
    long long int checkOpen4s(int i, int j, int tile);
    long long int slidingWindowHeuristicFull();
    pair<int, int> toWin3(int i, int j);
    int slidingWindowHeuristicPartial(vector<pair<int, int> >range);
};

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

void slidingHeuristic::initializeHeuristicMaps()
{
    // MAXIMIZE WHITE
    heuristicWindow5W[1950] = infi; // w w w w w
    heuristicWindow5W[1940] = 1000000; // w w w w _
    heuristicWindow5W[1910] = 1000000; // w w w _ w
    heuristicWindow5W[1800] = 10000; // w w _ w w
    heuristicWindow5W[1450] = 1000000; // w _ w w w
    heuristicWindow5W[700] = 1000000; // _ w w w w
    heuristicWindow5W[690] = 100000; // _ w w w _
    heuristicWindow5W[980] = 100000000; // b b b b w
    heuristicWindow5W[995] = 100000000; // b b b w b
    heuristicWindow5W[1050] = 100000000; // b b w b b
    heuristicWindow5W[1225] = 100000000; // b w b b b
    heuristicWindow5W[1600] = 100000000; // w b b b b
    heuristicWindow5W[1595] = 20000; // w b b b _
    heuristicWindow5W[355] = 20000; // _ b b b w

    heuristicWindow4W[380] = 1000; // w w w _
    heuristicWindow4W[138] = 1000; // _ w w w
    heuristicWindow4W[358] = 800; // w w _ w
    heuristicWindow4W[288] = 800; // w _ w w
    heuristicWindow4W[315] = 500; // Dynamic // w b b _
    heuristicWindow4W[73] = 500; // Dynamic // _ b b w
    heuristicWindow4W[198] = 10000; // b b b w
    heuristicWindow4W[319] = 10000; // w b b b

    heuristicWindow4W[280] = 500; // w _ w _
    heuristicWindow4B[108] = 500; // _ w _ w
 
    // MAXIMIZE BLACK
    heuristicWindow5B[975] = infi; // b b b b b
    heuristicWindow5B[970] = 1000000; // b b b b _
    heuristicWindow5B[350] = 1000000; // _ b b b b
    heuristicWindow5W[955] = 1000000; // b b b _ b
    heuristicWindow5W[900] = 10000; // b b _ b b
    heuristicWindow5W[725] = 1000000; // b _ b b b
    heuristicWindow5B[345] = 100000; // _ b b b _
    heuristicWindow5B[1945] = 100000000; // w w w w b
    heuristicWindow5W[1930] = 100000000; // w w w b w
    heuristicWindow5W[1875] = 100000000; // w w b w w
    heuristicWindow5W[1700] = 100000000; // w b w w w
    heuristicWindow5B[1325] = 100000000; // b w w w w
    heuristicWindow5B[1315] = 20000; // b w w w _ 
    heuristicWindow5B[695] = 20000; // _ w w w b

    heuristicWindow4B[190] = 1000; // b b b _
    heuristicWindow4B[69] = 1000; // _ b b b
    heuristicWindow4B[179] = 800; // b b _ b
    heuristicWindow4B[144] = 800; // b _ b b
    heuristicWindow4B[255] = 500; // Dynamic // b w w _
    heuristicWindow4B[134] = 500; // Dynamic // _ w w b
    heuristicWindow4B[384] = 10000; // w w w b
    heuristicWindow4B[263] = 10000; // b w w w

    heuristicWindow4B[140] = 500; // b _ b _
    heuristicWindow4B[54] = 500; // _ b _ b
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
        if(count1 == 0 && count2 == 2) toWin32++;
        if(count2 == 0 && count2 == 2) toWin31++;
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
        if(count1 == 0 && count2 == 2) toWin32++;
        if(count2 == 0 && count2 == 2) toWin31++;
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
        if(count1 == 0 && count2 == 2) toWin32++;
        if(count2 == 0 && count2 == 2) toWin31++;
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
        if(count1 == 0 && count2 == 2) toWin32++;
        if(count2 == 0 && count2 == 2) toWin31++;
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


long long int needToWin(vector<vector<int> > board, int tile, int wCaps, int bCaps)
{
    int opponent = tile == 1 ? 2 : 1;

    int won = 0;
    int lost = 0;
    int win1 = 0;
    int loose1 = 0;
    int win2 = 0;
    int loose2 = 0;
    int win3 = 0;
    int loose3 = 0;
    int win4 = 0;
    int block4 = 0;
    int block3 = 0;

    // Heuristics
    long long int WON = infi;
    long long int LOST = -infi;
    long long int WIN1 = 100000000000;
    long long int LOOSE1 = -10000000000;
    long long int WIN2 = 100000000;
    long long int LOOSE2 = -10000000;
    long long int WIN3 = 100000;
    long long int LOOSE3 = -10000;
    long long int WIN4 = 100;
    long long int CAPTURE = 1000000000;
    long long int BLOCK4 = 1000000000000;
    long long int BLOCK3 = 1000000000;


    // Horizontal
    for (int i = 0; i < 19; i++)
    {
        int countA = 0;
        int countO = 0;
        int N = 0;
        for (int j = 0 ; j < 19; j++)
        {
            if(board[i][j] == tile)
                countA++;
            else if(board[i][j] == opponent)
                countO++;
            
            if(N<5) N++;
            else {
                if(board[i][j-N] == tile)
                    countA--;
                else if(board[i][j-N] == opponent)
                    countO--;
                

                if(countO == 0) {
                    if(countA == 5) won++;
                    else if(countA == 4) win1++;
                    else if(countA == 3) win2++;
                    else if(countA == 2) win3++;
                    else if(countA == 1) win4++;
                }
                else if(countA == 0) {
                    if(countO == 5) lost++;
                    else if(countO == 4) loose1++;
                    else if(countO == 3) loose2++;
                    else if(countO == 2) loose3++;
                }
                else {
                    if(countO == 4 && countA == 1) block4++;
                    else if(countO == 3 && countA == 1) block3++;
                }
            }
        }
        N=0;
        countA = 0;
        countO = 0;
    }

    // Vertical
    for (int i = 0; i < 19; i++)
    {
        int countA = 0;
        int countO = 0;
        int N = 0;
        for (int j = 0 ; j < 19; j++)
        {
            if(board[j][i] == tile)
                countA++;
            else if(board[j][i] == opponent)
                countO++;
            
            if(N<5) N++;
            else {
                if(board[j-N][i] == tile)
                    countA--;
                else if(board[j-N][i] == opponent)
                    countO--;
                

                if(countO == 0) {
                    if(countA == 5) won++;
                    else if(countA == 4) win1++;
                    else if(countA == 3) win2++;
                    else if(countA == 2) win3++;
                    else if(countA == 1) win4++;
                }
                else if(countA == 0) {
                    if(countO == 5) lost++;
                    else if(countO == 4) loose1++;
                    else if(countO == 3) loose2++;
                    else if(countO == 2) loose3++;
                }
                else {
                    if(countO == 4 && countA == 1) block4++;
                    else if(countO == 3 && countA == 1) block3++;
                }
            }
        }
        N = 0;
        countA = 0;
        countO = 0;
    }

    // Diagonal
    for (int k = 0; k < 19 + 19 - 1; k++) {
        int countA = 0;
        int countO = 0;
        int N = 0;
        for (int i = 0; i < 19; i++) {
            int j = k - i;
            if (j >= 0 && j < 19) {
                if(board[i][j] == tile)
                    countA++;
                else if(board[i][j] == opponent)
                    countO++;
                
                if(N<5) N++;
                else {
                    if(board[i-N][j+N] == tile)
                        countA--;
                    else if(board[i-N][j+N] == opponent)
                        countO--;
                    

                    if(countO == 0) {
                        if(countA == 5) won++;
                        else if(countA == 4) win1++;
                        else if(countA == 3) win2++;
                        else if(countA == 2) win3++;
                        else if(countA == 1) win4++;
                    }
                    else if(countA == 0) {
                        if(countO == 5) lost++;
                        else if(countO == 4) loose1++;
                        else if(countO == 3) loose2++;
                        else if(countO == 2) loose3++;
                    }
                    else {
                        if(countO == 4 && countA == 1) block4++;
                        else if(countO == 3 && countA == 1) block3++;
                    }
                }
            }
        }
        countA = 0;
        countO = 0;
        N=0;
    }

    // Anti-Diagonal
    for (int k = 0; k < 19 + 19 - 1; k++) {
        int countA = 0;
        int countO = 0;
        int N = 0;
        for (int i = 0; i < 19; i++) {
            int j = k - (19 - i - 1);
            if (j >= 0 && j < 19) {
                if(board[i][j] == tile)
                    countA++;
                else if(board[i][j] == opponent)
                    countO++;
                
                if(N<5) N++;
                else {
                    if(board[i-N][j-N] == tile)
                        countA--;
                    else if(board[i-N][j-N] == opponent)
                        countO--;
                    

                    if(countO == 0) {
                        if(countA == 5) won++;
                        else if(countA == 4) win1++;
                        else if(countA == 3) win2++;
                        else if(countA == 2) win3++;
                        else if(countA == 1) win4++;
                    }
                    else if(countA == 0) {
                        if(countO == 5) lost++;
                        else if(countO == 4) loose1++;
                        else if(countO == 3) loose2++;
                        else if(countO == 2) loose3++;
                    }
                    else {
                        if(countO == 4 && countA == 1) block4++;
                        else if(countO == 3 && countA == 1) block3++;
                    }
                }
            }
        }
        countA = 0;
        countO = 0;
        N=0;
    }

    long long int score = won*WON + win1*WIN1 + win2*WIN2 + win3*WIN3 + win4*WIN4 + lost*LOST + loose1*LOOSE1 + loose2*LOOSE2 + loose3*LOOSE3 + block4*BLOCK4 + block3*BLOCK3;
    long long int captureScore = (wCaps - bCaps)*CAPTURE;
    if(tile == 2) score += captureScore;
    else score -= captureScore;

    return score;
}

// TODO =====================================================================================================
// TODO: OPENING MOVES

void initializeOpeningMoves()
{
    openingMoves[9101533770646912702] = "10K"; // opening white
    openingMoves[13777879953909322134] = "9K"; // opening black
    openingMoves[7886114032676240813] = "10N"; // K9
    openingMoves[15510469182678292660] = "10N"; // L9
    openingMoves[3024958866278058490] = "10N"; // M9
    openingMoves[17578088071886051271] = "10N"; // N9
    openingMoves[10943431002829706131] = "7J"; // O9
    openingMoves[4033035296802732547] = "13L"; // K8
    openingMoves[17167260374030425187] = "14K"; // M8
    openingMoves[7559183552038571392] = "11N"; // N8
    openingMoves[1554884274173108145] = "7H"; // O8
    openingMoves[16097626642881148955] = "12N"; // O7
    openingMoves[10367220700633438709] = "9G"; // K7
}

string getOpeningMove(int nTurn, int agentTile, uint64_t hash, vector <vector <int> > board)
{
    // opening moves
    if(nTurn == 1 && agentTile == 2) return "10K";
    if(nTurn == 1 && agentTile == 1) return "9K";
    
    // white second move
    // cout<<"hash: "<<hash<<endl;
    if(openingMoves[hash] != "") return openingMoves[hash];
    if(nTurn == 2 && agentTile == 2) {
        int blackI = -1, blackJ = -1;
        for (int i = 0; i < 19; i++) {
            for (int j = 0; j < 19; j++) {
                if (board[i][j] == 1) {
                    blackI = i;
                    blackJ = j;
                    break;
                }
            }
            if(blackI != -1) break;
        }

        if(blackI <= 9 && blackJ <= 9 && blackJ >= blackI) return "10N";
        if(blackI <= 9 && blackJ <= 9 && blackJ < blackI) return "13N";
        if(blackI <= 9 && blackJ >= 10 && blackJ + blackI < 19) return "10G";
        if(blackI <= 9 && blackJ >= 10 && blackJ + blackI >= 19) return "13G";
        if(blackI >= 10 && blackJ <= 9 && blackJ + blackI < 19) return "13K";
        if(blackI >= 10 && blackJ <= 9 && blackJ + blackI >= 19) return "13G";
        if(blackI >= 10 && blackJ >= 10 && blackJ >= blackI) return "13K";
        return "K10";
    }

    // black second move
    if(nTurn == 2 && agentTile == 1) {
        int whiteI = -1, whiteJ = -1;
        for (int i = 0; i < 19; i++) {
            for (int j = 0; j < 19; j++) {
                if (board[i][j] == 2) {
                    whiteI = i;
                    whiteJ = j;
                    break;
                }
            }
            if(whiteI != -1) break;
        }

        // !RECHECK
        if(whiteI <= 9 && whiteJ <= 9 && whiteJ > whiteI) {
            if(board[8][9] == 2) return "12K";
            return "11K";
        }
        if(whiteI < 9 && whiteJ <= 9 && whiteJ <= whiteI) {
            if(board[8][8] == 2) return "12H";
            return "11J";
        }
        if(whiteI <= 9 && whiteJ >= 10 && whiteJ + whiteI <= 19) {
            if(board[8][10] == 2) return "12M";
            return "11L";
        }
        if(whiteI <= 9 && whiteJ >= 10 && whiteJ + whiteI > 19) {
            if(board[9][10] == 2) return "10M";
            return "10L";
        }
        if(whiteI >= 10 && whiteJ <= 9 && whiteJ + whiteI <= 19) {
            if(board[9][8] == 2) return "10M";
            return "10L";
        }
        if(whiteI >= 10 && whiteJ <= 9 && whiteJ + whiteI >= 19) {
            if(board[10][8] == 2) return "11L";
            return "9J";
        }
        if(whiteI >= 10 && whiteJ >= 10 && whiteJ >= whiteI) {
            if(board[11][8] == 2) return "9J";
            return "8J";
        }
        if(board[10][10] == 2) return "11J";
        return "9L";
    }
    return "";
}

long long int evaluateBoard(vector<vector<int> > currBoard, bool isMaximizing, int tile, int whiteCaptures=0, int blackCapture=0)
{
    // int heuristic = centralHeuristic(currBoard, isMaximizing ? agentTile : agentTile == 1 ? 2 : 1);
    // return isMaximizing ? heuristic : -heuristic;
	// slidingHeuristic heuristic(currBoard, tile, whiteCaptures, blackCapture);
	// return isMaximizing ? heuristic.slidingWindowHeuristicFull() : -heuristic.slidingWindowHeuristicFull();
    return isMaximizing ? needToWin(currBoard, tile, whiteCaptures, blackCapture) : -needToWin(currBoard, tile, whiteCaptures, blackCapture);

}


// TODO =====================================================================================================
// TODO: ALPHA BETA PRUNING

class Compare
{
public:
    bool operator()(pair<long long int, pii> a, pair<long long int, pii> b) {
        return a.first > b.first;
    }
};

// Capture Functions
pair<vector<vector<int> >, pair<int, uint64_t> > checkCaptures(vector<vector<int> > currBoard, int row, int col, int color, ZobristHash hasher)
{
	int opponent = (color == 1) ? 2 : 1; // determine the opponent's color
    int numCaptures = 0;

	// check for captures horizontally
	if (col > 2 && currBoard[row][col - 1] == opponent && currBoard[row][col - 2] == opponent && currBoard[row][col - 3] == color)
	{
		currBoard[row][col - 1] = 0;
		currBoard[row][col - 2] = 0;
		hasher.updateHash(row, col - 1, 0);
		hasher.updateHash(row, col - 2, 0);
        numCaptures++;
	}
	if (col < 16 && currBoard[row][col + 1] == opponent && currBoard[row][col + 2] == opponent && currBoard[row][col + 3] == color)
	{
		currBoard[row][col + 1] = 0;
		currBoard[row][col + 2] = 0;
		hasher.updateHash(row, col + 1, 0);
		hasher.updateHash(row, col + 2, 0);
        numCaptures++;
	}

	// check for captures vertically
	if (row > 2 && currBoard[row - 1][col] == opponent && currBoard[row - 2][col] == opponent && currBoard[row - 3][col] == color)
	{
		currBoard[row - 1][col] = 0;
		currBoard[row - 2][col] = 0;
		hasher.updateHash(row - 1, col, 0);
		hasher.updateHash(row - 2, col, 0);
        numCaptures++;
	}
	if (row < 16 && currBoard[row + 1][col] == opponent && currBoard[row + 2][col] == opponent && currBoard[row + 3][col] == color)
	{
		currBoard[row + 1][col] = 0;
		currBoard[row + 2][col] = 0;
		hasher.updateHash(row + 1, col, 0);
		hasher.updateHash(row + 2, col, 0);
        numCaptures++;
	}

	// check for captures diagonally
	if (row > 2 && col > 2 && currBoard[row - 1][col - 1] == opponent && currBoard[row - 2][col - 2] == opponent && currBoard[row - 3][col - 3] == color)
	{
		currBoard[row - 1][col - 1] = 0;
		currBoard[row - 2][col - 2] = 0;
		hasher.updateHash(row - 1, col - 1, 0);
		hasher.updateHash(row - 2, col - 2, 0);
        numCaptures++;
	}
	if (row < 16 && col < 16 && currBoard[row + 1][col + 1] == opponent && currBoard[row + 2][col + 2] == opponent && currBoard[row + 3][col + 3] == color)
	{
		currBoard[row + 1][col + 1] = 0;
		currBoard[row + 2][col + 2] = 0;
		hasher.updateHash(row + 1, col + 1, 0);
		hasher.updateHash(row + 2, col + 2, 0);
        numCaptures++;
	}
	if (row > 2 && col < 16 && currBoard[row - 1][col + 1] == opponent && currBoard[row - 2][col + 2] == opponent && currBoard[row - 3][col + 3] == color)
	{
		currBoard[row - 1][col + 1] = 0;
		currBoard[row - 2][col + 2] = 0;
		hasher.updateHash(row - 1, col + 1, 0);
		hasher.updateHash(row - 2, col + 2, 0);
        numCaptures++;
	}
	if (row < 16 && col > 2 && currBoard[row + 1][col - 1] == opponent && currBoard[row + 2][col - 2] == opponent && currBoard[row + 3][col - 3] == color)
	{
		currBoard[row + 2][col - 2] = 0;
		currBoard[row + 3][col - 3] = 0;
		hasher.updateHash(row + 1, col - 1, 0);
		hasher.updateHash(row + 2, col - 2, 0);
        numCaptures++;
	}
    
    return make_pair(currBoard, make_pair(numCaptures, hasher.hash()));
}

// Check Win Function
bool checkWin(vector<vector<int> > currBoard, int wCaps, int bCaps, int color, int row, int col)
{

	// check for capture wins
	if (wCaps == 5 || bCaps == 5)
		return true;

	// check for horizontal wins
	int count = 0;
	for (int c = max(0, col - 4); c <= min(18, col + 4); c++)
	{
		if (currBoard[row][c] == color)
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
		if (currBoard[r][col] == color)
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
		if (currBoard[r][c] == color)
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
		if (currBoard[r][c] == color)
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
pair<vector<pii>, vector<pii> > getChildren(vector<vector<int> > currBoard)
{
	// int R = 4;
	int R = 2;
    vector<pii> children(0);
    vector<pii> range(19, pair<int, int>(19, -1)); // [start, end] inclusive

    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            if (currBoard[i][j] != 0)
            {
                int startR = max(0, i - R);
                int endR = min(18, i + R);

                for (int k = startR; k <= endR; k++)
                {
                    range[k].first = max(0, min(range[k].first, j - R));
                    range[k].second = min(18, max(range[k].second, j + R));
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

    return make_pair(children, range);
}


// MOVE ORDERING
priority_queue <pair<long long int, pii> > moveOrderingMax(vector<pii> children, vector<vector<int> > board, int agentTile, bool isMaximizing, int wCaps, int bCaps, ZobristHash zobrist, vector<pii>range)
{
	priority_queue <pair<long long int, pii> > moveOrderMax;
    int opponentTile = agentTile == 1 ? 2 : 1;

	for (size_t i = 0; i < children.size(); i++)
	{
		vector<vector<int> > currBoard = board;
		int numCaptures;
		long long int heuristic;
		
		uint64_t oldHash = zobrist.hash(); // get old hash
		currBoard[children[i].first][children[i].second] = agentTile;
		zobrist.updateHash(children[i].first, children[i].second, agentTile); // update hash
		uint64_t newHash;
		pair<int, uint64_t> temp;

		tie(currBoard, temp) = checkCaptures(currBoard, children[i].first, children[i].second, agentTile, zobrist);
		tie(numCaptures, newHash) = temp;

		agentTile == 1 ? bCaps += numCaptures : wCaps += numCaptures;
		// zobrist.updateHash(newHash); // update hash

		if(oldTranspositionTable[newHash] == 0) {
			heuristic = evaluateBoard(currBoard, isMaximizing, agentTile, wCaps, bCaps); // Heuristic
			newTranspositionTable[newHash] = heuristic == 0 ? 1 : heuristic;
			oldTranspositionTable[newHash] = heuristic == 0 ? 1 : heuristic;
			// transpositionTable2[newHash] = make_pair(heuristic == 0 ? 1 : heuristic, children[i]);
			// missCount++; // DEBUG
		}
		else {
			heuristic = oldTranspositionTable[newHash];
            newTranspositionTable[newHash] = oldTranspositionTable[newHash];
        }
		// cout<<children[i].first<<" "<<children[i].second<<" "<<heuristic<<endl; // DEBUG
		
		moveOrderMax.push(make_pair(heuristic, children[i]));

		agentTile == 1 ? bCaps -= numCaptures : wCaps -= numCaptures;
		zobrist.updateHash(oldHash); // update hash
	}
	// cout<<missCount<<"\t"<<(double)missCount/(double)children.size()<<endl; // DEBUG
	return moveOrderMax;
}

priority_queue <pair<long long int, pii>, vector<pair<long long int, pii> >, Compare > moveOrderingMin(vector<pii> children, vector<vector<int> > board, int agentTile, bool isMaximizing, int wCaps, int bCaps, ZobristHash zobrist)
{
	priority_queue <pair<long long int, pii>, vector<pair<long long int, pii> >, Compare > moveOrderMin;
	int opponentTile = agentTile == 1 ? 2 : 1;
 
	for (size_t i = 0; i < children.size(); i++)
	{
		vector<vector<int> > currBoard = board;
		int numCaptures;
		long long int heuristic;
		
		uint64_t oldHash = zobrist.hash(); // get old hash
		currBoard[children[i].first][children[i].second] = opponentTile;
		zobrist.updateHash(children[i].first, children[i].second, opponentTile); // update hash
		uint64_t newHash;
		pair<int, uint64_t> temp;

		tie(currBoard, temp) = checkCaptures(board, children[i].first, children[i].second, opponentTile, zobrist);
		tie(numCaptures, newHash) = temp;

		opponentTile == 1 ? bCaps += numCaptures : wCaps += numCaptures;
		// zobrist.updateHash(newHash); // update hash

		if(oldTranspositionTable[newHash] == 0) {
			heuristic = evaluateBoard(currBoard, isMaximizing, opponentTile, wCaps, bCaps); // Heuristic
			oldTranspositionTable[newHash] = heuristic == 0 ? 1 : heuristic;
			newTranspositionTable[newHash] = heuristic == 0 ? 1 : heuristic;
			// transpositionTable2[newHash] = make_pair(heuristic == 0 ? 1 : heuristic, children[i]);
		}
		else {
			heuristic = oldTranspositionTable[newHash];
            newTranspositionTable[newHash] = oldTranspositionTable[newHash];
        }
		// cout<<children[i].first<<" "<<children[i].second<<" "<<heuristic<<endl; // DEBUG
		
		moveOrderMin.push(make_pair(heuristic, children[i]));

		opponentTile == 1 ? bCaps -= numCaptures : wCaps -= numCaptures;
		zobrist.updateHash(oldHash); // update hash
	}
	return moveOrderMin;
}

// ALPHA BETA
long long int alphaBeta(vector<vector<int> > currBoard, int wCaps, int bCaps, int depth, long long int alpha, long long int beta, bool isMaximizing, int agentTile, ZobristHash hasher, float ForwardPruningPercentage, long long int FractionalPruning)
{
    int opponentTile = agentTile == 1 ? 2 : 1;

    if (depth == 0) {
		if(oldTranspositionTable[hasher.hash()] == 0) {
	    	long long int value = evaluateBoard(currBoard, isMaximizing, isMaximizing ? agentTile : opponentTile , wCaps, bCaps);
            newTranspositionTable[hasher.hash()] = value == 0 ? 1 : value;
            return value;
        }
        newTranspositionTable[hasher.hash()] = oldTranspositionTable[hasher.hash()];
		return oldTranspositionTable[hasher.hash()];
	}
	
	vector<pii> children(0);
	vector<pii> range(0);
	tie(children, range) = getChildren(currBoard);
	int numChildren = children.size();
	int fwdPruningChildren = numChildren*ForwardPruningPercentage;

    if (isMaximizing)
    {
        long long int bestValue = ninfi;
		// cout << children.size() << endl; // DEBUG

		// ?MOVE ORDERING
		priority_queue <pair<long long int, pii> > moveOrderMax = moveOrderingMax(children, currBoard, agentTile, isMaximizing, wCaps, bCaps, hasher, range);
		
		long long int maxHeuristic = moveOrderMax.top().first;

        while(!moveOrderMax.empty())
        {
			long long int heuristic = moveOrderMax.top().first;
			pii child = moveOrderMax.top().second;
			moveOrderMax.pop();

			// ?FWD PRUNING
			if(moveOrderMax.size() < fwdPruningChildren)
				break;
			if(maxHeuristic > 0 && heuristic < maxHeuristic/FractionalPruning)
				break;

            currBoard[child.first][child.second] = agentTile;
			vector<vector<int> > newBoard = currBoard;
			int numCaptures;
			
			uint64_t oldHash = hasher.hash(); // get old hash
        	hasher.updateHash(child.first, child.second, agentTile); // update hash for move
			uint64_t hash;
			pair<int, uint64_t> temp;

            tie(newBoard, temp) = checkCaptures(currBoard, child.first, child.second, agentTile, hasher);
			tie(numCaptures, hash) = temp;

            agentTile == 1 ? bCaps += numCaptures : wCaps += numCaptures;
            if(checkWin(newBoard, wCaps, bCaps, agentTile, child.first, child.second))
                return INT_MAX;
            // printBoard(board); // DEBUG

			hasher.updateHash(hash); // update hash for captures
            long long int value = alphaBeta(newBoard, wCaps, bCaps, depth - 1, alpha, beta, false, agentTile, hasher, ForwardPruningPercentage, FractionalPruning);


            currBoard[child.first][child.second] = 0;
            agentTile == 1 ? bCaps -= numCaptures : wCaps -= numCaptures; // undo captures
			hasher.updateHash(oldHash); // update hash for undoing move
			
			bestValue = max(bestValue, value);
            alpha = max(alpha, bestValue);
            if (beta <= alpha)
                break;
        }

        return bestValue;
    }
    else
    {
        long long int bestValue = infi;
        // cout << children.size() << endl; // DEBUG
		int opponentTile = agentTile == 1 ? 2 : 1;

		// ?MOVE ORDERING
		priority_queue <pair<long long int, pii>, vector<pair<long long int, pii> >, Compare > moveOrderMin = moveOrderingMin(children, currBoard, agentTile, isMaximizing, wCaps, bCaps, hasher);
		long long int minHeuristic = moveOrderMin.top().first;
        while(!moveOrderMin.empty())
        {
			long long int heuristic = moveOrderMin.top().first;
			pii child = moveOrderMin.top().second;
			moveOrderMin.pop();

			// ?FWD PRUNING
			if(moveOrderMin.size() < fwdPruningChildren)
				break;
			if(minHeuristic<0 && heuristic > minHeuristic/FractionalPruning)
				break;

			vector<vector<int> > newBoard = currBoard;
            newBoard[child.first][child.second] = opponentTile;
			int numCaptures;
			
			uint64_t oldHash = hasher.hash(); // get old hash
        	hasher.updateHash(child.first, child.second, opponentTile); // update hash for move
			uint64_t hash;
			pair<int, uint64_t> temp;

            tie(newBoard, temp) = checkCaptures(currBoard, child.first, child.second, opponentTile, hasher);
			tie(numCaptures, hash) = temp;

			// ?CHECK IF NEXT LINE IS CORRECT
            opponentTile == 1 ? bCaps += numCaptures : wCaps += numCaptures;
            if(checkWin(newBoard, wCaps, bCaps, opponentTile, child.first, child.second))
                return INT_MIN;
            // printBoard(board); // DEBUG
			
			hasher.updateHash(hash); // update hash for captures
            long long int value = alphaBeta(newBoard, wCaps, bCaps, depth - 1, alpha, beta, true, agentTile, hasher, ForwardPruningPercentage, FractionalPruning);

			opponentTile == 1 ? bCaps -= numCaptures : wCaps -= numCaptures; // undo captures
			hasher.updateHash(oldHash); // update hash for undoing move

            bestValue = max(bestValue, value);
            beta = min(beta, bestValue);
            if (beta <= alpha)
                break;
        }
        return bestValue;
    }
}

// TODO =====================================================================================================
// TODO: HELPERS
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

// TODO =====================================================================================================
// TODO: AGENT FUNCTION

string indices_to_position(int row, int column) {
    if(column >= 8) column++;
    char letter = static_cast<char>(column + 'A');
    string number = to_string(19 - row);
    return number + string(1, letter) ;
}


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
    string BEST_MOVE;

    // Hyperparameters
    int maxDepth;
    float FwdPrunePercent;
    long long int FractionalPruning;

public:
    agent(string path);
    ~agent();
    // void printData();
    void playGame();
    // long long int evaluateBoard(vector<vector<int> > currBoard, bool isMaximizing);
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

    maxDepth = MAX_DEPTH;
    FwdPrunePercent = 0.5;
    FractionalPruning = 100;

    // read playdata.txt

    FILE *fp = fopen("playdata.txt", "r");
    if(fp == NULL)
    {
        nTurns = 1;
    }
    else {
        fclose(fp);
        freopen("./playdata.txt", "r", stdin);
        int nHeuristics;
        cin >> nTurns;
        cin >> nHeuristics;
        for (int i = 0; i < nHeuristics; i++)
        {
            uint64_t hash;
            long long int heuristic;
            cin>>hash>>heuristic;
            oldTranspositionTable[hash] = heuristic;
        }
        fclose(stdin);
    }
    
 
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
    
    // write best move to output.txt
    fstream fout;
    fout.open("output.txt", ios::out);
    fout << BEST_MOVE;
    fout.close();

    freopen("playdata.txt", "w", stdout);
    cout<<nTurns+1<<endl;
    cout<<newTranspositionTable.size();
    for(auto it:newTranspositionTable){
        cout<<endl<<it.first<<" "<<it.second;
    }
    fclose(stdout);
    // freopen("./playdata2.txt", "w", stdout);
    // for(auto it:transpositionTable2){
    //     cout<<endl<<it.first<<" "<<it.second.first<<" "<<it.second.second.first<<" "<<it.second.second.second;
    // }
    // fclose(stdout);
}

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
        BEST_MOVE = getOpeningMove(nTurns, agentTile, zobristHash.hash(), board);
        return;
    }

    vector<pii> children(0);
    vector<pii> range(0);
    priority_queue<pair<long long int, pii> > moveOrder;
	tie(children, range) = getChildren(board);
    // cout << children.size() << endl; // DEBUG

    moveOrder = moveOrderingMax(children, board, agentTile, true, whiteCaptures, blackCaptures, zobristHash, range);
    int numChildren = children.size();
    long long int maxHeuristic = moveOrder.top().first;


    // FWD PRUNING ADJUSTMENT
    if(numChildren >70 && maxDepth == MAX_DEPTH)
        FwdPrunePercent = 0.3;
    else
        FwdPrunePercent = 0.5;

	int fwdPruningChildren = numChildren*FwdPrunePercent;
    // cout<<numChildren<<" "<<fwdPruningChildren<<endl; // DEBUG


    // DEPTH ADJUSTMENT
    // if(maxHeuristic < 10000000)
    //     maxDepth = MAX_DEPTH - 1;
    if(time < 120)
        maxDepth = MAX_DEPTH - 1;
    if(time < 10)
        maxDepth = MAX_DEPTH - 2;

    while(!moveOrder.empty())
    {
        long long int heuristic = moveOrder.top().first;
        pii child = moveOrder.top().second;
        moveOrder.pop();
        // cout << heuristic << " " << indices_to_position(child.first, child.second) << endl; // DEBUG

        if(moveOrder.size() < fwdPruningChildren)
			break;
        if(maxHeuristic > 0 && heuristic < maxHeuristic/FractionalPruning)
            break;

        vector<vector<int> > currBoard = board;
        currBoard[child.first][child.second] = agentTile;
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
            BEST_MOVE = indices_to_position(child.first, child.second);
            return;
        }
            
        // printBoard(board); // DEBUG
        
        zobristHash.updateHash(hash); // update hash for captures
        // cout<<zobristHash.hash()<<" "; // DEBUG
        
        long long int value = alphaBeta(currBoard, whiteCaptures, blackCaptures, maxDepth, INT_MIN, INT_MAX, false, agentTile, zobristHash, FwdPrunePercent, FractionalPruning);
        
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
    // freopen("output.txt", "w", stdout);
    BEST_MOVE = indices_to_position(bestRow, bestCol);
    // fclose(stdout);
    // cout << bestRow << " " << bestCol << endl; // DEBUG
    // board[bestRow][bestCol] = agentTile; // DEBUG
    // printBoard(board); // DEBUG
}

int main()
{
    agent a("input.txt");
    // a.printData(); // DEBUG
    // auto start = chrono::high_resolution_clock::now();
    a.playGame();
    // auto end = chrono::high_resolution_clock::now();
    // auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    // int minutes = chrono::duration_cast<chrono::minutes>(duration).count();
    // int seconds = chrono::duration_cast<chrono::seconds>(duration).count() - minutes * 60;
    // int milliseconds = chrono::duration_cast<chrono::milliseconds>(duration).count() - minutes * 60 * 1000 - seconds * 1000;
    // cout << "Time taken: " << minutes << " minutes " << seconds << " seconds " << milliseconds << " milliseconds" << endl;

    return 0;
}