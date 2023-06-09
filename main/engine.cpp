#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <utility>
using namespace std;

class engine
{
private:
	bool agentTile; // 0 = black, 1 = white
	float whiteTime;
	float blackTime;
	int whiteCaptures;
	int blackCaptures;
	vector<vector<int> > board; // 1 = black, 2 = white, 0 = empty
public:
	engine(bool agentTile, float timeLimit);
	~engine();
	void playGame();
	string generatePenteBoard();
	void makeMove(int pos1, int pos2, int currrntTurn);
	void check_capture(int row, int col, int color);
	bool check_win(int color, int row, int col);
	pair<int, int> placeRandomPiece();
};

engine::engine(bool agentTile, float timeLimit)
{
	this->agentTile = !agentTile;
	this->whiteTime = timeLimit;
	this->blackTime = timeLimit;
	this->whiteCaptures = 0;
	this->blackCaptures = 0;
	this->board = vector<vector<int> >(19, vector<int>(19, 0));
}

engine::~engine()
{
}

void engine::playGame()
{
	int currentTurn = 1;
	while (true)
	{
		if (currentTurn % 2 != agentTile)
		{
			int pos1, pos2;
			cout << generatePenteBoard() << endl;
			cout << "Your Turn\n:	";
			cin >> pos1 >> pos2;
			makeMove(pos1, pos2, currentTurn % 2);
		}
		else
		{
			pair<int, int> move = placeRandomPiece();
			makeMove(move.first, move.second, currentTurn % 2);
		}
		currentTurn++;
	}
}

// make a move
void engine::makeMove(int pos1, int pos2, int currentTurn)
{
	int currColor = currentTurn + 1;
	if (board[pos1][pos2] != 0)
		return;
	board[pos1][pos2] = currColor;
	check_capture(pos1, pos2, currColor);
	if (check_win(currColor, pos1, pos2))
	{
		string winner = currColor == 1 ? "Black" : "White";
		cout << winner << " Wins!\n";
		exit(0);
	}
}

// check for captures
void engine::check_capture(int row, int col, int color)
{
	int opponent = (color == 1) ? 2 : 1; // determine the opponent's color

	// check for captures horizontally
	if (col > 2 && board[row][col - 1] == opponent && board[row][col - 2] == opponent && board[row][col - 3] == color)
	{
		board[row][col - 1] = 0;
		board[row][col - 2] = 0;
		color == 1 ? blackCaptures++ : whiteCaptures++;
	}
	if (col < 16 && board[row][col + 1] == opponent && board[row][col + 2] == opponent && board[row][col + 3] == color)
	{
		board[row][col + 1] = 0;
		board[row][col + 2] = 0;
		color == 1 ? blackCaptures++ : whiteCaptures++;
	}

	// check for captures vertically
	if (row > 2 && board[row - 1][col] == opponent && board[row - 2][col] == opponent && board[row - 3][col] == color)
	{
		board[row - 1][col] = 0;
		board[row - 2][col] = 0;
		color == 1 ? blackCaptures++ : whiteCaptures++;
	}
	if (row < 16 && board[row + 1][col] == opponent && board[row + 2][col] == opponent && board[row + 3][col] == color)
	{
		board[row + 1][col] = 0;
		board[row + 2][col] = 0;
		color == 1 ? blackCaptures++ : whiteCaptures++;
	}

	// check for captures diagonally
	if (row > 2 && col > 2 && board[row - 1][col - 1] == opponent && board[row - 2][col - 2] == opponent && board[row - 3][col - 3] == color)
	{
		board[row - 1][col - 1] = 0;
		board[row - 2][col - 2] = 0;
		color == 1 ? blackCaptures++ : whiteCaptures++;
	}
	if (row < 16 && col < 16 && board[row + 1][col + 1] == opponent && board[row + 2][col + 2] == opponent && board[row + 3][col + 3] == color)
	{
		board[row + 1][col + 1] = 0;
		board[row + 2][col + 2] = 0;
		color == 1 ? blackCaptures++ : whiteCaptures++;
	}
	if (row > 2 && col < 16 && board[row - 1][col + 1] == opponent && board[row - 2][col + 2] == opponent && board[row - 3][col + 3] == color)
	{
		board[row - 1][col + 1] = 0;
		board[row - 2][col + 2] = 0;
		color == 1 ? blackCaptures++ : whiteCaptures++;
	}
	if (row < 16 && col > 2 && board[row + 1][col - 1] == opponent && board[row + 2][col - 2] == opponent && board[row + 3][col - 3] == color)
	{
		board[row + 2][col - 2] = 0;
		board[row + 3][col - 3] = 0;
		color == 1 ? blackCaptures++ : whiteCaptures++;
	}
}

// check for a win
bool engine::check_win(int color, int row, int col)
{

	// check for capture wins
	if (whiteCaptures == 5 || blackCaptures == 5)
		return true;

	// check for horizontal wins
	int count = 0;
	for (int c = max(0, col - 4); c <= min(18, col + 4); c++)
	{
		if (board[row][c] == color)
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
		if (board[r][col] == color)
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
		if (board[r][c] == color)
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
		if (board[r][c] == color)
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

// Places a random piece on the board
pair<int, int> engine::placeRandomPiece()
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distrib(0, 18);
	int row, col;
	do
	{
		row = distrib(gen);
		col = distrib(gen);
	} while (board[row][col] != 0);
	return make_pair(row, col);
}

// Generates a string representation of the board
string engine::generatePenteBoard()
{
	string output = "";
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (board[i][j] == 0)
			{
				output += ". ";
			}
			else if (board[i][j] == 1)
			{
				output += "b ";
			}
			else if (board[i][j] == 2)
			{
				output += "w ";
			}
		}
		output += "\n";
	}
	return output;
}

int main()
{
	bool firstPlayer;
	float timeLimit;
	cout << "Your Choice (B=0/W=1)\t: ";
	cin >> firstPlayer;
	cout << "Time limit (in seconds)\t: ";
	cin >> timeLimit;

	engine game(firstPlayer, timeLimit);
	game.playGame();

	return 0;
}