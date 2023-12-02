#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>

#define SIZE 4

struct game {
    int tiles[SIZE][SIZE];
	int score;
};

enum {
    DOWN,
    UP,
    LEFT,
    RIGHT,
    MOVES
};

int random_spot(struct game* g)
{
    return rand() % SIZE;
}

int random_value(struct game* g)
{
    return (rand() % 10) ? 2 : 4;
}

void print(struct game* g)
{
    printf("--------------------------\n");
    for (int j = SIZE - 1; j >= 0; --j) {
        printf("|");
        for (int i = 0; i < SIZE; ++i) {
            if (g->tiles[i][j])
                printf("%4d  ", g->tiles[i][j]);
            else
                printf("      ");
        }
        printf("|\n");
    }
    printf("--------------------------\n");
}

void twist(struct game* g)
{
    struct game g2;
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            g2.tiles[i][j] = g->tiles[j][i];
    *g = g2;
}

void flip(struct game* g)
{
    struct game g2;
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            g2.tiles[i][j] = g->tiles[i][SIZE - j - 1];
    *g = g2;
}

void begin(struct game* g)
{
    struct game g2 = {};
    *g = g2;
    g->tiles[random_spot(g)][random_spot(g)] = random_value(g);
    g->tiles[random_spot(g)][random_spot(g)] = random_value(g);
}

void fall_column(int* a, int* b)
{
    int prev = 0;
    int j = 0;
    for (int i = 0; i < SIZE; ++i) {
        if (a[i]) {
            if (a[i] == prev) {
                b[j - 1] *= 2;
                prev = 0;
            } else {
                b[j++] = a[i];
                prev = a[i];
            }
        }
    }
}

void fall(struct game* g)
{
    struct game g2 = {};
    for (int i = 0; i < SIZE; ++i)
        fall_column(g->tiles[i], g2.tiles[i]);
    *g = g2;
}

int same(struct game* a, struct game* b)
{
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (a->tiles[i][j] != b->tiles[i][j])
                return 0;
    return 1;
}

int tryfalling(struct game* g)
{
    struct game g2 = *g;
    fall(g);
    if (same(g, &g2))
        return 0;
    return 1;
}

void popup(struct game* g)
{
    int i, j;
    while (1) {
        i = random_spot(g);
        j = random_spot(g);
        if (!g->tiles[i][j]) {
            g->tiles[i][j] = random_value(g);
            return;
        }
    }
}

void move(struct game* g, int way)
{
    if (way / 2)
        twist(g);
    if (way % 2)
        flip(g);
    if (tryfalling(g))
        popup(g);
    if (way % 2)
        flip(g);
    if (way / 2)
        twist(g);
}

int read_move(void)
{
    char keys[MOVES] = {'k','i','j','l'};
    int c;
    int i;
    while (isspace(c = getchar()));
    if (c == EOF)
        return c;
    for (i = 0; i < MOVES; ++i)
        if (c == keys[i])
            return i;
    return EOF;
}

static struct termios backup;
static struct termios current;

void take_stdin(void)
{
    tcgetattr(STDIN_FILENO, &backup);
    current = backup;
    current.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &current);
}

void give_stdin(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &backup);
}

void update_score(struct game* g, int old_score)
{
    int new_score = 0;
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (g->tiles[i][j] != 0)
                new_score += g->tiles[i][j];

    if (new_score != old_score)
        printf("\033[38;5;1mSCORE: %d\033[0m\n", new_score);
}

int game_over(struct game* g)
{
    for (int i = 0; i < SIZE - 1; ++i)
        for (int j = 0; j < SIZE - 1; ++j)
            if (g->tiles[i][j] == g->tiles[i+1][j] || g->tiles[i][j] == g->tiles[i][j+1])
                return 0;
    return 1;
}


int main()
{
    int c;
    struct game g = { .score = 0 };
    begin(&g);
    print(&g);
    update_score(&g, g.score);
    take_stdin();
    while ((c = read_move()) != EOF) {
        int old_score = g.score;
        move(&g, c);
        print(&g);
        update_score(&g, old_score);

        if (game_over(&g)) {
            printf("\033[38;5;1mGAME OVER!\033[0m\n");
            printf("\033[38;5;1mSCORE: %d\033[0m\n", g.score);
            break;
        }
    }
    give_stdin();
    return 0;
}
