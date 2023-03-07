#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>

using namespace std;
#include "hashing.h"

// TODO:ZOBRIST HASHING

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

void ZobristHash::printTable() {
    cout<<"[";
    for (size_t i = 0; i < 19; i++)
    {
        cout<<"[";
        for (size_t j = 0; j < 19; j++)
        {
            cout<<"[";
            for (size_t k = 0; k < 3; k++)
            {
                cout << hashTable[i][j][k] << ",";
            }
            cout << "]" << endl;
        }
        cout << "]" << endl;
    }
    cout<<"],"<<endl;
}