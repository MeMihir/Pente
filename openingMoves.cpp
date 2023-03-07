#include <iostream>
#include <unordered_map>
#include <utility>
#include <string>
#include <vector>

using namespace std;
#include "openingMoves.h"
#include "helpers.h"
unordered_map <uint64_t, string > openingMoves;

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
