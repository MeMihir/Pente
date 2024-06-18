// src/components/Board.js
import React, { useEffect, useState } from "react";
import Square from "./Square";
import { randomEngine, loadWasm } from "../services/Engine";

const BOARD_SIZE = 19;

const Board = ({ onMove }) => {
  const [squares, setSquares] = useState(Array(BOARD_SIZE * BOARD_SIZE).fill(null));
  const [isUserTurn, setIsUserTurn] = useState(true);
  const [penteEngine, setPenteEngine] = useState(null);

  useEffect(() => {
    loadWasm().then((engine) => setPenteEngine(engine));
  }, []);

  
  const handleClick = async (i) => {
    if (squares[i] || !isUserTurn || !penteEngine) return;

    const newSquares = squares.slice();
    newSquares[i] = 1; // User's move
    setSquares(newSquares);
    setIsUserTurn(false);

    const boardPtr = penteEngine.malloc(newSquares.length * Int32Array.BYTES_PER_ELEMENT);
    penteEngine.HEAP32.set(new Int32Array(newSquares), boardPtr / Int32Array.BYTES_PER_ELEMENT);

    const aiMove = await penteEngine.getNextMove(boardPtr);
    // const aiMove = randomEngine(newSquares);

    penteEngine.free(boardPtr);

    if (aiMove !== -1) {
      newSquares[aiMove] = 2; // AI's move
      setSquares(newSquares);
    }

    setIsUserTurn(true);
  };

  const renderSquare = (i) => {
    return <Square key={i} value={squares[i]} onClick={() => handleClick(i)} />;
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
