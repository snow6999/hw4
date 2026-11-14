#include "snake.h"

#include <stdlib.h>

/* True when the two directions point at each other. */
static bool isOpposite(Direction first, Direction second) {
    switch (first) {
    case DIR_UP:
        return second == DIR_DOWN;
    case DIR_DOWN:
        return second == DIR_UP;
    case DIR_LEFT:
        return second == DIR_RIGHT;
    case DIR_RIGHT:
        return second == DIR_LEFT;
    default:
        return false;
    }
}

/* Keep the current direction for invalid or forbidden reverse requests. */
static Direction chooseDirection(const Snake *snake, Direction requested) {
    if ((int)requested < 0 || requested >= DIRECTION_COUNT) {
        return snake->m_direction;
    }
    if (snake->m_length > 1 && isOpposite(snake->m_direction, requested)) {
        return snake->m_direction;
    }
    return requested;
}

/* The cell one step away from the given coordinate in the given direction. */
static Coord positionAfter(Coord from, Direction direction) {
    Coord next = from;
    switch (direction) {
    case DIR_UP:
        --next.m_row;
        break;
    case DIR_DOWN:
        ++next.m_row;
        break;
    case DIR_LEFT:
        --next.m_col;
        break;
    case DIR_RIGHT:
        ++next.m_col;
        break;
    default:
        break;
    }
    return next;
}

/* Walk the ring once. A normal move may take the old tail's cell, so that
 * single node can be skipped; growth must consider every node. */
static bool bodyOccupies(const Snake *snake, Coord pos, bool skipTail) {
    if (snake->m_head == NULL) {
        return false;
    }
    const SnakeNode *tail = snake->m_head->m_next;
    const SnakeNode *node = snake->m_head;
    for (size_t i = 0; i < snake->m_length; ++i) {
        bool isTail = node == tail && snake->m_length > 1;
        if (!(skipTail && isTail) && node->m_pos.m_row == pos.m_row &&
            node->m_pos.m_col == pos.m_col) {
            return true;
        }
        node = node->m_next;
    }
    return false;
}

bool snakeInit(Snake *snake, int rows, int cols, size_t length) {
    snake->m_head = NULL;
    snake->m_length = 0;
    snake->m_direction = DIR_RIGHT;
    snake->m_rows = rows;
    snake->m_cols = cols;

    if (rows < MIN_BOARD_SIZE || cols < MIN_BOARD_SIZE) {
        *snake = (Snake){0};
        return false;
    }
    int maxLength = cols - 2 * BOARD_WALL_THICKNESS;
    if (length == 0 || length > (size_t)maxLength) {
        *snake = (Snake){0};
        return false;
    }

    int row = rows / 2;
    int headCol = cols / 2;
    if (headCol < (int)length) {
        headCol = (int)length;
    }

    /* Build from the tail towards the head: each new node becomes m_head,
     * and m_head->m_next always stays the oldest (tail) node. */
    for (size_t i = 0; i < length; ++i) {
        SnakeNode *node = malloc(sizeof(*node));
        if (node == NULL) {
            snakeDestroy(snake);
            return false;
        }
        node->m_pos.m_row = row;
        node->m_pos.m_col = headCol - (int)length + 1 + (int)i;
        if (snake->m_head == NULL) {
            node->m_next = node;
            snake->m_head = node;
        } else {
            node->m_next = snake->m_head->m_next;
            snake->m_head->m_next = node;
            snake->m_head = node;
        }
        ++snake->m_length;
    }
    return true;
}

Coord snakeNextPosition(const Snake *snake, Direction requested) {
    if (snake->m_head == NULL) {
        return (Coord){0};
    }
    Direction direction = chooseDirection(snake, requested);
    return positionAfter(snake->m_head->m_pos, direction);
}

bool snakeContains(const Snake *snake, Coord pos) {
    if (snake->m_head == NULL || snake->m_length == 0) {
        return false;
    }
    const SnakeNode *node = snake->m_head;
    for (size_t i = 0; i < snake->m_length; ++i) {
        if (node->m_pos.m_row == pos.m_row && node->m_pos.m_col == pos.m_col) {
            return true;
        }
        node = node->m_next;
    }
    return false;
}

StepResult snakeStep(Snake *snake, Direction requested, bool grow) {
    Direction direction = chooseDirection(snake, requested);
    Coord next = positionAfter(snake->m_head->m_pos, direction);

    if (next.m_row < BOARD_WALL_THICKNESS || next.m_row >= snake->m_rows - BOARD_WALL_THICKNESS ||
        next.m_col < BOARD_WALL_THICKNESS || next.m_col >= snake->m_cols - BOARD_WALL_THICKNESS) {
        return STEP_WALL;
    }
    if (bodyOccupies(snake, next, !grow)) {
        return STEP_SELF;
    }

    if (grow) {
        SnakeNode *node = malloc(sizeof(*node));
        if (node == NULL) {
            return STEP_NO_MEMORY;
        }
        node->m_pos = next;
        node->m_next = snake->m_head->m_next;
        snake->m_head->m_next = node;
        snake->m_head = node;
        ++snake->m_length;
    } else {
        SnakeNode *tail = snake->m_head->m_next;
        tail->m_pos = next;
        snake->m_head = tail;
    }
    snake->m_direction = direction;
    return STEP_OK;
}

void snakeDestroy(Snake *snake) {
    if (snake->m_head != NULL) {
        SnakeNode *node = snake->m_head->m_next;
        while (node != snake->m_head) {
            SnakeNode *next = node->m_next;
            free(node);
            node = next;
        }
        free(snake->m_head);
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
