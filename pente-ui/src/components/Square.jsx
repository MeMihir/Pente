// src/components/Square.js
import React from "react";

const Square = ({ value, onClick }) => {
  return (
    <button
      className={`square ${!value && "plus"} ${value}`}
      onClick={onClick}
    ></button>
  );
};

export default Square;
