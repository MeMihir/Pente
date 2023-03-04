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

    int heusristicOpen4s;
    unordered_map<int, int> heuristicWindow4W;
    unordered_map<int, int> heuristicWindow5W;
    unordered_map<int, int> heuristicWindow4B;
    unordered_map<int, int> heuristicWindow5B;

public:
    slidingHeuristic(vector<vector<int> > board, int tile, int whiteCaptures, int blackCaptures);
    ~slidingHeuristic();
    void initializeHeuristicMaps();
    vector<int> hashWindow4(int i, int j);
    vector<int> hashWindow5(int i, int j);
    int checkOpen4s(int i, int j, int tile);
    int slidingWindowHeuristicFull();
};