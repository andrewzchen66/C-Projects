#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "linked_list.h"
#include "mbstrings.h"




/** Updates the game by a single step, and modifies the game information
 * accordingly. Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: width of the board.
 *  - height: height of the board.
 *  - snake_p: pointer to your snake struct (not used until part 2!)
 *  - input: the next input.
 *  - growing: 0 if the snake does not grow on eating, 1 if it does.
 */
void update(int* cells, size_t width, size_t height, snake_t* snake_p,
            enum input_key input, int growing) {
    // `update` should update the board, your snake's data, and global
    // variables representing game information to reflect new state. If in the
    // updated position, the snake runs into a wall or itself, it will not move
    // and global variable g_game_over will be 1. Otherwise, it will be moved
    // to the new position. If the snake eats food, the game score (`g_score`)
    // increases by 1. This function assumes that the board is surrounded by
    // walls, so it does not handle the case where a snake runs off the board.

    // TODO: implement!
    if (g_game_over != 1) {
        int* snake_head_position = snake_p->g_snake_head_position->data;
        int next_snake_head_position = *snake_head_position;
        int snake_length = length_list(snake_p->g_snake_head_position);
        enum snake_direction* snake_dir = &(snake_p->g_snake_dir);
        int* last_position;

        //Process keyboard input differently based on snake size
        if (snake_length > 1) {
            switch (input) {
                case INPUT_UP:
                    if (*snake_dir != SNAKE_DOWN) {
                        *snake_dir = SNAKE_UP;
                    }
                    break;
                case INPUT_DOWN:
                    if (*snake_dir != SNAKE_UP) {
                        *snake_dir = SNAKE_DOWN;
                    }
                    break;
                case INPUT_LEFT:
                    if (*snake_dir != SNAKE_RIGHT) {
                        *snake_dir = SNAKE_LEFT;
                    }
                    break;
                case INPUT_RIGHT:
                    if (*snake_dir != SNAKE_LEFT) {
                        *snake_dir = SNAKE_RIGHT;
                    }
                    break;
                case INPUT_NONE:
                    break;
            }
        }
        else {
            switch (input) {
                case INPUT_UP:
                    snake_p->g_snake_dir = SNAKE_UP;
                    break;
                case INPUT_DOWN:
                    snake_p->g_snake_dir = SNAKE_DOWN;
                    break;
                case INPUT_LEFT:
                    snake_p->g_snake_dir = SNAKE_LEFT;
                    break;
                case INPUT_RIGHT:
                    snake_p->g_snake_dir = SNAKE_RIGHT;
                    break;
                case INPUT_NONE:
                    break;
            }
        }

        switch (snake_p->g_snake_dir) {
            case SNAKE_UP:
                next_snake_head_position -= width;
                break;
            case SNAKE_DOWN:
                next_snake_head_position += width;
                break;
            case SNAKE_LEFT:
                next_snake_head_position -= 1;
                break;
            case SNAKE_RIGHT:
                next_snake_head_position += 1;
                break;
        }

        if (cells[next_snake_head_position] == FLAG_WALL) { //If Snake hits wall
            g_game_over = 1;
        }
        else {
            if (cells[next_snake_head_position] == FLAG_FOOD) { //If snake encounters food
                g_score++;
                int* next_position_ptr = &next_snake_head_position;
                insert_first(&(snake_p->g_snake_head_position), next_position_ptr, sizeof(int));
                if (growing == 0) {
                    last_position = remove_last(&(snake_p->g_snake_head_position));
                }
                place_food(cells, width, height);
            }
            else { //Normal snake movement
                int* next_position_ptr = &next_snake_head_position;
                insert_first(&(snake_p->g_snake_head_position), next_position_ptr, sizeof(int));
                last_position = remove_last(&(snake_p->g_snake_head_position));
            }
            
            //Handle snake colliding with itself
            if ((cells[next_snake_head_position] == FLAG_SNAKE) && (next_snake_head_position != *last_position)) {
                g_game_over = 1;
                free(last_position);
            }
            else if ((cells[next_snake_head_position] == FLAG_FOOD) && (growing == 1)) {
                cells[next_snake_head_position] = FLAG_SNAKE;
            }
            else {
                cells[*last_position] = FLAG_PLAIN_CELL;
                cells[next_snake_head_position] = FLAG_SNAKE;
                free(last_position);
            }
        }
    }
}

/** Sets a random space on the given board to food.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: the width of the board
 *  - height: the height of the board
 */
void place_food(int* cells, size_t width, size_t height) {
    /* DO NOT MODIFY THIS FUNCTION */
    unsigned food_index = generate_index(width * height);
    if (*(cells + food_index) == FLAG_PLAIN_CELL) {
        *(cells + food_index) = FLAG_FOOD;
    } else {
        place_food(cells, width, height);
    }
    /* DO NOT MODIFY THIS FUNCTION */
}

/** Prompts the user for their name and saves it in the given buffer.
 * Arguments:
 *  - `write_into`: a pointer to the buffer to be written into.
 */
void read_name(char* write_into) {
    // TODO: implement! (remove the call to strcpy once you begin your
    // implementation)
    int reprompt = 1;
    while(reprompt == 1) {
        printf("Name > ");
        fflush(stdout);
        read(0, write_into, 1000);
        if (*write_into == '\n') {
            printf("Name Invalid: must be longer than 0 characters.\n");
            fflush(stdout);
        }
        else {
            reprompt = 0;
        }
    }
    int i = 0;
    while(*(write_into + i) != '\0') {
        if (*(write_into + i) == '\n') {
            *(write_into + i) = '\0';
        }
        else {
            i++;
        }
    }
}

/** Cleans up on game over — should free any allocated memory so that the
 * LeakSanitizer doesn't complain.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - snake_p: a pointer to your snake struct. (not needed until part 2)
 */
void teardown(int* cells, snake_t* snake_p) {
    // TODO: implement!
    free(cells);
    int* data;
    while (snake_p->g_snake_head_position) {
        data = remove_last(&(snake_p->g_snake_head_position));
        free(data);
    }
}
