#include <vector>
#include <unordered_map>

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