// src/App.js
import React from 'react';
import Board from './components/Board';
import './App.css';

const App = () => {
  return (
    <div className="App">
      <h1>Pente Game</h1>
      <div className="game-container">
        <Board />
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
        </div>
      </div>
    </div>
  );
};

export default App;
