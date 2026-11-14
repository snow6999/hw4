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
    if (bird.m_y < 0 || bird.m_y + BIRD_HEIGHT > FLOOR_Y) {
        return true;
    }

    for (int i = 0; i < PIPE_COUNT; i++) {
        float pipe_x = game->m_pipes[i].m_x;
        if (bird.m_x < pipe_x + PIPE_WIDTH && bird.m_x + BIRD_WIDTH > pipe_x) {
            float gap_top = game->m_pipes[i].m_gap_y - PIPE_GAP_HEIGHT / 2.0f;
            float gap_bottom = game->m_pipes[i].m_gap_y + PIPE_GAP_HEIGHT / 2.0f;

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