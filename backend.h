
#include <stdbool.h>

#define BOARD_SIZE 3
#define CELL unsigned int *

typedef enum { ROW, COLUMN } line_type_t; 
typedef enum { UP, RIGHT, DOWN, LEFT } direction_t;

CELL *  create_board    ();
void    destroy_board   ();

void    retrieve_line   (CELL *board, line_type_t line, unsigned int nth_line, CELL *ret);
bool    no_more_moves           (CELL *board);
bool    game_won                (CELL *board);
void    insert_two_randomly     (CELL *board);
int     apply_gravity_to_board  (CELL *board, direction_t dir);

bool    possible_move   (CELL *line, direction_t dir);
int     gravity         (CELL *line, direction_t dir, int pos);
int     fusion          (CELL *line, direction_t dir, int pos);
int     apply           (int (*p_func)(), CELL *line, direction_t dir);


bool    cell_equal      (CELL alpha, CELL beta);
int     cell_combine    (CELL alpha, CELL beta);
bool    cell_empty      (CELL alpha);
bool    cell_collision  (CELL alpha, CELL beta);
void    cell_swap       (CELL alpha, CELL beta);

double log2 (double x);

