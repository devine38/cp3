#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SPACE	0
#define RED		1
#define BLUE	2
#define GREEN	3
#define PADDING	6
#define MAX_DEPTH 2
#define INT_MIN -2147483647
#define INT_MAX 2147483647
#define DELAY 0.5
#define MAXTIME 1
#define TROGDEEP 2.5
#define RUSHTIME 0.1
#define TESTMODE 0

// Macros
#define getPiece(r,c) board[skipPadding + r + c * rows]
#define boardSize(x) (x - PADDING)
#define setReturnVal(future,points,column,played) ((future << 10) + (points << 6) + (column << 2) + played)
#define setReturnValScore(score,future,points,column,played) ((score << 14) + (future << 10) + (points << 6) + (column << 2) + played)
#define getScore(x) ((x >> 14) & 127)
#define getFuture(x) ((x >> 10) & 15)
#define getPoints(x) ((x >> 6) & 15)
#define getColumn(x) ((x >> 2) & 15)
#define getPlayed(x) (x & 3)

// Function Prototypes

void readboard(void);
void freeboard(void);
void printboard(void);
void printWin(void);
int getTop(int column);
void addPiece(int col, int colour);
void remPiece(int col);
int burninate(int player, int depth, int origDepth);
int isWin(int lastColumn);

// Global variables
int *board;
int columns;
int rows;
int last_move;
int total_time;
int player_1_time;
int last_move_time;
int skipPadding;
int *columnHeight;
char pieces[4] = { 's', 'r', 'b', 'g' };
int almostPoints[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0, 256, 256, 0,
		0, 0, 0, 0, 1, 0, 1, 0, 0, 256, 256, 0, 1, 256, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 512, 0, 0, 512, 512, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 512, 0, 0, 0, 0, 0, 0, 0, 0, 256, 0, 0, 0, 0, 0, 512, 512, 0, 0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 512, 512, 0, 2, 512, 0, 0, 2,
		0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 512, 512, 0, 0, 0, 0, 256, 0, 0, 0, 256, 0, 0, 0, 0, 2, 512, 0, 1, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0 };
int points[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 1280, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 768, 0, 0, 1024, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0,
		3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1024, 0, 0, 768, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 1280, 0, 0, 0, 0, 0 };

/**
 * Reads in the board and other variables from standard in.
 */
void readboard(void) {
	int i, j;
	char tmpChar = 0;

	scanf("(%d,%d,%d,%d,%d,%d", &columns, &rows, &last_move, &total_time, &player_1_time, &last_move_time);
	columnHeight = (int *) calloc(sizeof(int), columns);
	rows += PADDING; // Make board bigger for padding
	columns += PADDING;
	skipPadding = 3 * rows + 3;
	board = (int *) calloc(sizeof(int), rows * columns); // Need to pad the board with spaces
	for (i = 0; i < columns; i++)
		for (j = 0; j < rows; j++) {
			if (i < 3 || i > columns - 4 || j < 3 || j > rows - 4) {
				board[i * rows + j] = SPACE;
			} else {
				scanf(",%c", &tmpChar);

				switch (tmpChar) {
				case 's':
					board[i * rows + j] = SPACE;
					break;
				case 'r':
					columnHeight[i - 3] = j - 2;
					board[i * rows + j] = RED;
					break;
				case 'b':
					columnHeight[i - 3] = j - 2;
					board[i * rows + j] = BLUE;
					break;
				case 'g':
					columnHeight[i - 3] = j - 2;
					board[i * rows + j] = GREEN;
				}
			}
		}
	if (last_move)
		last_move--; // First column is zero, not one
}

/**
 * Frees the board
 */
void freeboard(void) {
	free(board);
	free(columnHeight);
}

/**
 * Prints out the board to standard out.
 * For testing purposes.
 */
