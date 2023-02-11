#include "game_setup.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Some handy dandy macros for decompression
#define E_CAP_HEX 0x45
#define E_LOW_HEX 0x65
#define S_CAP_HEX 0x53
#define S_LOW_HEX 0x73
#define W_CAP_HEX 0x57
#define W_LOW_HEX 0x77
#define DIGIT_START 0x30
#define DIGIT_END 0x39

/** Initializes the board with walls around the edge of the board.
 *
 * Modifies values pointed to by cells_p, width_p, and height_p and initializes
 * cells array to reflect this default board.
 *
 * Returns INIT_SUCCESS to indicate that it was successful.
 *
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 */
enum board_init_status initialize_default_board(int** cells_p, size_t* width_p,
                                                size_t* height_p) {
    *width_p = 20;
    *height_p = 10;
    int* cells = malloc(20 * 10 * sizeof(int));
    *cells_p = cells;
    for (int i = 0; i < 20 * 10; i++) {
        cells[i] = FLAG_PLAIN_CELL;
    }

    // Set edge cells!
    // Top and bottom edges:
    for (int i = 0; i < 20; ++i) {
        cells[i] = FLAG_WALL;
        cells[i + (20 * (10 - 1))] = FLAG_WALL;
    }
    // Left and right edges:
    for (int i = 0; i < 10; ++i) {
        cells[i * 20] = FLAG_WALL;
        cells[i * 20 + 20 - 1] = FLAG_WALL;
    }

    // Add snake
    cells[20 * 2 + 2] = FLAG_SNAKE;

    return INIT_SUCCESS;
}

/** Initialize variables relevant to the game board.
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 *  - snake_p: a pointer to your snake struct (not used until part 2!)
 *  - board_rep: a string representing the initial board. May be NULL for
 * default board.
 */
enum board_init_status initialize_game(int** cells_p, size_t* width_p,
                                       size_t* height_p, snake_t* snake_p,
                                       char* board_rep) {
    // TODO: implement!
    enum board_init_status result;
    if (board_rep == NULL) {
        result = initialize_default_board(cells_p, width_p, height_p);
        g_snake_row = 3;
        g_snake_column = 3;
    }
    else {
        result = decompress_board_str(cells_p, width_p, height_p, snake_p, board_rep);
    }
    if (result == INIT_SUCCESS) {
        place_food(*cells_p, *width_p, *height_p);
        g_snake_dir = SNAKE_RIGHT;
        g_game_over = 0;  // 1 if game is over, 0 otherwise
        g_score = 0;
    }
     
    return result;
}

/** Takes in a cells_ptr and adds a desired number and type of cells to the 
 *                  location of cells_ptr while updating cells_ptr. Assume 
 *                  there is enough space allocated after cells_ptr, and 
 *                  cell_type is well-formed. Arguments:
 *      - cells_ptr: a pointer to the pointer representing the current location
 *                  of where chars will be inserted.
 *      - cell_type: a char* of either 'W', 'E', or 'S' representing the type
 *                 of cell to be inserted.
 *      - num_chars: an int representing the number of cells to be added to 
 *                 end of cell_p.
 */
void add_to_board(int** cells_ptr, char cell_type, int num_chars) {
    int cell_value;
    switch (cell_type) {
        case 'W':
            cell_value = FLAG_WALL;
            break;
        case 'E':
            cell_value = FLAG_PLAIN_CELL;
            break;
        case 'S':
            cell_value = FLAG_SNAKE;
            break;
    }
    int i;
    for (i = 0; i < num_chars; i++) {
        **cells_ptr = cell_value;
        (*cells_ptr)++;
    }
}


/** Takes in a string `compressed` and initializes values pointed to by
 * cells_p, width_p, and height_p accordingly. Arguments:
 *      - cells_p: a pointer to the pointer representing the cells array
 *                 that we would like to initialize.
 *      - width_p: a pointer to the width variable we'd like to initialize.
 *      - height_p: a pointer to the height variable we'd like to initialize.
 *      - snake_p: a pointer to your snake struct (not used until part 2!)
 *      - compressed: a string that contains the representation of the board.
 * Note: We assume that the string will be of the following form:
 * B24x80|E5W2E73|E5W2S1E72... To read it, we scan the string row-by-row
 * (delineated by the `|` character), and read out a letter (E, S or W) a number
 * of times dictated by the number that follows the letter.
 */
enum board_init_status decompress_board_str(int** cells_p, size_t* width_p,
                                            size_t* height_p, snake_t* snake_p,
                                            char* compressed) {
    // TODO: implement!
    int row_num = 1; //first row will be row 1
    int column_num = 0; //first column will be column 1
    int snake_count = 0;
    char* current_segment = compressed;
    int num_chars;
    int height;
    int width;
    int* cells_ptr;

    //Determine expected dimensions of board using beginning segment
    if (compressed[0] != 'B') {
        return INIT_ERR_BAD_CHAR;
    }
    current_segment = strtok(&compressed[1], "x");
    *height_p = atoi(current_segment);
    height = *height_p;
    current_segment = strtok(NULL, "|");
    *width_p = atoi(current_segment);
    width = *width_p;
    cells_ptr = malloc((*width_p) * (*height_p) * sizeof(int));
    *cells_p = cells_ptr;

    //Iterate through the main segment of compressed
    while (current_segment != NULL) { //Loop through every row
        current_segment = strtok(NULL, "|");
        int i;
        for (i = 0; current_segment[i] != '\0'; i++) { //Loop through every column
            switch (*current_segment) {
                case 'W':
                    num_chars = atoi(current_segment + 1);
                    break;
                case 'E':
                    num_chars = atoi(current_segment + 1);
                    break;
                case 'S':
                    num_chars = atoi(current_segment + 1);
                    if ((num_chars != 1) || (snake_count == 1)) {
                        return INIT_ERR_WRONG_SNAKE_NUM;
                    }
                    snake_count++;
                    g_snake_row = row_num;
                    g_snake_column = column_num + num_chars;
                    break;
                default:
                    return INIT_ERR_BAD_CHAR;
            }
            column_num += num_chars;
            if (column_num > width) {
                return INIT_ERR_INCORRECT_DIMENSIONS;
            }
            else {
                add_to_board(&cells_ptr, *current_segment, num_chars);
            }
        }
        
        if (column_num != width) {
                return INIT_ERR_INCORRECT_DIMENSIONS;
        }
        column_num = 0;
        row_num += 1;
        if (row_num > height) {
            return INIT_ERR_INCORRECT_DIMENSIONS;
        }
    } 
    if (row_num != height) {
        return INIT_ERR_INCORRECT_DIMENSIONS;
    }
    else if (snake_count != 1) {
        return INIT_ERR_WRONG_SNAKE_NUM;
    }
    return INIT_SUCCESS;
}
