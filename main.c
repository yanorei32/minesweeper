#include <stdio.h>
#include <stdlib.h>

#define ROW_SIZE 16
#define COL_SIZE 16
#define BOOM_DIVIDE_BY_N 10

#define CELL_BOMB -1
#define CELL_EMPTY 0

#define CALL_FUNC_FOR_NEAR_CELLS( FUNC, COL, ROW ) \
	FUNC ( COL    , ROW + 1 ); \
	FUNC ( COL    , ROW - 1 ); \
	FUNC ( COL + 1, ROW     ); \
	FUNC ( COL - 1, ROW     ); \
	FUNC ( COL + 1, ROW + 1 ); \
	FUNC ( COL - 1, ROW - 1 ); \
	FUNC ( COL + 1, ROW - 1 ); \
	FUNC ( COL - 1, ROW + 1 );

#define IS_VALID_POS( COL, ROW ) \
	(0 <= COL && 0 <= ROW && COL < COL_SIZE && ROW < ROW_SIZE)

struct CellStatus {
	/*	 -1: CELL_BOMB
		  0: CELL_EMPTY
		1-8: Bomb count */
	signed char BombCount;
	char IsOpen;
} typedef CellStatus_t;

union Board {
	CellStatus_t cells[ROW_SIZE][COL_SIZE];
	CellStatus_t cells_flatten[ROW_SIZE*COL_SIZE];
} typedef Board_t;

Board_t board;
int remainCells = ROW_SIZE*COL_SIZE;

a2i4input(c) 
	char c;
{
	return c < 'a' ? c - '0' : c - 'a' + 10;
}

char i2a4draw(i) {
	return i < 10 ? '0' + i : 'a' + i - 10;
}

void drawCells() {
	int col, row;

	putchar(' ');
	for (col = 0; col < COL_SIZE; ++col) {
		putchar(' ');
		putchar(i2a4draw(col));
	}
	putchar('\n');

	for (row = 0; row < ROW_SIZE; ++row) {
		putchar(i2a4draw(row));
		putchar('|');
		for (col = 0; col < COL_SIZE; ++col) {
			if (!board.cells[row][col].IsOpen) {
				putchar('.');
			} else {
				switch (board.cells[row][col].BombCount) {
					case CELL_BOMB:
						putchar('*');
						break;
					case CELL_EMPTY:
						putchar(' ');
						break;
					default:
						putchar(i2a4draw(board.cells[row][col].BombCount));
				}
			}
			putchar('|');
		}
		putchar('\n');
	}
}

void safeBombAdd(col, row) {
	if (!IS_VALID_POS(col, row)) return;
	if (board.cells[row][col].BombCount == CELL_BOMB) return;
	++board.cells[row][col].BombCount;
}

void setMines(divideBy) {
	int col, row;

	for (row = 0; row < ROW_SIZE; ++row)
		for (col = 0; col < COL_SIZE; ++col) {
			if (rand() % divideBy != 0) continue;
			board.cells[row][col].BombCount = CELL_BOMB;
			CALL_FUNC_FOR_NEAR_CELLS(safeBombAdd, col, row)
			--remainCells;
		}
}

void recursiveOpen(col, row) {
	if (!IS_VALID_POS(col, row)) return;
	if (board.cells[row][col].IsOpen) return;
	if (board.cells[row][col].BombCount == CELL_BOMB) return;

	board.cells[row][col].IsOpen = 1;
	--remainCells;
	
	if (board.cells[row][col].BombCount != CELL_EMPTY) return;
	CALL_FUNC_FOR_NEAR_CELLS(recursiveOpen, col, row)
}

main() {
	int row, col;

	/* NOTE: ASLR (Address space layout randomization) trick.
	         Looks like a random number. */
	srand((unsigned long)&row);

	for (row = 0; row < COL_SIZE*ROW_SIZE; ++row)
		board.cells_flatten[row].IsOpen ^= board.cells_flatten[row].IsOpen;

	setMines(BOOM_DIVIDE_BY_N);

	while (1) {
		drawCells();

		printf("COL: ");
		col = a2i4input(getchar());
		getchar(); /* NOTE: skip EOL */

		printf("ROW: ");
		row = a2i4input(getchar());
		getchar(); /* NOTE: skip EOL */

		recursiveOpen(col, row);

		if (board.cells[row][col].BombCount == CELL_BOMB) {
			puts("BOOOOOMB!!!!");
			break;
		}

		if (!remainCells) {
			puts("CONGRATULATIONS!!!!");
			break;
		}
	}

	for (row = 0; row < COL_SIZE*ROW_SIZE; ++row)
		board.cells_flatten[row].IsOpen = 1;

	drawCells();

	return EXIT_SUCCESS;
}
