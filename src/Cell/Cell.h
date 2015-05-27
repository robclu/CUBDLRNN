/*
 *  Cell header file for CUBDLRNN that defines a LSTM cell.
 *
 *  Copyright (C) 2015 Rob Clucas robclu1818@gmail.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT AN_size.y WARRANTY; without even the implied warranty of
 *  MERCHANTABILIT_size.y or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  _size.you should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation,
 *	Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef CUBDLRNN_CELL_INCLUDED
#define CUBDLRNN_CELL_INCLUDED

namespace cubdlrnn {
	/*
	 * ============================================================================
	 * Class        : Cell
	 *
	 * Description  : Cell class that defines a Long Short-Term Memory cell for
	 *                the deep bi-directional recurrent neural network.
	 *
	 * Params (T)   : The precision of the computation (Double, Float etc ...)
	 * ============================================================================
	 */
	template <class Precision>
	class Cell {
		public:
			Precision input;         // Input value to the cell (if the actual input should be used)
			Precision output;        // Output value of the cell (if the computed output should be used)
			Precision forget;        // Forget value of the cell (if the cell must forget its state)
			Precision state_t;       // Current state of the cell
			Precision state_t1;      // Previous state of the cell
	};	
}           // End namespace CUBDLRNN
#endif      // CUBDLRNN_CELL_INCLUDED