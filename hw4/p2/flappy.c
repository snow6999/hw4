#include "flappy.h"

void advancePipes(Game *game, float dt) {
    for (int i = 0; i < PIPE_COUNT; i++) {
        game->m_pipes[i].m_x -= PIPE_SPEED * dt;
    }
    for (int i = 0; i < PIPE_COUNT; i++) {
        if (game->m_pipes[i].m_x + PIPE_WIDTH <= 0) {
            recyclePipe(game, i);
        }
    }
}

bool birdHitsObstacle(const Game *game) {
    Box bird;
    bird = gameBirdBox(game);
    
    // 撞天花板或地板
    if (bird.m_y < 0 || bird.m_y + BIRD_HEIGHT > FLOOR_Y) {
        return true;
    }

    for (int i = 0; i < PIPE_COUNT; i++) {
        float pipe_x = game->m_pipes[i].m_x;
        
        // 判断小鸟X轴是否和管道X轴有重叠
        if (bird.m_x < pipe_x + PIPE_WIDTH && bird.m_x + BIRD_WIDTH > pipe_x) {
            
            // 计算缺口的上下边界
            float gap_top = game->m_pipes[i].m_gap_y - PIPE_GAP_HEIGHT / 2.0f;
            float gap_bottom = game->m_pipes[i].m_gap_y + PIPE_GAP_HEIGHT / 2.0f;

            // 撞上管道：小鸟的 y 小于缺口顶部
            // 撞下管道：小鸟的 y + 高度 大于缺口底部
            // 注意：这里是严格的小于和大于（因为没有等于，所以碰到边缘算碰撞！）
            if (bird.m_y < gap_top || bird.m_y + BIRD_HEIGHT > gap_bottom) {
                return true;
            }
        }
    }

    return false;
}

int scorePassedPipes(Game *game) {
    int score = 0;
    for (int i = 0; i < PIPE_COUNT; i++) {
        if (game->m_pipes[i].m_x + PIPE_WIDTH < BIRD_X && game->m_pipes[i].m_scored == false) {
            game->m_pipes[i].m_scored = true;
            game->m_score++;
            score++;
        }
    }
    return score;
}

unsigned int nextRandom(unsigned int seed) {
    static unsigned int state = RANDOM_INITIAL_SEED;
    if (seed > 0) {
        state = seed % RANDOM_MODULUS;
    }
    state = (RANDOM_MULTIPLIER * state + RANDOM_INCREMENT) % RANDOM_MODULUS;
    return state;
}