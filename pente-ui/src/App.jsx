// src/App.js
import React, { useEffect, useState } from "react";
import Board from "./components/Board";
import "./App.css";
import MovesTable from "./components/MovesTable";
import { oppColor, playerColor } from "./util";

const App = () => {
  const [moves, setMoves] = useState([]);
  const [player, setPlayer] = useState(2);

  const handleMove = (move) => {
    setMoves((moves) => [...moves, move]);
  };

  useEffect(() => {
    player === 1 && setMoves([{
      position: "K10",
      player: "X",
    }]);
  }, [player])

  return (
    <div className="App">
      <h1>Pente Game</h1>
      <div className="game-container">
        <Board onMove={handleMove} numMoves={moves} playerTile={player} />
        <div className="game-info">
          <h2>Captures:</h2>
          <div>
            <span>Player: X</span>
            <span>AI: O</span>
          </div>
          <button>Play</button>
          <button>Undo</button>
          <button>Change Game</button>
          <button onClick={() => setPlayer(oppColor(player))} >
            Play as {playerColor(oppColor(player))}
          </button>
          <button>Use Opening Book</button>
          <MovesTable moves={moves} />
        </div>
      </div>
    </div>
  );
};

export default App;
