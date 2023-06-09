#include <vector>
#include <unordered_map>

int randomHeuristic();
int centralHeuristic(vector<vector<int> > board, int tile);
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
    ~slidingHeuristic();
    void initializeHeuristicMaps();
    vector<long long int> hashWindow4(int i, int j);
    vector<long long int> hashWindow5(int i, int j);
    long long int checkOpen4s(int i, int j, int tile);
    long long int slidingWindowHeuristicFull();
    pair<int, int> toWin3(int i, int j);
    int slidingWindowHeuristicPartial(vector<pair<int, int> >range);
};