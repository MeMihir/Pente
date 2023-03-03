pair<vector<vector<int> >, int> checkCaptures(vector<vector<int> > currBoard, int row, int col, int color);
bool checkWin(vector<vector<int> > currBoard, int wCaps, int bCaps, int color, int row, int col);
vector<pair<int, int> > getChildren(vector<vector<int> > currBoard);
int alphaBeta(vector<vector<int> > currBoard, int wCaps, int bCaps, int depth, int alpha, int beta, bool isMaximizing, int agentTile);
