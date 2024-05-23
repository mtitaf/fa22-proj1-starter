#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

static void allocation_failed()
{
    fprintf(stderr, "Out of memory.\n");
    exit(1);
}

/* Task 1 */
game_state_t *create_default_state()
{
    game_state_t *t;
    t = malloc(sizeof(game_state_t));
    if (t == NULL)
    {
        allocation_failed();
    }

    t->num_rows = 18;
    t->num_snakes = 1;

    t->board = malloc(t->num_rows * sizeof(char*));
    if (t->board == NULL)
    {
        allocation_failed();
    }

    size_t colume = 20;

    for (int i = 0; i < t->num_rows; i++)
    {
        t->board[i] = malloc((colume + 1) * sizeof(char));
        if (t->board == NULL) {
            allocation_failed();
        } 

        if (i == 0 || i == t->num_rows - 1)
        {
            strcpy(t->board[i], "####################");
        } 
        else if (i == 2)
        {
            strcpy(t->board[i], "# d>D    *         #");
        }
        else
        {
            strcpy(t->board[i], "#                  #");
        }
    }


    t->snakes = malloc(sizeof(snake_t));
    if (t->snakes == NULL)
    {
        allocation_failed();
    }

    t->snakes->head_col = 4;
    t->snakes->head_row = 2;
    t->snakes->tail_col = 2;
    t->snakes->tail_row = 2;
    t->snakes->live = true;

    

    // TODO: Implement this function.
    return t;
}

/* Task 2 */
void free_state(game_state_t *state)
{
    for (int i = 0; i < state->num_rows; i++) {
        free(state->board[i]);
    }
    free(state->board);
    free(state->snakes);
    free(state);
    // TODO: Implement this function.
    return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp)
{
    for (int i = 0; i < state->num_rows; i++) {
        fprintf(fp, "%s\n", state->board[i]);
    }
    // TODO: Implement this function.
    return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename)
{
    FILE *f = fopen(filename, "w");
    print_board(state, f);
    fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col)
{
    return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch)
{
    state->board[row][col] = ch;
}


static bool in_char(char *str, char c) {

    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == c) {
            return true;
        }
    }
    return false;
}
/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c)
{
    char *str = "wasd";
    // TODO: Implement this function.
    return in_char(str, c);
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c)
{
    char *str = "WASDx";
    // TODO: Implement this function.
    bool ish = in_char(str, c);
    return ish;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c)
{
    char *str = "wasd^<v>WASDx";
    // TODO: Implement this function.
    return in_char(str, c);
}


/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c)
{
    if (c == '^') {
        return 'w';
    } else if (c == '>')
    {
        return 'd';
    } else if (c == 'v')
    {
        return 's';
    } else if (c == '<')
    {
        return 'a';
    }
    return '?';
    
    // TODO: Implement this function.
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c)
{
    if (c == 'W') {
        return '^';
    } else if (c == 'D')
    {
        return '>';
    } else if (c == 'S')
    {
        return 'v';
    } else if (c == 'A')
    {
        return '<';
    }
    // TODO: Implement this function.
    return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c)
{
    if (c == 'v' || c == 's' || c == 'S') {
        return cur_row + 1;
    } else if (c == '^' || c == 'w' || c == 'W') {
        return cur_row - 1;
    }
    // TODO: Implement this function.
    return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c)
{
        if (c == '>' || c == 'd' || c == 'D') {
        return cur_col + 1;
    } else if (c == '<' || c == 'a' || c == 'A') {
        return cur_col - 1;
    }
    // TODO: Implement this function.
    return cur_col;
}


void find_snakes(game_state_t *state, unsigned int snum) {
    unsigned int board_snakes = 0;
    for (unsigned int r = 0; r < state->num_rows; r++) {
        for (unsigned int c = 0; c < strlen(state->board[r]); c++) {
            if (is_tail(get_board_at(state, r, c))) {
                board_snakes += 1;
                if (board_snakes - 1 == snum) {
                    state->snakes->tail_row = r;
                    state->snakes->tail_col = c;
                    state->snakes->head_row = r;
                    state->snakes->head_col = c;
                    while (!is_head(get_board_at(state, state->snakes->head_row, state->snakes->head_col))) {
                        char c = get_board_at(state, state->snakes->head_row, state->snakes->head_col);
                        state->snakes->head_row = get_next_row(state->snakes->head_row, c);
                        state->snakes->head_col = get_next_col(state->snakes->head_col, c);
                    }
                    return;
                }
            }
        }
    }
            


}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum)
{   
    find_snakes(state, snum);
    char c = get_board_at(state, state->snakes->head_row, state->snakes->head_col);
    char next = get_board_at(state, get_next_row(state->snakes->head_row, c), get_next_col(state->snakes->head_col, c) );
    return next;
    
    // TODO: Implement this function.
}


/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum)
{
    // find_snakes(state, snum);
    char h_next = next_square(state, snum);
    char head = get_board_at(state, state->snakes->head_row, state->snakes->head_col);
    if (h_next == '#') {
        set_board_at(state, state->snakes->head_row, state->snakes->head_col, 'x');
    } else {
        set_board_at(state, state->snakes->head_row, state->snakes->head_col, head_to_body(head));
        state->snakes->head_row = get_next_row(state->snakes->head_row, head);
        state->snakes->head_col = get_next_col(state->snakes->head_col, head);
        set_board_at(state, state->snakes->head_row, state->snakes->head_col, head);
    }

    // TODO: Implement this function.
    return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum)
{
    // TODO: Implement this function.
    return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state))
{
    // TODO: Implement this function.
    return;
}

/* Task 5 */
game_state_t *load_board(char *filename)
{
    // TODO: Implement this function.
    return NULL;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum)
{
    // TODO: Implement this function.
    return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state)
{
    // TODO: Implement this function.
    return NULL;
}
