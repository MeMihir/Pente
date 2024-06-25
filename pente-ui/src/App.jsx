// src/App.js
import React, { useEffect, useState } from "react";
import Board from "./components/Board";
import "./App.css";
import MovesTable from "./components/MovesTable";
import { oppColor, playerColor } from "./util";

const BOARD_SIZE = 19;

const App = () => {
  const [squares, setSquares] = useState(
    Array(BOARD_SIZE * BOARD_SIZE).fill(null)
  );
  const [moves, setMoves] = useState([]);
  const [player, setPlayer] = useState(2);
  const [whiteCapture, setWhiteCapture] = useState(0);
  const [blackCapture, setBlackCapture] = useState(0);

  const handleMove = (move) => {
    setMoves((moves) => [...moves, move]);
  };

  useEffect(() => {
    player === 1 &&
      setMoves([
        {
          position: "K10",
          player: "X",
        },
      ]);
  }, [player]);

  return (
    <div className="App">
      <h1>Pente Game</h1>
      <div className="game-container">
        <Board
          onMove={handleMove}
          numMoves={moves}
          playerTile={player}
          squares={squares}
          setSquares={setSquares}
          whiteCapture={whiteCapture}
          blackCapture={blackCapture}
          setWhiteCapture={setWhiteCapture}
          setBlackCapture={setBlackCapture}
        />
        <div className="game-info">
          <div>
            <span>Player: X</span>
            <span>AI: O</span>
          </div>
          <button>Restart</button>
          <button>Undo</button>
          <button>Change Game</button>
          <button onClick={() => setPlayer(oppColor(player))}>
            Play as {playerColor(oppColor(player))}
          </button>
          <MovesTable moves={moves} />
        </div>
      </div>
    </div>
  );
};

export default App;
