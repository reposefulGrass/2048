
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define BOARD_SIZE 4
#define CELL unsigned int *

typedef enum { ROW, COLUMN } line_type_t; 
typedef enum { UP, RIGHT, DOWN, LEFT } direction_t;


CELL *
create_board () {
    // a (BOARD_SIZE x BOARD_SIZE) board where each cell is a 'unsigned int *'
    CELL *board = (CELL *) malloc(sizeof(CELL) * BOARD_SIZE * BOARD_SIZE);

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            CELL temp = (CELL) malloc(sizeof(unsigned int));
            *temp = 0;

            board[(4 * y) + x] = temp;
        }
    }

    return board;
}


void
destroy_board (CELL *board) {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[(4 * y) + x] != NULL) {
                free(board[(4 * y) + x]);
            } 
        }
    }

    free(board);
}


void
print_board (CELL *board) {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            CELL cell = board[(BOARD_SIZE * y) + x];
            if (*cell != 0) {
                printf("%5d", *cell); 
            }
            else {
                printf("%5s", "_");
            }
        }
        putchar('\n');
    }
}


void
retrieve_line (CELL *board, line_type_t line, unsigned int nth_line, CELL *ret) {
    switch (line) {
        case ROW:
            *(ret + 0)  = board[(BOARD_SIZE * nth_line) + 0];
            *(ret + 1)  = board[(BOARD_SIZE * nth_line) + 1];
            *(ret + 2)  = board[(BOARD_SIZE * nth_line) + 2];
            *(ret + 3)  = board[(BOARD_SIZE * nth_line) + 3];
            break;

        case COLUMN:
            *(ret + 0)  = board[nth_line + BOARD_SIZE * 0];
            *(ret + 1)  = board[nth_line + BOARD_SIZE * 1];
            *(ret + 2)  = board[nth_line + BOARD_SIZE * 2];
            *(ret + 3)  = board[nth_line + BOARD_SIZE * 3];
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
    if (cell_collision(line[pos], line[pos + 1]) && cell_equal(line[pos], line[pos + 1])) {
        if (dir == UP) {
            return cell_combine(line[pos + 1], line[pos]);
        }
        else if (dir == DOWN) {
            return cell_combine(line[pos], line[pos + 1]);
        }
    }
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


int
apply_gravity_to_board (CELL *board, direction_t dir) {
    CELL line[4];
    int score = 0;
 
    direction_t line_direction; // The direction to apply to the line
    line_type_t line_type;      // Which lines should we use (col or row) 

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

void
clear_screen () {
    printf("\x1b[2J");
}

void
move_cursor_to_pos (int row, int col) {
    printf("\x1b[%d;%dH", row, col);
}

typedef enum { VALID, INVALID }
    move_t;

int 
main () {
    srand(time(NULL));
    CELL *board = create_board();
    char input[10];
    int score = 0;
    move_t last_move = VALID;

    while (true) {
        clear_screen();
        move_cursor_to_pos(0, 0);

        if (last_move == VALID) {
            insert_two_randomly(board);
        }

        putchar('\n');
        print_board(board);
        putchar('\n');

        printf("    Score: %d\n\n", score);
          
        printf(">> ");
        scanf(" %10s", input);

        int c;
        while ((c = getc(stdin)) != '\n')
            ;

        if (strcmp(input, "up") == 0 || strcmp(input, "u") == 0) {
            score += apply_gravity_to_board(board, UP);
        }
        else if (strcmp(input, "right") == 0 || strcmp(input, "r") == 0) {
            score += apply_gravity_to_board(board, RIGHT);
        }
        else if (strcmp(input, "down") == 0 || strcmp(input, "d") == 0) {
            score += apply_gravity_to_board(board, DOWN);
        }
        else if (strcmp(input, "left") == 0 || strcmp(input, "l") == 0) {
            score += apply_gravity_to_board(board, LEFT);
        }
        else if (strcmp(input, "quit") == 0 || strcmp(input, "q") == 0) {
            break;
        }
        else {
            putchar('\n');
            printf("Enter one of the following directions: 'up', 'down', 'left', 'right'.\n");
            printf("Press 'ENTER' to continue!\n");
            int c = getc(stdin);

            last_move = INVALID;
        }

        if (no_more_moves(board)) {
            putchar('\n');
            printf("There are no more possible moves!\n");
            break; // while(true)
        }

        if (game_won(board)) {
            putchar('\n');
            printf("You Won!\n");
            break; // while(true)
        }
    }

    destroy_board(board);
}


