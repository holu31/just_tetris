#include <raylib.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

typedef struct __player_info {
    int score;
} player_info_t;

#define BLOCK_SIZE 20
#define MIN_X_BLOCK_POS 0
#define MAX_X_BLOCK_POS 14
#define MIN_Y_BLOCK_POS 0
#define MAX_Y_BLOCK_POS 24

#define SCORE_TEXT "SCORE: %d"

#define MOVE_DOWN(speed) if (timer >= speed) { \
                                    current_tet.position.y += 1; \
                                    timer = 0.0f; \
                                }
#define GET_BLOCK_POSITION_X(t, b) t.position.x + tetromino[t.type][t.rotation][b].x
#define GET_BLOCK_POSITION_Y(t, b) t.position.y + tetromino[t.type][t.rotation][b].y

player_info_t *player_info;

Vector2 tetromino[6][4][4] = {
    { // Square
        {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
        {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
        {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
        {{0, 0}, {1, 0}, {0, 1}, {1, 1}}
    },
    { // Line
        {{0, 0}, {0, 1}, {0, 2}, {0, 3}},
        {{0, 0}, {1, 0}, {2, 0}, {3, 0}},
        {{0, 0}, {0, 1}, {0, 2}, {0, 3}},
        {{0, 0}, {1, 0}, {2, 0}, {3, 0}}
    },
    { // L
        {{0, 0}, {0, 1}, {0, 2}, {1, 2}},
        {{0, 0}, {1, 0}, {2, 0}, {0, 1}},
        {{0, 0}, {1, 0}, {1, 1}, {1, 2}},
        {{2, 0}, {0, 1}, {1, 1}, {2, 1}}
    },
    { // reversed L
        {{1, 0}, {1, 1}, {1, 2}, {0, 2}},
        {{0, 0}, {1, 0}, {2, 0}, {2, 1}},
        {{0, 0}, {0, 1}, {0, 2}, {1, 0}},
        {{0, 0}, {0, 1}, {1, 1}, {2, 1}}
    },
    { // ZZZ GOIDA
        {{1, 0}, {1, 1}, {0, 1}, {2, 0}},
        {{0, 0}, {0, 1}, {1, 1}, {1, 2}},
        {{1, 0}, {1, 1}, {0, 1}, {2, 0}},
        {{0, 0}, {0, 1}, {1, 1}, {1, 2}}
    },
    { // idk
        {{1, 0}, {1, 1}, {0, 1}, {2, 1}},
        {{0, 1}, {1, 0}, {1, 1}, {1, 2}},
        {{1, 0}, {0, 1}, {1, 1}, {2, 1}},
        {{0, 0}, {0, 1}, {0, 2}, {1, 1}}
    }
};

Color tetromino_colors[6] = {
    YELLOW, BLUE, ORANGE, RED, GREEN, SKYBLUE
};

typedef struct __tetromino {
    Vector2 position;
    int type;
    int rotation;
    Color color;
} tetromino_t;

tetromino_t current_tet;

tetromino_t *all_tetrominos;
int tet_count = 0;

void gen_tetromino() {
    current_tet.position.x = GetRandomValue(MIN_X_BLOCK_POS, MAX_X_BLOCK_POS);
    current_tet.position.y = 0;
    current_tet.type = GetRandomValue(0, 5);
    current_tet.rotation = 0;
    current_tet.color = tetromino_colors[GetRandomValue(0, 5)];
}

void add_teromino() {
    tetromino_t new_tet;
    new_tet.position = current_tet.position;
    new_tet.type = current_tet.type;
    new_tet.rotation = current_tet.rotation;
    new_tet.color = current_tet.color;
            
    tet_count += 1;
    all_tetrominos = realloc(all_tetrominos, sizeof(tetromino_t) * (tet_count + 1));
    all_tetrominos[tet_count-1] = new_tet;
    gen_tetromino();
}

void draw_gui(Font font) {
    char *score_text = malloc((sizeof(char) * strlen(SCORE_TEXT)) +
            sizeof(int));
    sprintf(score_text, SCORE_TEXT, player_info->score);

    DrawTextEx(font, score_text, (Vector2) {(float) 500,
            (float) 50}, 32, 2, RAYWHITE);
    free(score_text);

    DrawRectangleLines(100, 40, 300, 500, DARKGRAY);
}

void check_collision_border() {
    for (int i = 0; i < 4; i++) {
        int x = GET_BLOCK_POSITION_X(current_tet, i);
        int y = GET_BLOCK_POSITION_Y(current_tet, i);

        if (x > MAX_X_BLOCK_POS) {
            current_tet.position.x -= 1;
        }

        if (x < MIN_X_BLOCK_POS) {
            current_tet.position.x += 1;
        }

        if (y > MAX_Y_BLOCK_POS) {
            current_tet.position.y -= 1;

            add_teromino();
            return;
        }
    }
}

#define COLLISION_BLOCK(action) for (int i = 0; i < 4; i++) { \
                int x = GET_BLOCK_POSITION_X(current_tet, i); \
                int y = GET_BLOCK_POSITION_Y(current_tet, i); \
                for (int j = 0; j < tet_count; j++) { \
                    tetromino_t tet = all_tetrominos[j]; \
                    for (int k = 0; k < 4; k++) { \
                        int x_other = GET_BLOCK_POSITION_X(tet, k); \
                        int y_other = GET_BLOCK_POSITION_Y(tet, k); \
                        if (y == y_other && x == x_other) { \
                            action; \
                        } \
                    } \
                } \
            } \

void check_y_collision_block() {
    COLLISION_BLOCK({
        current_tet.position.y -= 1.0f;
        add_teromino();
        return;
    });
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tetris");
    SetTargetFPS(20);

    all_tetrominos = malloc(sizeof(tetromino_t) * (tet_count + 1));

    player_info = malloc(sizeof(player_info_t));
    player_info->score = 0;

    Font font = LoadFontEx("resources/agat-8.ttf", 48, NULL, 0);

    gen_tetromino();

    float timer = 0.0f;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        timer += GetFrameTime();

        if (IsKeyDown(KEY_DOWN)) {
            MOVE_DOWN(0.1f);
        }
        else {
            MOVE_DOWN(0.5f);
        }

        if (IsKeyDown(KEY_RIGHT) && GET_BLOCK_POSITION_X(current_tet, 3) < MAX_X_BLOCK_POS) {
            current_tet.position.x += 1.0f;
            COLLISION_BLOCK(current_tet.position.x -= 1.0f);
        }
        if (IsKeyDown(KEY_LEFT) && current_tet.position.x > MIN_X_BLOCK_POS) {
            current_tet.position.x -= 1.0f;
            COLLISION_BLOCK(current_tet.position.x += 1.0f);
        }

        if (IsKeyPressed(KEY_UP)) {
            current_tet.rotation = (current_tet.rotation + 1) % 4;
        }

        check_y_collision_block();
        check_collision_border();
        // мне лень это делать
        // check_and_clear_lines();

        for (int i = 0; i < 4; i++) {
            int x = GET_BLOCK_POSITION_X(current_tet, i) + 5;
            int y = GET_BLOCK_POSITION_Y(current_tet, i) + 2;

            DrawRectangle(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, current_tet.color);
        }

        for (int i = 0; i < tet_count; i++) {
            tetromino_t tet = all_tetrominos[i];

            for (int j = 0; j < 4; j++) {
                int x = GET_BLOCK_POSITION_X(tet, j) + 5;
                int y = GET_BLOCK_POSITION_Y(tet, j) + 2;
                if (GET_BLOCK_POSITION_Y(tet, j) <= MIN_Y_BLOCK_POS)
                    CloseWindow();

                DrawRectangle(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, tet.color);
            }
        }

        draw_gui(font);

        EndDrawing();
    }

    UnloadFont(font);
    free(player_info);
    CloseWindow();

    return 0;
}