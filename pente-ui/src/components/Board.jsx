// src/components/Board.js
import React, { useEffect, useState } from "react";
import Square from "./Square";
import { randomEngine, loadWasm } from "../services/Engine";

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

  const handleClick = async (i) => {
    if (squares[i] || !isUserTurn || !penteEngine) return;

    const newSquares = squares.slice();
    newSquares[i] = 2; // User's move
    setSquares(newSquares);
    setIsUserTurn(false);
    let coord = move2coord(i);
    onMove({ position: `${coord[0]}${coord[1]}`, player: "X" });

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

    console.log("newSquares", newSquares);

    console.log(numMoves?.length)

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

    console.log(aiMove);
    coord = move2coord(aiMove);
    console.log("aiMove", aiMove, coord);
    onMove({ position: `${coord[0]}${coord[1]}`, player: "X" });

    penteEngine.free(boardPtr);

    if (aiMove !== -1) {
      newSquares[aiMove] = 1; // AI's move
      setSquares(newSquares);
    }

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
          key={i}
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
      row.push(<Square value={null} key={i} header={i + 1} />);
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
