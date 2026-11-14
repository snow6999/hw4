#include "snake.h"
#include <stdlib.h>

bool snakeInit(Snake *snake, int rows, int cols, size_t length) {
    snake->m_rows = rows;
    snake->m_cols = cols;
    snake->m_length = length;
    snake->m_direction = DIR_RIGHT;
    snake->m_head = NULL;

    if (rows < MIN_BOARD_SIZE || cols < MIN_BOARD_SIZE || length == 0 ||
        length > (size_t)((rows - 2) * (cols - 2))) {
        *snake = (Snake){0};
        return false;
    }

    for (size_t i = 0; i < length; i++) {
        SnakeNode *node = malloc(sizeof(SnakeNode));
        if (node == NULL) {
            snake->m_length = i;
            snakeDestroy(snake);
            *snake = (Snake){0};
            return false;
        }
        node->m_pos.m_row = rows / 2;
        node->m_pos.m_col = cols / 2 - (int)i;
        if (i == 0) {
            snake->m_head = node;
            node->m_next = node;
        } else {
            node->m_next = snake->m_head->m_next;
            snake->m_head->m_next = node;
        }
    }

    return true;
}

Coord snakeNextPosition(const Snake *snake, Direction requested) {
    bool if_turn = true;
    Direction fin_dir = snake->m_direction;
    if (snake->m_length > 1) {
        if (requested == DIR_UP && snake->m_direction == DIR_DOWN) {
            if_turn = false;
        }
        if (requested == DIR_DOWN && snake->m_direction == DIR_UP) {
            if_turn = false;
        }
        if (requested == DIR_RIGHT && snake->m_direction == DIR_LEFT) {
            if_turn = false;
        }
        if (requested == DIR_LEFT && snake->m_direction == DIR_RIGHT) {
            if_turn = false;
        }
    }

    if (if_turn && requested >= DIR_UP && requested < DIRECTION_COUNT) {
        fin_dir = requested;
    }

    Coord next = snake->m_head->m_pos;
    if (fin_dir == DIR_UP) {
        next.m_row -= 1;
    } else if (fin_dir == DIR_DOWN) {
        next.m_row += 1;
    } else if (fin_dir == DIR_LEFT) {
        next.m_col -= 1;
    } else if (fin_dir == DIR_RIGHT) {
        next.m_col += 1;
    }
    return next;
}

bool snakeContains(const Snake *snake, Coord pos) {
    const SnakeNode *current = snake->m_head;
    for (size_t i = 0; i < snake->m_length; i++) {
        if (current->m_pos.m_row == pos.m_row && current->m_pos.m_col == pos.m_col) {
            return true;
        }
        current = current->m_next;
    }
    return false;
}

StepResult snakeStep(Snake *snake, Direction requested, bool grow) {
    Coord next_pos = snakeNextPosition(snake, requested);
    if (next_pos.m_row <= 0 || next_pos.m_row >= snake->m_rows - 1 || next_pos.m_col <= 0 ||
        next_pos.m_col >= snake->m_cols - 1) {
        return STEP_WALL;
    }

    SnakeNode *current = snake->m_head;
    SnakeNode *tail = snake->m_head->m_next;
    for (size_t i = 0; i < snake->m_length; i++) {
        if (current->m_pos.m_row == next_pos.m_row && current->m_pos.m_col == next_pos.m_col) {
            if (grow || current != tail) {
                return STEP_SELF;
            } else {
                break;
            }
        }
        current = current->m_next;
    }

    Coord old_head = snake->m_head->m_pos;
    if (grow) {
        SnakeNode *new_head = malloc(sizeof(SnakeNode));
        if (new_head == NULL) {
            return STEP_NO_MEMORY;
        }
        new_head->m_pos = next_pos;
        new_head->m_next = tail;
        snake->m_head->m_next = new_head;
        snake->m_head = new_head;
        snake->m_length++;
    } else {
        tail->m_pos = next_pos;
        snake->m_head = tail;
    }

    if (next_pos.m_row < old_head.m_row) {
        snake->m_direction = DIR_UP;
    } else if (next_pos.m_row > old_head.m_row) {
        snake->m_direction = DIR_DOWN;
    } else if (next_pos.m_col < old_head.m_col) {
        snake->m_direction = DIR_LEFT;
    } else if (next_pos.m_col > old_head.m_col) {
        snake->m_direction = DIR_RIGHT;
    }
    return STEP_OK;
}

void snakeDestroy(Snake *snake) {
    SnakeNode *current = snake->m_head;
    SnakeNode *next = NULL;
    for (size_t i = 0; i < snake->m_length; i++) {
        if (current == NULL) {
            break;
        }
        next = current->m_next;
        free(current);
        current = next;
    }
    *snake = (Snake){0};
}

unsigned int nextRandom(unsigned int seed) {
    static unsigned int state = RANDOM_INITIAL_SEED;
    if (seed > 0) {
        state = seed % RANDOM_MODULUS;
    }
    state = (RANDOM_MULTIPLIER * state + RANDOM_INCREMENT) % RANDOM_MODULUS;
    return state;
}