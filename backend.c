
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "backend.h"

CELL *
create_board () {
    CELL *board = (CELL *) malloc(sizeof(CELL) * BOARD_SIZE * BOARD_SIZE);

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            CELL temp = (CELL) malloc(sizeof(unsigned int));
            *temp = 0;

            board[(BOARD_SIZE * y) + x] = temp;
        }
    }

    return board;
}


void
destroy_board (CELL *board) {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[(BOARD_SIZE * y) + x] != NULL) {
                free(board[(BOARD_SIZE * y) + x]);
            } 
        }
    }

    free(board);
}


void
retrieve_line (CELL *board, line_type_t line, unsigned int nth_line, CELL *ret) {
    switch (line) {
        case ROW:
            for (int i = 0; i < BOARD_SIZE; i++) {
                *(ret + i) = board[(BOARD_SIZE * nth_line) + i];
            }
            break;

        case COLUMN:
            for (int i = 0; i < BOARD_SIZE; i++) {
                *(ret + i) = board[nth_line + (BOARD_SIZE * i)];
            }
            break;
    }
}


bool
cell_equal (CELL alpha, CELL beta) {
    return *alpha == *beta;
}


int
cell_combine (CELL alpha, CELL beta) {
    *alpha = *alpha + *beta;  
    *beta = 0;

    return *alpha;
}


bool
cell_empty (CELL alpha) {
    return *alpha == 0;
}


bool
cell_collision (CELL alpha, CELL beta) {
    return ! (cell_empty(alpha) || cell_empty(beta));
}


void
cell_swap (CELL alpha, CELL beta) {
    int temp = *alpha;
    *alpha = *beta;
    *beta = temp;
}

bool
possible_move (CELL *line, direction_t dir) {
    bool can_move = false;

    for (int pos = 0; pos < BOARD_SIZE - 1; pos++) {
        if (cell_equal(line[pos], line[pos + 1])) {
            can_move = true;
        }
        if (cell_empty(line[pos]) || cell_empty(line[pos + 1])) {
            can_move = true;
        }
    }

    return can_move;
}


int
gravity (CELL *line, direction_t dir, int pos) {
    if (dir == UP) {
        int x = pos;
        for (; x < (BOARD_SIZE - 1) && cell_empty(line[x + 1]); x++) {
            cell_swap(line[x], line[x + 1]);
        }
    }
    else if (dir == DOWN) {
        int x = pos + 1;
        for (; x > 0 && cell_empty(line[x - 1]); x--) {
            cell_swap(line[x], line[x-1]);
        }
    }

    return 0; // only to comply with apply()'s signature
}


int
fusion (CELL *line, direction_t dir, int pos) {
    if (
        cell_collision(line[pos], line[pos + 1]) && 
        cell_equal(line[pos], line[pos + 1])
    ) {
        if (dir == UP) {
            return cell_combine(line[pos + 1], line[pos]);
        }
        else if (dir == DOWN) {
            return cell_combine(line[pos], line[pos + 1]);
        }
    }

    return 0;
}


int
apply (int (*p_func)(), CELL *line, direction_t dir) {
    int score = 0;

    switch (dir) {
        case UP:
            for (int i = BOARD_SIZE - 2; i >= 0; i--) {
                score += (*p_func)(line, UP, i);
            }
            break;

        case DOWN:
            for (int i = 0; i < BOARD_SIZE - 1; i++) {
                score += (*p_func)(line, DOWN, i);
            }
            break;

        default:
            fprintf(stderr, "Invalid direction. UP & DOWN only!\n");
            exit(EXIT_FAILURE);
    }

    return score;
}


int
apply_gravity_to_line (CELL *line, direction_t dir) {
    apply(gravity, line, dir);
    int score = apply(fusion, line, dir);
    apply(gravity, line, dir);

    return score;
}


int
apply_gravity_to_board (CELL *board, direction_t dir) {
    CELL line[4];
    int score = 0;
 
    direction_t line_direction; // The direction to apply to the line
    line_type_t line_type;      // Which line type should we use (col or row) 

    // DOWN refers to the direction where the 0th element is at the top.
    // UP refers to the direction where the 0th element is at the bottom.
    switch (dir) {
        case UP:    line_direction = DOWN;  line_type = COLUMN; break;
        case DOWN:  line_direction = UP;    line_type = COLUMN; break;
        case LEFT:  line_direction = DOWN;  line_type = ROW;    break;
        case RIGHT: line_direction = UP;    line_type = ROW;    break;
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        retrieve_line(board, line_type, i, line);
        score += apply_gravity_to_line(line, line_direction);
    }

    return score;
}


bool
no_more_moves (CELL *board) {
    CELL line[4];
    bool game_is_over = true;

    for (int i = 0; i < BOARD_SIZE; i++) {
        retrieve_line(board, COLUMN, i, line); 
        if (possible_move(line, UP) == true) 
            game_is_over = false;
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        retrieve_line(board, ROW, i, line);
        if (possible_move(line, UP) == true) 
            game_is_over = false;
    }

    return game_is_over;
}


bool
game_won (CELL *board) {
    CELL line[BOARD_SIZE];

    for (int i = 0; i < BOARD_SIZE; i++) {
        retrieve_line(board, COLUMN, i, line);
        for (int j = 0; j < BOARD_SIZE; j++) {
            CELL cell = line[j];
            if (*cell == 2048) {
                return true;
            }
        }

        retrieve_line(board, ROW, i, line);
        for (int j = 0; j < BOARD_SIZE; j++) {
            CELL cell = line[j];
            if (*cell == 2048) {
                return true;
            }
        }
    }

    return false;
}


void
insert_two_randomly (CELL *board) {
    CELL list[BOARD_SIZE * BOARD_SIZE] = {NULL};
    int i = 0;

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            CELL cell = board[y * BOARD_SIZE + x];
            if (*cell == 0) {
                list[i++] = cell;
            } 
        }
    }

    if (i == 0) {
        return;
    }

    CELL picked = list[rand() % i];
    *picked = 2;
}


double log2 (double x) {
    return log(x) / log(2);
}


