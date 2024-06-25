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

  const move2coord = (move) => {
    const x = move % BOARD_SIZE;
    const y = Math.floor(move / BOARD_SIZE);
    const z = x > 7 ? String.fromCharCode(x + 66) : String.fromCharCode(x + 65);
    return [z, y + 1];
  };

  function checkCaptures(move, player, squares) {
    const opponent = player === 1 ? 2 : 1;
    const directions = [
      [-1, 0],
      [1, 0],
      [0, -1],
      [0, 1],
      [-1, -1],
      [1, 1],
      [-1, 1],
      [1, -1],
    ];
    let captures = [];

    const x = move % BOARD_SIZE;
    const y = Math.floor(move / BOARD_SIZE);

    for (let [dx, dy] of directions) {
      const nx1 = x + dx;
      const ny1 = y + dy;
      const nx2 = x + 2 * dx;
      const ny2 = y + 2 * dy;
      const nx3 = x + 3 * dx;
      const ny3 = y + 3 * dy;
      if (
        nx1 >= 0 &&
        nx1 < BOARD_SIZE &&
        ny1 >= 0 &&
        ny1 < BOARD_SIZE &&
        nx2 >= 0 &&
        nx2 < BOARD_SIZE &&
        ny2 >= 0 &&
        ny2 < BOARD_SIZE &&
        nx3 >= 0 &&
        nx3 < BOARD_SIZE &&
        ny3 >= 0 &&
        ny3 < BOARD_SIZE &&
        squares[ny1 * BOARD_SIZE + nx1] === opponent &&
        squares[ny2 * BOARD_SIZE + nx2] === opponent &&
        squares[ny3 * BOARD_SIZE + nx3] === player
      ) {
        captures.push(ny1 * BOARD_SIZE + nx1, ny2 * BOARD_SIZE + nx2);
      }
    }

    return captures;
  }

  const checkWin = (move, player, squares) => {
    if (player === 1 && blackCapture >= 10) {
      return true;
    }
    if (player === 2 && whiteCapture >= 10) {
      return true;
    }

    if (moves?.length === BOARD_SIZE * BOARD_SIZE) {
      return true;
    }

    // 5 in a row / column / diagonal
    const directions = [
      [-1, 0],
      [1, 0],
      [0, -1],
      [0, 1],
      [-1, -1],
      [1, 1],
      [-1, 1],
      [1, -1],
    ];
    const x = move % BOARD_SIZE;
    const y = Math.floor(move / BOARD_SIZE);

    for (let [dx, dy] of directions) {
      let count = 1;
      let nx = x + dx;
      let ny = y + dy;
      while (
        nx >= 0 &&
        nx < BOARD_SIZE &&
        ny >= 0 &&
        ny < BOARD_SIZE &&
        squares[ny * BOARD_SIZE + nx] === player
      ) {
        count++;
        nx += dx;
        ny += dy;
      }
      nx = x - dx;
      ny = y - dy;
      while (
        nx >= 0 &&
        nx < BOARD_SIZE &&
        ny >= 0 &&
        ny < BOARD_SIZE &&
        squares[ny * BOARD_SIZE + nx] === player
      ) {
        count++;
        nx -= dx;
        ny -= dy;
      }
      if (count >= 5) {
        return true;
      }
    }
    return false;
  };

  const handleMove = (move, player, squares) => {
    let newSquares = squares.slice();
    newSquares[move] = player;

    const coord = move2coord(move);
    setMoves((moves) => [...moves, { position: `${coord[0]}${coord[1]}`, player }]);

    const captures = checkCaptures(move, player, newSquares);
    if (captures.length > 0) {
      if (player === 1) {
        setWhiteCapture(whiteCapture + captures.length / 2);
      } else {
        setBlackCapture(blackCapture + captures.length / 2);
      }
      captures.forEach((capture) => {
        newSquares[capture] = null;
      });
    }

    setSquares(newSquares);

    if (checkWin(move, player, squares)) {
      alert(`Player ${player === 1 ? "O" : "X"} wins!`);
    }

    return newSquares;
  };

  useEffect(() => {
    if(player === 1) {
      const squares = Array(BOARD_SIZE * BOARD_SIZE).fill(null);
      handleMove(180, 2, squares);
    }
    else if(player === 2) {
      setSquares(Array(BOARD_SIZE * BOARD_SIZE).fill(null));
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [player]);

  return (
    <div className="App">
      <h1>Pente Game</h1>
      <div className="game-container">
        <Board
          handleMove={handleMove}
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
