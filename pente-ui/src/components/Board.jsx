// src/components/Board.js
import React, { useEffect, useState } from "react";
import Square from "./Square";
import { loadWasm } from "../services/Engine";

const BOARD_SIZE = 19;

const Board = ({ onMove, numMoves }) => {
  const [squares, setSquares] = useState(
    Array(BOARD_SIZE * BOARD_SIZE).fill(null)
  );
  const [isUserTurn, setIsUserTurn] = useState(true);
  const [penteEngine, setPenteEngine] = useState(null);
  const [whiteCapture, setWhiteCapture] = useState(0);
  const [blackCapture, setBlackCapture] = useState(0);

  useEffect(() => {
    loadWasm().then((engine) => setPenteEngine(engine));
  }, []);

  const move2coord = (move) => {
    const x = move % BOARD_SIZE;
    const y = Math.floor(move / BOARD_SIZE);
    const z = x > 7 ? String.fromCharCode(x + 66) : String.fromCharCode(x + 65);
    return [z, y + 1];
  };

  function checkCaptures(move, player, squares) {
    const opponent = player === 1 ? 2 : 1;
    const directions = [[-1, 0], [1, 0], [0, -1], [0, 1], [-1, -1], [1, 1], [-1, 1], [1, -1]];
    let captures = [];

    const x = move % BOARD_SIZE;
    const y = Math.floor(move / BOARD_SIZE);

    for (let [dx, dy] of directions) {
        const nx1 = x + dx;
        const ny1 = y + dy;
        const nx2 = x + 2*dx;
        const ny2 = y + 2*dy;
        const nx3 = x + 3*dx;
        const ny3 = y + 3*dy;
        if (
            nx1 >= 0 && nx1 < BOARD_SIZE && ny1 >= 0 && ny1 < BOARD_SIZE &&
            nx2 >= 0 && nx2 < BOARD_SIZE && ny2 >= 0 && ny2 < BOARD_SIZE &&
            nx3 >= 0 && nx3 < BOARD_SIZE && ny3 >= 0 && ny3 < BOARD_SIZE &&
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

    if (numMoves?.length === BOARD_SIZE * BOARD_SIZE) {
      return true;
    }

    // 5 in a row / column / diagonal
    const directions = [[-1, 0], [1, 0], [0, -1], [0, 1], [-1, -1], [1, 1], [-1, 1], [1, -1]];
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
    const coord = move2coord(move);
    onMove({ position: `${coord[0]}${coord[1]}`, player });
    const captures = checkCaptures(move, player, squares);

    if (captures.length > 0) {
      if (player === 1) {
        setWhiteCapture(whiteCapture + captures.length / 2);
      } else {
        setBlackCapture(blackCapture + captures.length / 2);
      }
      captures.forEach((capture) => {
        squares[capture] = null;
      });
      setSquares(squares);
    }

    if (checkWin(move, player, squares)) {
      alert(`Player ${player === 1 ? "O" : "X"} wins!`);
    }

    return squares;
  }

  // const printBoard = () => {
  //   let board = [];
  //   for (let i = 0; i < BOARD_SIZE; i++) {
  //     let row = [];
  //     for (let j = 0; j < BOARD_SIZE; j++) {
  //       row.push(squares[i * BOARD_SIZE + j] === null ? 0 : squares[i * BOARD_SIZE + j]);
  //     }
  //     board.push(row);
  //   }
  //   console.log(board);
  // };


  // useEffect(() => {
  //   printBoard();
  // }, [squares]);

  const handleClick = async (i) => {
    if (squares[i] || !isUserTurn || !penteEngine) return;

    let newSquares = squares.slice();
    newSquares[i] = 2; // User's move
    setSquares(newSquares);
    setIsUserTurn(false);
    newSquares = handleMove(i, 2, newSquares);

    const boardPtr = penteEngine.malloc(
      BOARD_SIZE * BOARD_SIZE * Int32Array.BYTES_PER_ELEMENT
    );

    penteEngine.HEAP32.set(new Int32Array(newSquares), boardPtr / Int32Array.BYTES_PER_ELEMENT);
    
    // let testBoard = [];
    // for (let i = 0; i < 19; i++) {
    //   const row = [];
    //   for (let j = 0; j < 19; j++) {
    //     const value = penteEngine.HEAP32[(boardPtr / Int32Array.BYTES_PER_ELEMENT) + (i * 19 + j)];
    //     row.push(value);
    //   }
    //   testBoard.push(row);
    // }
    // console.log(testBoard);

    // console.log("newSquares", newSquares);

    // console.log(numMoves?.length)

    const aiMove = await penteEngine.getNextMove(
      boardPtr,
      whiteCapture,
      blackCapture,
      300,
      1,
      Math.ceil(numMoves?.length/2)+1
    );

    // testBoard = [];
    // for (let i = 0; i < 19; i++) {
    //   const row = [];
    //   for (let j = 0; j < 19; j++) {
    //     const value = penteEngine.HEAP32[(boardPtr / Int32Array.BYTES_PER_ELEMENT) + (i * 19 + j)];
    //     row.push(value);
    //   }
    //   testBoard.push(row);
    // }
    // console.log(testBoard);

    // console.log(aiMove);

    penteEngine.free(boardPtr);

    if (aiMove !== -1) {
      newSquares[aiMove] = 1; // AI's move
      setSquares(newSquares);
    }
    handleMove(aiMove, 1, newSquares);

    setIsUserTurn(true);
  };

  const renderSquare = (i) => {
    return (
      <Square
        key={i}
        value={squares[i] && (squares[i] === 1 ? "X" : "O")}
        onClick={() => handleClick(i)}
      />
    );
  };

  const renderBoard = () => {
    let board = [];
    // Add column indexes
    let header = [<div key="top-left" className="board-cell"></div>]; // Empty top-left cell
    header.push(<Square value={null} key={"x"} header={"X"} />);
    for (let i = 0; i < BOARD_SIZE; i++) {
      header.push(
        <Square
          value={null}
          key={i > 7 ? String.fromCharCode(i + 66) : String.fromCharCode(i + 65)}
          header={
            i > 7 ? String.fromCharCode(i + 66) : String.fromCharCode(i + 65)
          }
        />
      );
    }
    board.push(
      <div key="header" className="board-row">
        {header}
      </div>
    );

    for (let i = 0; i < BOARD_SIZE; i++) {
      let row = [];
      // Add row index
      row.push(<Square value={null} key={-1} header={i + 1} />);
      for (let j = 0; j < BOARD_SIZE; j++) {
        row.push(renderSquare(i * BOARD_SIZE + j));
      }
      board.push(
        <div key={i} className="board-row">
          {row}
        </div>
      );
    }
    return board;
  };

  return <div className="board-container">{renderBoard()}</div>;
};

export default Board;
