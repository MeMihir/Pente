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

#define infi 10000000000000
#define ninfi -10000000000000
#define pii pair<int, int>

unordered_map <uint64_t, long long int > transpositionTable;
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


// TODO =====================================================================================================
// TODO: OPENING MOVES

void initializeOpeningMoves()
{
    openingMoves[9101533770646912702] = "K10"; // opening white
    openingMoves[13777879953909322134] = "K9"; // opening black
    openingMoves[7886114032676240813] = "N10"; // K9
    openingMoves[15510469182678292660] = "N10"; // L9
    openingMoves[3024958866278058490] = "N10"; // M9
    openingMoves[17578088071886051271] = "N10"; // N9
    openingMoves[10943431002829706131] = "J7"; // O9
    openingMoves[4033035296802732547] = "L13"; // K8
    openingMoves[17167260374030425187] = "K14"; // M8
    openingMoves[7559183552038571392] = "N11"; // N8
    openingMoves[1554884274173108145] = "H7"; // O8
    openingMoves[16097626642881148955] = "N12"; // O7
    openingMoves[10367220700633438709] = "G9"; // K7
}

string getOpeningMove(int nTurn, int agentTile, uint64_t hash, vector <vector <int> > board)
{
    // opening moves
    if(nTurn == 1 && agentTile == 2) return "K10";
    if(nTurn == 1 && agentTile == 1) return "K9";
    
    // white second move
    cout<<"hash: "<<hash<<endl;
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

        if(blackI <= 9 && blackJ <= 9 && blackJ >= blackI) return "N10";
        if(blackI <= 9 && blackJ <= 9 && blackJ < blackI) return "N13";
        if(blackI <= 9 && blackJ >= 10 && blackJ + blackI < 19) return "G10";
        if(blackI <= 9 && blackJ >= 10 && blackJ + blackI >= 19) return "G13";
        if(blackI >= 10 && blackJ <= 9 && blackJ + blackI < 19) return "K13";
        if(blackI >= 10 && blackJ <= 9 && blackJ + blackI >= 19) return "G13";
        if(blackI >= 10 && blackJ >= 10 && blackJ >= blackI) return "K13";
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
        if(whiteI <= 9 && whiteJ <= 9 && whiteJ > whiteI) return "K11";
        if(whiteI < 9 && whiteJ <= 9 && whiteJ <= whiteI) return "J11";
        if(whiteI <= 9 && whiteJ >= 10 && whiteJ + whiteI <= 19) return "L11";
        if(whiteI <= 9 && whiteJ >= 10 && whiteJ + whiteI > 19) return "L10";
        if(whiteI >= 10 && whiteJ <= 9 && whiteJ + whiteI <= 19) return "L10";
        if(whiteI >= 10 && whiteJ <= 9 && whiteJ + whiteI >= 19) return "J9";
        if(whiteI >= 10 && whiteJ >= 10 && whiteJ >= whiteI) return "J8";
        return "L9";
    }
    return "";
}

long long int evaluateBoard(vector<vector<int> > currBoard, bool isMaximizing, int tile, int whiteCaptures=0, int blackCapture=0)
{
    // int heuristic = centralHeuristic(currBoard, isMaximizing ? agentTile : agentTile == 1 ? 2 : 1);
    // return isMaximizing ? heuristic : -heuristic;
	slidingHeuristic heuristic(currBoard, tile, whiteCaptures, blackCapture);
	return isMaximizing ? heuristic.slidingWindowHeuristicFull() : -heuristic.slidingWindowHeuristicFull();
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
    vector<pii> children;
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

		if(transpositionTable[newHash] == 0) {
			heuristic = evaluateBoard(currBoard, isMaximizing, agentTile, wCaps, bCaps); // Heuristic
			transpositionTable[newHash] = heuristic == 0 ? 1 : heuristic;
			// transpositionTable2[newHash] = make_pair(heuristic == 0 ? 1 : heuristic, children[i]);
			// missCount++; // DEBUG
		}
		else
			heuristic = transpositionTable[newHash];
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

		if(transpositionTable[newHash] == 0) {
			heuristic = evaluateBoard(currBoard, isMaximizing, opponentTile, wCaps, bCaps); // Heuristic
			transpositionTable[newHash] = heuristic == 0 ? 1 : heuristic;
			// transpositionTable2[newHash] = make_pair(heuristic == 0 ? 1 : heuristic, children[i]);
		}
		else
			heuristic = transpositionTable[newHash];
		// cout<<children[i].first<<" "<<children[i].second<<" "<<heuristic<<endl; // DEBUG
		
		moveOrderMin.push(make_pair(heuristic, children[i]));

		opponentTile == 1 ? bCaps -= numCaptures : wCaps -= numCaptures;
		zobrist.updateHash(oldHash); // update hash
	}
	return moveOrderMin;
}

// ALPHA BETA
long long int alphaBeta(vector<vector<int> > currBoard, int wCaps, int bCaps, int depth, long long int alpha, long long int beta, bool isMaximizing, int agentTile, ZobristHash hasher, float ForwardPruningPercentage, long long int MinHeuristic)
{
    int opponentTile = agentTile == 1 ? 2 : 1;

    if (depth == 0) {
		if(transpositionTable[hasher.hash()] == 0)
	    	return evaluateBoard(currBoard, isMaximizing, isMaximizing ? agentTile : opponentTile , wCaps, bCaps);
		return transpositionTable[hasher.hash()];
	}
	
	vector<pii> children;
	vector<pii> range;
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
			if(moveOrderMax.size() < fwdPruningChildren*(1+ (2-depth)*0.5))
				break;
			if(heuristic < maxHeuristic/MinHeuristic)
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
            long long int value = alphaBeta(newBoard, wCaps, bCaps, depth - 1, alpha, beta, false, agentTile, hasher, ForwardPruningPercentage, MinHeuristic);


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
			if(moveOrderMin.size() < fwdPruningChildren*(1+ (2-depth)*0.5))
				break;
			if(heuristic > (minHeuristic>0 ? minHeuristic/MinHeuristic : -1*minHeuristic/MinHeuristic))
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
            long long int value = alphaBeta(newBoard, wCaps, bCaps, depth - 1, alpha, beta, true, agentTile, hasher, ForwardPruningPercentage, MinHeuristic);

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
// TODO: AGENT FUNCTION

string indices_to_position(int row, int column) {
    if(column >= 8) column++;
    char letter = static_cast<char>(column + 'A');
    string number = to_string(19 - row);
    return string(1, letter) + number;
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

    // Hyperparameters
    int maxDepth;
    float FwdPrunePercent;
    long long int MinHeuristic;

public:
    agent(string path);
    ~agent();
    // void printData();
    void playGame();
    long long int evaluateBoard(vector<vector<int> > currBoard, bool isMaximizing);
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
        // cout << heuristic << " " << child.first << " " << child.second << endl; // DEBUG

        if(moveOrder.size() < fwdPruningChildren)
			break;
        if(heuristic < maxHeuristic/MinHeuristic)
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
            cout<<indices_to_position(child.first, child.second);
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
    // board[bestRow][bestCol] = agentTile; // DEBUG
    // printBoard(board); // DEBUG
}

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