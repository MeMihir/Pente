// src/components/MovesTable.js
import React from 'react';

const MovesTable = ({ moves }) => {

	const getRows = () => {
		let rows = [];
		for (let i = 0; i < moves.length; i += 2) {
			let whiteMove = moves[i];
			let blackMove = moves[i + 1];
			rows.push(
				<tr key={i / 2}>
					<td>{i / 2 + 1}</td>
					<td>{whiteMove ? whiteMove.position : ''}</td>
					<td>{blackMove ? blackMove.position : ''}</td>
				</tr>
			);
		}
		return rows;
	};
    
  return (
    <div className="moves-table">
      <h2>Moves</h2>
      <table>
        <thead>
          <tr>
            <th>#</th>
            <th>White</th>
            <th>Black</th>
          </tr>
        </thead>
        <tbody>
          {getRows()}
        </tbody>
      </table>
    </div>
  );
};

export default MovesTable;
