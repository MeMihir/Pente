#define pii pair<int, int>


class Compare
{
public:
    bool operator()(pair<int, pii> a, pair<int, pii> b);
};

pair<vector<vector<int> >, pair<int, uint64_t> > checkCaptures(vector<vector<int> > currBoard, int row, int col, int color, ZobristHash hasher);
bool checkWin(vector<vector<int> > currBoard, int wCaps, int bCaps, int color, int row, int col);
pair<vector<pii>, vector<pii> > getChildren(vector<vector<int> > currBoard);
priority_queue <pair<int, pii> > moveOrderingMax(vector<pii> children, vector<vector<int> > board, int agentTile, bool isMaximizing, int wCaps, int bCaps, ZobristHash zobrist, vector<pii> range);
priority_queue <pair<int, pii>, vector<pair<int, pii> >, Compare > moveOrderingMin(vector<pii> children, vector<vector<int> > board, int agentTile, bool isMaximizing, int wCaps, int bCaps, ZobristHash zobrist, vector<pii> range);
long long int alphaBeta(vector<vector<int> > currBoard, int wCaps, int bCaps, int depth, long long int alpha, long long int beta, bool isMaximizing, int agentTile, ZobristHash hasher, float ForwardPruningPercentage, int MinHeuristic);