void printBoard() {
	int i, j;

	for (i = rows - 7; i >= 0; i--) {
		for (j = 0; j < boardSize(columns); j++) {
			fprintf(stderr, "%c ", pieces[getPiece(i,j)]);
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n\n");
}

int getMovePro(int rush) {
	double maxTime = MAXTIME;
	int i, start = 1;
	int lastMove;
	int bestMove;

	if (rush) {
		maxTime = RUSHTIME;
		fprintf(stderr, "Hurry up!\n");
	}

	bestMove = alphaBeta(start, start, 0, INT_MIN, INT_MAX, BLUE, last_move, 1);
	lastMove = bestMove;
	for (i = start + 1;; i += 1) {
		if (!i & 1)
			continue;
		clock_t t1, t2;
		t1 = clock();
		int move = alphaBeta(i, i, 0, INT_MIN, INT_MAX, BLUE, getColumn(lastMove), 1);
		t2 = clock();
		double time = (double) (t2 - t1) / CLOCKS_PER_SEC;

		bestMove = move;
		if (TESTMODE && i < 12)
			fprintf(stderr, "Depth %d best move is %d,%c for %d points.\nTime: %f\n", i, getColumn(move), pieces[getPlayed(move)], move >> 6, time);

		lastMove = move;

<<<<<<< HEAD
		if (time > maxTime || i > 100) {
			i += 2;
			if (time < TROGDEEP)
				i ++;
			t1 = clock();
			int move = alphaBeta(i, i, 0, 0, INT_MAX, BLUE, getColumn(lastMove), 0); // 0 for speed
			t2 = clock();
			double time = (double) (t2 - t1) / CLOCKS_PER_SEC;
			fprintf(stderr, "Time: %f\n", time);
			int score = move >> 6;
			if (score > 1000000 || score < -1000000) {
				bestMove = move;
=======
		if (i >= remaining)
			break;

		if (time > maxTime) {
			done = 1;
			for (i++;; i++) {
				t1 = clock();
				int move = alphaBeta(i, i, 0, 0, INT_MAX, BLUE, getColumn(lastMove), 0); // 0 for speed
				t2 = clock();
				time = (double) (t2 - t1) / CLOCKS_PER_SEC;
				if (TESTMODE)
					fprintf(stderr, "Depth %d best move is %d,%c for %d points.\nTime: %f\n", i, getColumn(move), pieces[getPlayed(move)], move >> 6, time);
				int score = move >> 6;
				if (score > 1000000) {
					bestMove = move;
				}
				if (score < -1000000) {
					i--;
					done = 1;
					break;
				}
				if (time > maxTime || i >= remaining)
					break;
>>>>>>> parent of fad2aeb... Fixed columnWins from being complete crap to working.
			}

			break;
		}
	}
	fprintf(stderr, "Trog-Depth %d\n\n", i);
	if (bestMove >> 6 > 1000000)
		fprintf(stderr, "TROGDOOOOOR!!!\n");
	return bestMove;
}

void wasteMovePro() {
	clock_t t1, t2;
	t1 = clock();
	double time;
	while (time < DELAY) {
		t2 = clock();
		time = (double) (t2 - t1) / CLOCKS_PER_SEC;
	}
	if (TESTMODE)
		fprintf(stderr, "Trog-Depth unknown\n\n");
}

int alphaBeta(int origDepth, int depth, int col, int a, int b, int player, int startColumn, int heuristic) {
	if (!depth) {
		if (heuristic)
			return diffWins() + 1024 * columnWins(player & 1) + isWin(col) * 1048676; //theirTurn = player&1
		return isWin(col) * 1048676;
	}
	int original = 0;
	int next, temp_i;
	if (!(origDepth - depth))
		original = 1;
	if (!original) {
		int tmp = isWin(col);
		if (tmp) {
			if (tmp > 1) {
				return 1048676 * tmp + depth;
			} else if (tmp < 0) {
				return -8389408 - depth;
			} else {
				return 0;
			}
		}
	}
	int i, j;
	if (player == BLUE) {
		int move;
		int hasMove = 0;
		for (j = 0; j <= boardSize(columns); j++) {
			i = boardSize(columns) / 2;
			if (j & 1) {
				i -= 1 + j / 2;
			} else {
				i += j / 2;
			}
			if (j == 0) {
				next = i;
				i = startColumn;
			} else {
				temp_i = i;
				i = next;
				next = temp_i;
				if (i == startColumn)
					continue;
			}
//			if (original)
//				fprintf(stderr, "doing %d\n", i);
			if (columnHeight[i] >= boardSize(rows))
				continue;
			hasMove = 1;
			addPiece(i, BLUE);
			int tmp = alphaBeta(origDepth, depth - 1, i, a, b, RED, startColumn, heuristic);
			remPiece(i);
			addPiece(i, GREEN);
			int tmp2 = alphaBeta(origDepth, depth - 1, i, a, b, RED, startColumn, heuristic);
			remPiece(i);
			if (tmp > a) {
				a = tmp;
				move = setReturnVal(0,a,i,BLUE);
			}
			if (tmp2 > a) {
				a = tmp2;
				move = setReturnVal(0,a,i,GREEN);
			}
			if (b <= a)
				break;
		}
		if (!hasMove)
			return 0;
		if (original)
			return move;
		return a;
	} else {
		int hasMove = 0;
		for (i = 0; i < boardSize(columns); i++) {
			if (columnHeight[i] >= boardSize(rows))
				continue;
			hasMove = 1;
			addPiece(i, RED);
			int tmp = alphaBeta(origDepth, depth - 1, i, a, b, BLUE, startColumn, heuristic);
			remPiece(i);
			addPiece(i, GREEN);
			int tmp2 = alphaBeta(origDepth, depth - 1, i, a, b, BLUE, startColumn, heuristic);
			remPiece(i);
			if (tmp < b)
				b = tmp;
			if (tmp2 < b)
				b = tmp2;
			if (b <= a)
				break;
		}
		if (!hasMove)
			return 0;
		return b;
	}
}

/**
 * Takes in the last column played
 * Returns an int value of points scored from the board.
 * Negative values are points for the other team.
 */
int isWin(int lastColumn) {
	int i, a, b, c, d;
	int lastRow;
	int left, right, top, bot;
	int redPoints = 0, bluePoints = 0, best = 0;
	//possible wins, values from -5 to 5
	int possible[13], winPoints;
	// Find height of last piece played
	lastRow = getTop(lastColumn) - 1;
	//printf("last piece played at (%d,%d)\n", lastColumn, lastRow);
	//printf("Total of %d columns, %d rows\n", columns, rows);
	// Find boundaries to search, row 0 is bottom row, column 0 is left column
	top = lastRow + 3;
	bot = lastRow - 3;
	left = lastColumn - 3;
	right = lastColumn + 3;

	register int p0 = getPiece(lastRow,lastColumn);

	//printf("Top: %d, Bottom: %d, Right: %d, Left: %d\n", top, bot, right, left);

	// Search vertical
	a = p0;
	b = getPiece(lastRow-1,lastColumn) << 2;
	c = getPiece(lastRow-2,lastColumn) << 4;
	d = getPiece(lastRow-3,lastColumn) << 6;
	possible[0] = points[a + b + c + d];

	// Search horizontal
	a = getPiece(lastRow,(left));
	b = getPiece(lastRow,(left + 1)) << 2;
	c = getPiece(lastRow,(left + 2)) << 4;
	d = p0 << 6;
	possible[1] = points[a + b + c + d];

	a = getPiece(lastRow,(left + 1));
	b = getPiece(lastRow,(left + 2)) << 2;
	c = p0 << 4;
	d = getPiece(lastRow,(left + 4)) << 6;
	possible[2] = points[a + b + c + d];

	a = getPiece(lastRow,(left + 2));
	b = p0 << 2;
	c = getPiece(lastRow,(left + 4)) << 4;
	d = getPiece(lastRow,(left + 5)) << 6;
	possible[3] = points[a + b + c + d];

	a = p0;
	b = getPiece(lastRow,(left + 4)) << 2;
	c = getPiece(lastRow,(left + 5)) << 4;
	d = getPiece(lastRow,(left + 6)) << 6;
	possible[4] = points[a + b + c + d];

	// Search diagonal from top left to bottom right
	a = p0;
	b = getPiece((lastRow + 1),(lastColumn - 1)) << 2;
	c = getPiece((lastRow + 2),(lastColumn - 2)) << 4;
	d = getPiece((lastRow + 3),(lastColumn - 3)) << 6;
	possible[5] = points[a + b + c + d];

	a = getPiece((lastRow - 1),(lastColumn + 1));
	b = p0 << 2;
	c = getPiece((lastRow + 1),(lastColumn - 1)) << 4;
	d = getPiece((lastRow + 2),(lastColumn - 2)) << 6;
	possible[6] = points[a + b + c + d];

	a = getPiece((lastRow - 2),(lastColumn + 2));
	b = getPiece((lastRow - 1),(lastColumn + 1)) << 2;
	c = p0 << 4;
	d = getPiece((lastRow + 1),(lastColumn - 1)) << 6;
	possible[7] = points[a + b + c + d];

	a = getPiece((lastRow - 3),(lastColumn + 3));
	b = getPiece((lastRow - 2),(lastColumn + 2)) << 2;
	c = getPiece((lastRow - 1),(lastColumn + 1)) << 4;
	d = p0 << 6;
	possible[8] = points[a + b + c + d];

	// Search diagonal from top right to bottom left
	a = p0;
	b = getPiece((lastRow + 1),(lastColumn + 1)) << 2;
	c = getPiece((lastRow + 2),(lastColumn + 2)) << 4;
	d = getPiece((lastRow + 3),(lastColumn + 3)) << 6;
	possible[9] = points[a + b + c + d];

	a = getPiece((lastRow - 1),(lastColumn - 1));
	b = p0 << 2;
	c = getPiece((lastRow + 1),(lastColumn + 1)) << 4;
	d = getPiece((lastRow + 2),(lastColumn + 2)) << 6;
	possible[10] = points[a + b + c + d];

	a = getPiece((lastRow - 2),(lastColumn - 2));
	b = getPiece((lastRow - 1),(lastColumn - 1)) << 2;
	c = p0 << 4;
	d = getPiece((lastRow + 1),(lastColumn + 1)) << 6;
	possible[11] = points[a + b + c + d];

	a = getPiece((lastRow - 3),(lastColumn - 3));
	b = getPiece((lastRow - 2),(lastColumn - 2)) << 2;
	c = getPiece((lastRow - 1),(lastColumn - 1)) << 4;
	d = p0 << 6;
	possible[12] = points[a + b + c + d];

	// Add up wins
	winPoints = possible[0] + possible[1] + possible[2] + possible[3] + possible[4] + possible[5] + possible[6] + possible[7] + possible[8] + possible[9] + possible[10]
			+ possible[11] + possible[12];

	if (winPoints) {
		redPoints = winPoints & 255;
		bluePoints = (winPoints & 65280) >> 8;
		//printf ("winpoints: %d, win & number: %d, red: %d, blue: %d", winPoints, (winPoints & 65280) >> 8, redPoints, bluePoints);
		if (redPoints > bluePoints) {
			if (redPoints <= 5)
				return 0 - redPoints;
			for (i = 0; i < 13; i++) {
				if ((possible[i] & 255) > best) {
					best = possible[i] & 255;
					if (best == 5)
						break;
				}
			}
			return 0 - best;
		} else if (redPoints < bluePoints) {
			if (bluePoints <= 5)
				return bluePoints;
			for (i = 0; i < 13; i++) {
				if ((possible[i] & 65280) >> 8 > best) {
					best = (possible[i] & 65280) >> 8;
					if (best == 5)
						break;
				}
			}
			return best;
		} else {
			return 1;
		}
	}

	return 0;
}

/**
 * Takes in the last column played
 * Returns an int value of near wins.
 * Negative values are points for the other team.
 */
int isAlmostWin(int lastColumn) {
	int a, b, c, d;
	int lastRow;
	int left, right, top, bot;
	int redPoints = 0, bluePoints = 0;
	//possible wins, values from -5 to 5
	int possible[13], winPoints;
	// Find height of last piece played
	lastRow = getTop(lastColumn) - 1;
	//printf("last piece played at (%d,%d)\n", lastColumn, lastRow);
	//printf("Total of %d columns, %d rows\n", columns, rows);
	// Find boundaries to search, row 0 is bottom row, column 0 is left column
	top = lastRow + 3;
	bot = lastRow - 3;
	left = lastColumn - 3;
	right = lastColumn + 3;

	//printf("Top: %d, Bottom: %d, Right: %d, Left: %d\n", top, bot, right, left);

	// Search vertical
	//	a = getPiece(lastRow,lastColumn);
	//	b = getPiece(lastRow-1,lastColumn) << 2;
	//	c = getPiece(lastRow-2,lastColumn) << 4;
	//	d = getPiece(lastRow-3,lastColumn) << 6;
	possible[0] = 0;

	// Search horizontal
	a = getPiece(lastRow,(left));
	b = getPiece(lastRow,(left + 1)) << 2;
	c = getPiece(lastRow,(left + 2)) << 4;
	d = getPiece(lastRow,(left + 3)) << 6;
	possible[1] = almostPoints[a + b + c + d];

	a = getPiece(lastRow,(left + 1));
	b = getPiece(lastRow,(left + 2)) << 2;
	c = getPiece(lastRow,(left + 3)) << 4;
	d = getPiece(lastRow,(left + 4)) << 6;
	possible[2] = almostPoints[a + b + c + d];

	a = getPiece(lastRow,(left + 2));
	b = getPiece(lastRow,(left + 3)) << 2;
	c = getPiece(lastRow,(left + 4)) << 4;
	d = getPiece(lastRow,(left + 5)) << 6;
	possible[3] = almostPoints[a + b + c + d];

	a = getPiece(lastRow,(left + 3));
	b = getPiece(lastRow,(left + 4)) << 2;
	c = getPiece(lastRow,(left + 5)) << 4;
	d = getPiece(lastRow,(left + 6)) << 6;
	possible[4] = almostPoints[a + b + c + d];

	// Search diagonal from top left to bottom right
	a = getPiece((lastRow + 0),(lastColumn - 0));
	b = getPiece((lastRow + 1),(lastColumn - 1)) << 2;
	c = getPiece((lastRow + 2),(lastColumn - 2)) << 4;
	d = getPiece((lastRow + 3),(lastColumn - 3)) << 6;
	possible[5] = almostPoints[a + b + c + d];

	a = getPiece((lastRow - 1),(lastColumn + 1));
	b = getPiece((lastRow + 0),(lastColumn - 0)) << 2;
	c = getPiece((lastRow + 1),(lastColumn - 1)) << 4;
	d = getPiece((lastRow + 2),(lastColumn - 2)) << 6;
	possible[6] = almostPoints[a + b + c + d];

	a = getPiece((lastRow - 2),(lastColumn + 2));
	b = getPiece((lastRow - 1),(lastColumn + 1)) << 2;
	c = getPiece((lastRow + 0),(lastColumn - 0)) << 4;
	d = getPiece((lastRow + 1),(lastColumn - 1)) << 6;
	possible[7] = almostPoints[a + b + c + d];

	a = getPiece((lastRow - 3),(lastColumn + 3));
	b = getPiece((lastRow - 2),(lastColumn + 2)) << 2;
	c = getPiece((lastRow - 1),(lastColumn + 1)) << 4;
	d = getPiece((lastRow + 0),(lastColumn - 0)) << 6;
	possible[8] = almostPoints[a + b + c + d];

	// Search diagonal from top right to bottom left
	a = getPiece((lastRow + 0),(lastColumn + 0));
	b = getPiece((lastRow + 1),(lastColumn + 1)) << 2;
	c = getPiece((lastRow + 2),(lastColumn + 2)) << 4;
	d = getPiece((lastRow + 3),(lastColumn + 3)) << 6;
	possible[9] = almostPoints[a + b + c + d];

	a = getPiece((lastRow - 1),(lastColumn - 1));
	b = getPiece((lastRow + 0),(lastColumn + 0)) << 2;
	c = getPiece((lastRow + 1),(lastColumn + 1)) << 4;
	d = getPiece((lastRow + 2),(lastColumn + 2)) << 6;
	possible[10] = almostPoints[a + b + c + d];

	a = getPiece((lastRow - 2),(lastColumn - 2));
	b = getPiece((lastRow - 1),(lastColumn - 1)) << 2;
	c = getPiece((lastRow + 0),(lastColumn + 0)) << 4;
	d = getPiece((lastRow + 1),(lastColumn + 1)) << 6;
	possible[11] = almostPoints[a + b + c + d];

	a = getPiece((lastRow - 3),(lastColumn - 3));
	b = getPiece((lastRow - 2),(lastColumn - 2)) << 2;
	c = getPiece((lastRow - 1),(lastColumn - 1)) << 4;
	d = getPiece((lastRow + 0),(lastColumn + 0)) << 6;
	possible[12] = almostPoints[a + b + c + d];

	// Add up wins
	winPoints = possible[0] + possible[1] + possible[2] + possible[3] + possible[4] + possible[5] + possible[6] + possible[7] + possible[8] + possible[9] + possible[10]
			+ possible[11] + possible[12];

	if (winPoints) {
		redPoints = winPoints & 255;
		bluePoints = (winPoints & 65280) >> 8;
		return bluePoints - 2 * redPoints;
	}

	return 0;
}

int getTop(int column) {
	int top, i;
	for (i = 0;; i++) {
		if (getPiece(i,column) == SPACE) {
			top = i;
			return top;
		}
	}
}

int diffWins() {
	int i, j;
	int points = 0;
	//H
	for (i = 0; i < boardSize(columns) - 3; i++) {
		for (j = 0; j < boardSize(rows); j++) {
			int current = 0, red = 0, blue = 0, green = 0;
			current += 1 << (getPiece(j,i) << 2);
			current += 1 << (getPiece(j,(i+1)) << 2);
			current += 1 << (getPiece(j,(i+2)) << 2);
			current += 1 << (getPiece(j,(i+3)) << 2);
			red = (current >> (RED << 2)) & 7;
			blue = (current >> (BLUE << 2)) & 7;
			green = (current >> (GREEN << 2)) & 7;
			if (!red && blue && blue < 3 && green < 3) {
				points += blue + green;
			} else if (!blue && red && red < 3 && green < 3) {
				points -= red + green;
			}
			if (!(red + blue + green))
				break;
		}
	}
	//D1
	for (i = 0; i < boardSize(columns) - 3; i++) {
		for (j = 0; j < boardSize(rows) - 3; j++) {
			int current = 0, red = 0, blue = 0, green = 0;
			current += 1 << (getPiece(j,i) << 2);
			current += 1 << (getPiece((j+1),(i+1)) << 2);
			current += 1 << (getPiece((j+2),(i+2)) << 2);
			current += 1 << (getPiece((j+3),(i+3)) << 2);
			red = (current >> (RED << 2)) & 7;
			blue = (current >> (BLUE << 2)) & 7;
			green = (current >> (GREEN << 2)) & 7;
			if (!red && blue && blue < 3 && green < 3) {
				points += blue + green;
			} else if (!blue && red && red < 3 && green < 3) {
				points -= red + green;
			}
			if (!(red + blue + green))
				break;
		}
	}
	//D2
	for (i = 0; i < boardSize(columns) - 3; i++) {
		for (j = 0; j < boardSize(rows) - 3; j++) {
			int current = 0, red = 0, blue = 0, green = 0;
			current += 1 << (getPiece((j+3),i) << 2);
			current += 1 << (getPiece((j+2),(i+1)) << 2);
			current += 1 << (getPiece((j+1),(i+2)) << 2);
			current += 1 << (getPiece(j,(i+3)) << 2);
			red = (current >> (RED << 2)) & 7;
			blue = (current >> (BLUE << 2)) & 7;
			green = (current >> (GREEN << 2)) & 7;
			if (!red && blue && blue < 3 && green < 3) {
				points += blue + green;
			} else if (!blue && red && red < 3 && green < 3) {
				points -= red + green;
			}
			if (!(red + blue + green))
				break;
		}
	}
	//V
	for (i = 0; i < boardSize(columns); i++) {
		for (j = 0; j < boardSize(rows) - 3; j++) {
			int current = 0, red = 0, blue = 0, green = 0;
			current += 1 << (getPiece((j+3),i) << 2);
			current += 1 << (getPiece((j+2),i) << 2);
			current += 1 << (getPiece((j+1),i) << 2);
			current += 1 << (getPiece(j,i) << 2);
			red = (current >> (RED << 2)) & 7;
			blue = (current >> (BLUE << 2)) & 7;
			green = (current >> (GREEN << 2)) & 7;
			if (!red && blue && blue < 3 && green < 3) {
				points += blue + green;
			} else if (!blue && red && red < 3 && green < 3) {
				points -= red + green;
			}
			//			printf("%c,%c,%c,%c\n", pieces[getPiece((j+3),i)], pieces[getPiece((j+2),i)], pieces[getPiece((j+1),i)], pieces[getPiece(j,i)]);
			//			printf("%d\n", points);
			//			printf("%d,%d,%d\n\n", red, blue, green);
			if (red + blue + green < 2)
				break;
		}
	}
	return points;
}

//Should make this function better?
int columnWins(int theirTurn) {
	int points = 0;
	int i, j;
	for (i = 0; i < boardSize(columns); i++) {
		if (columnHeight[i] >= boardSize(rows))
			continue;

		int placed = 0;
		int done = 0;
		if (theirTurn) {
			addPiece(i, RED);
			placed++;
		}
		while (columnHeight[i] < boardSize(rows)) {
			addPiece(i, GREEN);
			placed++;
			if (isWin(i) > 2) {
				points++;
				if (columnHeight[i] < boardSize(rows)) {
					addPiece(i, GREEN);
					placed++;
					if (isWin(i) > 2) {
						points += 10;
					} else {
						remPiece(i);
						addPiece(i, BLUE);
						if (isWin(i) > 2) {
							points += 10;
						}
					}
				}
				break;
			} else if (isWin(i) < -2) {
				points--;
				if (columnHeight[i] < boardSize(rows)) {
					addPiece(i, GREEN);
					placed++;
					if (isWin(i) < -2) {
						points -= 10;
					} else {
						remPiece(i);
						addPiece(i, RED);
						if (isWin(i) < -2) {
							points -= 10;

						}
					}
				}
				break;
			}
			remPiece(i);
			addPiece(i, BLUE);
			if (isWin(i) > 2) {
				points++;
				points++;
				if (columnHeight[i] < boardSize(rows)) {
					addPiece(i, GREEN);
					placed++;
					if (isWin(i) > 2) {
						points += 10;
					} else {
						remPiece(i);
						addPiece(i, BLUE);
						if (isWin(i) > 2) {
							points += 10;

						}
					}
				}
				break;
			}
			remPiece(i);
			addPiece(i, RED);
			if (isWin(i) < -2) {
				points--;
				if (columnHeight[i] < boardSize(rows)) {
					addPiece(i, GREEN);
					placed++;
					if (isWin(i) < -2) {
						points -= 10;
					} else {
						remPiece(i);
						addPiece(i, RED);
						if (isWin(i) < -2) {
							points -= 10;

						}
					}
				}
				break;
			}
			if (getPiece(columnHeight[i],i-1) == SPACE && getPiece(columnHeight[i],i+1) == SPACE)
				break;
		}
		for (j = 0; j < placed; j++) {
			remPiece(i);
		}
	}
	//printBoard();
	//fprintf(stderr, "Points: %d\n",points);
	return points;
}

void addPiece(int col, int colour) {
	getPiece(columnHeight[col],col) = colour;
	columnHeight[col]++;
}

void remPiece(int col) {
	getPiece(columnHeight[col]-1,col) = SPACE;
	columnHeight[col]--;
}

//void printWin() {
//	fprintf(stderr, "                     __               ) \n");
//	fprintf(stderr, "            _  \    | _\             Q)  / \n");
//	fprintf(stderr, "           / \  \   /  (            Q)  / \n");
//	fprintf(stderr, "          /_ |     / _/     \ /     )       /| \n");
//	fprintf(stderr, "   \      \- |     |/       .V.    _       / |______ \n");
//	fprintf(stderr, "    \      \_\_/---------_________/o\     /        / \n");
//	fprintf(stderr, "            ( |                      |   / /|__   /_____ \n");
//	fprintf(stderr, "         __/  |      _-____   /V-V-V-V    /   /     ___/ \n");
//	fprintf(stderr, "        (      |   v |     \  \^_^_^          \    <______ \n");
//	fprintf(stderr, "  ___    \  )   \    v \    \_____)     |\ |\ \    _______\ \n");
//	fprintf(stderr, "         ( / __/  \  vv  \              | \| \|    \ \n");
//	fprintf(stderr, "         /  \       \  vv  \            \       |\  \ \n");
//	fprintf(stderr, "        ( |  \ _      \      \           \  |\  | \  \ \n");
//	fprintf(stderr, "         \ \   _)        \     \          \ | \ |  \ | \n");
//	fprintf(stderr, "           \   _)         |     |          \|  \|   \| \n");
//	fprintf(stderr, "        /    \__)       |\|    | \n");
//	fprintf(stderr, "       /        ^ /\  |\|/    / \n");
//	fprintf(stderr, "                \\__\_|/    /         TROGDOR \n");
//	fprintf(stderr, "                 \_________/            the \n");
//	fprintf(stderr, "                      |  |           BURNiNATOR \n");
//	fprintf(stderr, "                      |  |____ \n");
//	fprintf(stderr, "                      |___ \n");
//}

/**
 * Calls functions to read in board etc.
 */
int main(void) {
	int col, move, i;
	char p;
	int rush = 0, totMoves = 0;

	readboard();

	if (player_1_time > 500000)
		rush = 1;

	for (i = 0; i < boardSize(columns); i++) {
		totMoves += columnHeight[i];
	}

	if (boardSize(columns) == 10 && totMoves < 20) {
		if (totMoves == 1 && getPiece(0,3) == RED) {
			col = 3;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 3 && getPiece(2,3) == RED) {
			col = 5;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 7 && getPiece(5,3) == RED && getPiece(3,3) == RED && getPiece(2,3) == RED) {
			col = 7;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 9 && getPiece(5,3) == RED && getPiece(0,4) == RED) {
			col = 4;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 13 && getPiece(5,3) == RED && getPiece(0,4) == RED && getPiece(1,2) == RED && getPiece(0,1) == GREEN) {
			col = 2;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 15 && getPiece(5,3) == RED && getPiece(0,4) == RED && getPiece(1,2) == RED && getPiece(0,1) == GREEN && getPiece(3,2) == RED) {
			col = 2;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 17 && getPiece(5,3) == RED && getPiece(0,4) == RED && getPiece(1,2) == RED && getPiece(0,1) == GREEN && getPiece(3,2) == RED && getPiece(1,6) == RED) {
			col = 6;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 0) {
			col = 2;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 2 && getPiece(0,3) == RED) {
			col = 3;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 4 && getPiece(0,3) == RED && getPiece(2,3) == RED) {
			col = 2;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 6 && getPiece(0,3) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED) {
			col = 5;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 8 && getPiece(0,3) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED && getPiece(4,3) == RED) {
			col = 5;
			p = pieces[GREEN];
			wasteMovePro();
		} else if (totMoves == 8 && getPiece(0,3) == RED && getPiece(0,4) == RED && getPiece(1,2) == RED) {
			col = 6;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 12 && getPiece(0,3) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED && getPiece(4,3) == RED && getPiece(2,2) == RED && getPiece(0,7) == RED) {
			col = 5;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 12 && getPiece(0,3) == RED && getPiece(0,4) == RED && getPiece(1,2) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED && getPiece(0,8) == RED) {
			col = 6;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 14 && getPiece(0,3) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED && getPiece(4,3) == RED && getPiece(2,2) == RED && getPiece(0,7) == RED && getPiece(3,5) == RED) {
			col = 7;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 14 && getPiece(0,3) == RED && getPiece(0,4) == RED && getPiece(1,2) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED && getPiece(0,8) == RED
				&& getPiece(3,6) == RED) {
			col = 7;
			p = pieces[BLUE];
			wasteMovePro();
		} else if (totMoves == 16 && getPiece(0,3) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED && getPiece(4,3) == RED && getPiece(2,2) == RED && getPiece(0,7) == RED && getPiece(3,5) == RED
				 && getPiece(3,2) == RED) {
			col = 6;
			p = pieces[GREEN];
			wasteMovePro();
		} else {
			move = getMovePro(rush);
			col = getColumn(move);
			p = pieces[getPlayed(move)];
		}
	} else {
		move = getMovePro(rush);
		col = getColumn(move);
		p = pieces[getPlayed(move)];
	}
	freeboard();
	printf("(%d,%c)", col + 1, p);

	return 0;
}


<<<<<<< HEAD
=======

//	if (boardSize(columns) == 10 && totMoves < 20) {
//		if (totMoves == 1 && getPiece(0,3) == RED) {
//			col = 3;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 3 && getPiece(2,3) == RED) {
//			col = 5;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 7 && getPiece(5,3) == RED && getPiece(3,3) == RED && getPiece(2,3) == RED) {
//			col = 7;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 9 && getPiece(5,3) == RED && getPiece(0,4) == RED) {
//			col = 4;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 13 && getPiece(5,3) == RED && getPiece(0,4) == RED && getPiece(1,2) == RED && getPiece(0,1) == GREEN) {
//			col = 2;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 15 && getPiece(5,3) == RED && getPiece(0,4) == RED && getPiece(1,2) == RED && getPiece(0,1) == GREEN && getPiece(3,2) == RED) {
//			col = 2;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 17 && getPiece(5,3) == RED && getPiece(0,4) == RED && getPiece(1,2) == RED && getPiece(0,1) == GREEN && getPiece(3,2) == RED && getPiece(1,6) == RED) {
//			col = 6;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 0) {
//			col = 2;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 2 && getPiece(0,3) == RED) {
//			col = 3;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 4 && getPiece(0,3) == RED && getPiece(2,3) == RED) {
//			col = 2;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 6 && getPiece(0,3) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED) {
//			col = 5;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 8 && getPiece(0,3) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED && getPiece(4,3) == RED) {
//			col = 5;
//			p = pieces[GREEN];
//			wasteMovePro();
//		} else if (totMoves == 8 && getPiece(0,3) == RED && getPiece(0,4) == RED && getPiece(1,2) == RED) {
//			col = 6;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 12 && getPiece(0,3) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED && getPiece(4,3) == RED && getPiece(2,2) == RED && getPiece(0,7) == RED) {
//			col = 5;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 12 && getPiece(0,3) == RED && getPiece(0,4) == RED && getPiece(1,2) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED && getPiece(0,8) == RED) {
//			col = 6;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 14 && getPiece(0,3) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED && getPiece(4,3) == RED && getPiece(2,2) == RED && getPiece(0,7) == RED
//				&& getPiece(3,5) == RED) {
//			col = 7;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 14 && getPiece(0,3) == RED && getPiece(0,4) == RED && getPiece(1,2) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED && getPiece(0,8) == RED
//				&& getPiece(3,6) == RED) {
//			col = 7;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else if (totMoves == 16 && getPiece(0,3) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED && getPiece(4,3) == RED && getPiece(2,2) == RED && getPiece(0,7) == RED
//				&& getPiece(3,5) == RED && getPiece(3,2) == RED) {
//			col = 6;
//			p = pieces[GREEN];
//			wasteMovePro();
//		} else if (totMoves == 18 && getPiece(0,3) == RED && getPiece(2,3) == RED && getPiece(3,3) == RED && getPiece(4,3) == RED && getPiece(2,2) == RED && getPiece(0,7) == RED
//				&& getPiece(3,5) == RED && getPiece(3,2) == RED && getPiece(4,2) == RED) {
//			col = 8;
//			p = pieces[BLUE];
//			wasteMovePro();
//		} else {
//			move = getMovePro(rush);
//			col = getColumn(move);
//			p = pieces[getPlayed(move)];
//		}
//	} else {
//	}
>>>>>>> parent of fad2aeb... Fixed columnWins from being complete crap to working.
/* else if (totMoves == 3 && getPiece(0,3) == RED && getPiece(1,3) == RED) {
 col = 7;
 p = pieces[BLUE];
 wasteMovePro();
 } else if (totMoves == 5 && getPiece(0,3) == RED && getPiece(1,3) == RED && getPiece(1,4) == RED) {
 col = 4;
 p = pieces[BLUE];
 wasteMovePro();
 } else if (totMoves == 7 && getPiece(0,3) == RED && getPiece(1,3) == RED && getPiece(1,4) == RED && getPiece(3,4) == RED) {
 col = 4;
 p = pieces[BLUE];
 wasteMovePro();
 } else if (totMoves == 9 && getPiece(0,3) == RED && getPiece(1,3) == RED && getPiece(1,4) == RED && getPiece(3,4) == RED && getPiece(5,4) == RED) {
 col = 6;
 p = pieces[GREEN];
 wasteMovePro();
 } else if (totMoves == 11 && getPiece(0,3) == RED && getPiece(1,3) == RED && getPiece(1,4) == RED && getPiece(3,4) == RED && getPiece(5,4) == RED && getPiece(5,4) == RED) {
 col = 7;
 p = pieces[BLUE];
 wasteMovePro();
 }*/
