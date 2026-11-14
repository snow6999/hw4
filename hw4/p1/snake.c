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
        node->m_pos.m_col = cols / 2 - (int)length + 1 + (int)i;
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
