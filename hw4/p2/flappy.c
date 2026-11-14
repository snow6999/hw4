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
    Box bird = gameBirdBox(game);
    
    if (bird.m_y < 0 || bird.m_y + bird.m_height > FLOOR_Y) {
        return true;
    }
    
    for (int i = 0; i < PIPE_COUNT; i++) {
        // 上管道
        Box top_pipe;
        top_pipe.m_x = game->m_pipes[i].m_x;
        top_pipe.m_y = 0.0f;
        top_pipe.m_width = PIPE_WIDTH;
        top_pipe.m_height = game->m_pipes[i].m_gap_y;
        
        if (boxesOverlap(bird, top_pipe)) {
            return true;
        }
        

        Box bottom_pipe;
        bottom_pipe.m_x = game->m_pipes[i].m_x;
        bottom_pipe.m_y = game->m_pipes[i].m_gap_y + PIPE_GAP_HEIGHT;
        bottom_pipe.m_width = PIPE_WIDTH;
        bottom_pipe.m_height = FLOOR_Y - game->m_pipes[i].m_gap_y - PIPE_GAP_HEIGHT;
        
        if (boxesOverlap(bird, bottom_pipe)) {
            return true;
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
