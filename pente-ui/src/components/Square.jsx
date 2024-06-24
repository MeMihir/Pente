// src/components/Square.js
import React from "react";

const Square = ({ value, onClick, header }) => {
  return (
    <button
      className={`square ${(!value && !header) && "plus"} ${value}`}
      onClick={onClick}
    >
      {header}
    </button>
  );
};

export default Square;
