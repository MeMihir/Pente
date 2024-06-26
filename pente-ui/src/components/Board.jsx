// src/components/Board.js
import React, { useEffect, useState } from "react";
import Square from "./Square";
import { loadWasm } from "../services/Engine";
import { oppColor } from "../util";

const BOARD_SIZE = 19;

const Board = ({
  handleMove,
  numMoves,
  playerTile,
  squares,
  setSquares,
  setMovesStack,
  whiteCapture,
  blackCapture,
}) => {
  const [isUserTurn, setIsUserTurn] = useState(true);
  const [penteEngine, setPenteEngine] = useState(null);

  useEffect(() => {
    loadWasm().then((engine) => setPenteEngine(engine));
  }, []);

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

    const { newSquares, currentTurn } = handleMove(i, playerTile, squares.slice(), []);
    setIsUserTurn(false);

    const boardPtr = penteEngine.malloc(
      BOARD_SIZE * BOARD_SIZE * Int32Array.BYTES_PER_ELEMENT
    );

    penteEngine.HEAP32.set(
      new Int32Array(newSquares),
      boardPtr / Int32Array.BYTES_PER_ELEMENT
    );

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
      oppColor(playerTile),
      Math.ceil(numMoves?.length / 2) + 1
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
      const move = handleMove(aiMove, oppColor(playerTile), newSquares, currentTurn);
      setMovesStack(movesStack => [...movesStack, move.currentTurn]);
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
          key={
            i > 7 ? String.fromCharCode(i + 66) : String.fromCharCode(i + 65)
          }
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
