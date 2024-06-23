// src/App.js
import React, { useState } from "react";
import Board from "./components/Board";
import "./App.css";
import MovesTable from "./components/MovesTable";

const App = () => {
  const [moves, setMoves] = useState([]);

  const handleMove = (move) => {
    setMoves((moves) => [...moves, move]);
  };

  return (
    <div className="App">
      <h1>Pente Game</h1>
      <div className="game-container">
        <Board onMove={handleMove} numMoves={moves} />
        <div className="game-info">
          <h2>Captures:</h2>
          <div>
            <span>Player: X</span>
            <span>AI: O</span>
          </div>
          <button>Play</button>
          <button>Undo</button>
          <button>Change Game</button>
          <button>Play as White</button>
          <button>Use Opening Book</button>
          <MovesTable moves={moves} />
        </div>
      </div>
    </div>
  );
};

export default App;
