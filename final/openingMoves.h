
extern unordered_map <uint64_t, string > openingMoves;
void initializeOpeningMoves();
string getOpeningMove(int nTurn, int agentTile, uint64_t hash, vector <vector <int> > board);