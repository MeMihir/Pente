// src/components/Board.js
import React, { useState } from "react";
import Square from "./Square";
import { penteEngine } from "../services/randomEngine";

const Board = () => {
  const [squares, setSquares] = useState(Array(19 * 19).fill(null));
  const [isUserTurn, setIsUserTurn] = useState(true);

  const handleClick = (i) => {
    if (squares[i] || !isUserTurn) return;

    const newSquares = squares.slice();
    newSquares[i] = "X";
    setSquares(newSquares);
    setIsUserTurn(false);

    const aiMove = penteEngine(newSquares);
    newSquares[aiMove] = "O";
    setSquares(newSquares);
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
    for (let i = 0; i < 19; i++) {
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

    for (let i = 0; i < 19; i++) {
      let row = [];
      // Add row index
      row.push(<Square value={null} key={i} header={i + 1} />);
      for (let j = 0; j < 19; j++) {
        row.push(renderSquare(i * 19 + j));
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
