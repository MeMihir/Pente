// src/services/penteEngine.js
export const penteEngine = (squares) => {
    // Example AI: randomly picks an empty square
    const emptySquares = squares.map((square, index) => square === null ? index : null).filter(val => val !== null);
    const randomMove = emptySquares[Math.floor(Math.random() * emptySquares.length)];
    return randomMove;
  };
  