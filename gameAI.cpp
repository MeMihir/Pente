#include <iostream>
#include <vector>
#include <cstdlib>
#include <utility>
#include <unordered_map>
#include <string>
#include <tuple>
#include <queue>

using namespace std;
#include "globals.h"
#include "heuristics.h"
#include "hashing.h"
#include "gameAI.h"
#include "helpers.h"

#define pii pair<int, int>

int missCount = 0;

// Evaluate Board
int evaluateBoard(vector<vector<int> > currBoard, bool isMaximizing, int agentTile, int whiteCaptures=0, int blackCapture=0)
{
    int heuristic = centralHeuristic(currBoard, isMaximizing ? agentTile : agentTile == 1 ? 2 : 1);
    return isMaximizing ? heuristic : -heuristic;
	// slidingHeuristic heuristic(currBoard, agentTile, whiteCaptures, blackCapture);
	// return isMaximizing ? heuristic.slidingWindowHeuristicFull() : -heuristic.slidingWindowHeuristicFull();
}

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

    return make_pair(children, range);
}

bool Compare::operator()(pair<int, pii> a, pair<int, pii> b)
{
	return a.first > b.first;
}

// MOVE ORDERING
priority_queue <pair<int, pii> > moveOrderingMax(vector<pii> children, vector<vector<int> > board, int agentTile, int depth, int alpha, int beta, bool isMaximizing, int wCaps, int bCaps, ZobristHash zobrist)
{
	priority_queue <pair<int, pii> > moveOrderMax;

	for (size_t i = 0; i < children.size(); i++)
	{
		vector<vector<int> > currBoard = board;
		int numCaptures;
		int heuristic;
		
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

priority_queue <pair<int, pii>, vector<pair<int, pii> >, Compare > moveOrderingMin(vector<pii> children, vector<vector<int> > board, int agentTile, int depth, int alpha, int beta, bool isMaximizing, int wCaps, int bCaps, ZobristHash zobrist)
{
	priority_queue <pair<int, pii>, vector<pair<int, pii> >, Compare > moveOrderMin;
 
	for (size_t i = 0; i < children.size(); i++)
	{
		vector<vector<int> > currBoard = board;
		int numCaptures;
		int heuristic;
		
		uint64_t oldHash = zobrist.hash(); // get old hash
		currBoard[children[i].first][children[i].second] = agentTile;
		zobrist.updateHash(children[i].first, children[i].second, agentTile); // update hash
		uint64_t newHash;
		pair<int, uint64_t> temp;

		tie(currBoard, temp) = checkCaptures(board, children[i].first, children[i].second, agentTile, zobrist);
		tie(numCaptures, newHash) = temp;

		agentTile == 1 ? bCaps += numCaptures : wCaps += numCaptures;
		// zobrist.updateHash(newHash); // update hash

		if(transpositionTable[newHash] == 0) {
			heuristic = evaluateBoard(currBoard, isMaximizing, agentTile, wCaps, bCaps); // Heuristic
			transpositionTable[newHash] = heuristic == 0 ? 1 : heuristic;
		}
		else
			heuristic = transpositionTable[newHash];
		// cout<<children[i].first<<" "<<children[i].second<<" "<<heuristic<<endl; // DEBUG
		
		moveOrderMin.push(make_pair(heuristic, children[i]));

		agentTile == 1 ? bCaps -= numCaptures : wCaps -= numCaptures;
		zobrist.updateHash(oldHash); // update hash
	}
	return moveOrderMin;
}

// ALPHA BETA
int alphaBeta(vector<vector<int> > currBoard, int wCaps, int bCaps, int depth, int alpha, int beta, bool isMaximizing, int agentTile, ZobristHash hasher)
{
    if (depth == 0)
        return evaluateBoard(currBoard, isMaximizing, agentTile, wCaps, bCaps);

	
	vector<pii> children;
	vector<pii> range;
	tie(children, range) = getChildren(currBoard);

    if (isMaximizing)
    {
        int bestValue = -1000;
		// cout << children.size() << endl; // DEBUG

		// ?MOVE ORDERING
		priority_queue <pair<int, pii> > moveOrderMax = moveOrderingMax(children, currBoard, agentTile, depth, alpha, beta, isMaximizing, wCaps, bCaps, hasher);

        while(!moveOrderMax.empty())
        {
			int heuristic = moveOrderMax.top().first;
			pii child = moveOrderMax.top().second;
			moveOrderMax.pop();

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
                return 1000;
            // printBoard(board); // DEBUG

			hasher.updateHash(hash); // update hash for captures
            int value = alphaBeta(newBoard, wCaps, bCaps, depth - 1, alpha, beta, false, agentTile, hasher);


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
        int bestValue = 1000;
        // cout << children.size() << endl; // DEBUG

		// ?MOVE ORDERING
		priority_queue <pair<int, pii>, vector<pair<int, pii> >, Compare > moveOrderMin = moveOrderingMin(children, currBoard, agentTile, depth, alpha, beta, isMaximizing, wCaps, bCaps, hasher);

        while(!moveOrderMin.empty())
        {
			int heuristic = moveOrderMin.top().first;
			pii child = moveOrderMin.top().second;
			moveOrderMin.pop();

            currBoard[child.first][child.second] = agentTile;
			vector<vector<int> > newBoard = currBoard;
			int numCaptures;
			
			uint64_t oldHash = hasher.hash(); // get old hash
        	hasher.updateHash(child.first, child.second, agentTile); // update hash for move
			uint64_t hash;
			pair<int, uint64_t> temp;

            tie(newBoard, temp) = checkCaptures(currBoard, child.first, child.second, agentTile == 1 ? 2 : 1, hasher);
			tie(numCaptures, hash) = temp;

			// ?CHECK IF NEXT LINE IS CORRECT
            agentTile == 1 ? wCaps += numCaptures : bCaps += numCaptures;
            if(checkWin(newBoard, wCaps, bCaps, agentTile == 1 ? 2 : 1, child.first, child.second))
                return -1000;
            // printBoard(board); // DEBUG
			
			hasher.updateHash(hash); // update hash for captures
            int value = alphaBeta(newBoard, wCaps, bCaps, depth - 1, alpha, beta, true, agentTile, hasher);

            currBoard[child.first][child.second] = 0;
			agentTile == 1 ? wCaps -= numCaptures : bCaps -= numCaptures; // undo captures
			hasher.updateHash(oldHash); // update hash for undoing move

            bestValue = max(bestValue, value);
            beta = min(beta, bestValue);
            if (beta <= alpha)
                break;
        }
        return bestValue;
    }
}
