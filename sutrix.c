#define _XOPEN_SOURCE_EXTENDED 1
#include <curses.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

#define KATAKANA_START 0x30A0
#define KATAKANA_END   0x30FF
#define KATAKANA_COUNT (KATAKANA_END - KATAKANA_START)

static const wchar_t SYMBOLS[] = L"!@#$%^&*()_+-=[]{}|;':\",./<>?";
static const wchar_t NUMBERS[] = L"0123456789";

typedef struct {
    wchar_t ch;
    double  timestamp;
} Cell;

typedef struct {
    int head_y;
} Column;

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static double frand(void) {
    return (double)rand() / ((double)RAND_MAX + 1.0);
}

static int rand_range(int lo, int hi_inclusive) {
    if (hi_inclusive < lo) return lo;
    return lo + rand() % (hi_inclusive - lo + 1);
}

static wchar_t get_random_char(void) {
    int roll = rand() % 100;
    if (roll < 75) {
        return (wchar_t)(KATAKANA_START + rand() % KATAKANA_COUNT);
    } else if (roll < 90) {
        int n = (int)(sizeof(SYMBOLS) / sizeof(wchar_t)) - 1;
        return SYMBOLS[rand() % n];
    } else {
        int n = (int)(sizeof(NUMBERS) / sizeof(wchar_t)) - 1;
        return NUMBERS[rand() % n];
    }
}

int main(void) {
    setlocale(LC_ALL, "");
    srand((unsigned)time(NULL));

    WINDOW *win = initscr();
    curs_set(0);
    nodelay(win, TRUE);
    noecho();
    cbreak();
    keypad(win, TRUE);

    start_color();
    use_default_colors();
    init_pair(1, COLOR_WHITE, -1);
    init_pair(2, COLOR_RED, -1);
    init_pair(3, 245, -1);

    int max_y, max_x;
    getmaxyx(win, max_y, max_x);

    Column *columns = calloc((size_t)max_x, sizeof(Column));
    Cell *trails = calloc((size_t)max_y * (size_t)max_x, sizeof(Cell));
    for (int x = 0; x < max_x; x++) {
        columns[x].head_y = rand_range(-max_y, 0);
    }
    for (int i = 0; i < max_y * max_x; i++) {
        trails[i].timestamp = -1.0;
    }
    #define TRAIL(y, x) trails[(size_t)(y) * (size_t)max_x + (size_t)(x)]

    int color_mode = 1;
    double base_delay = 0.07;

    int running = 1;
    while (running) {
        double current_time = now_seconds();
        int ch = wgetch(win);

        if (ch != ERR) {
            if (ch == 'q' || ch == 'Q') {
                nodelay(win, FALSE);
                const char *msg = "Y/N to leave: ";
                int mlen = (int)strlen(msg);
                mvaddstr(max_y / 2, (max_x - mlen) / 2, msg);
                attron(COLOR_PAIR(1) | A_REVERSE);
                mvaddstr(max_y / 2, (max_x - mlen) / 2, msg);
                attroff(COLOR_PAIR(1) | A_REVERSE);
                refresh();
                for (;;) {
                    int confirm = wgetch(win);
                    if (confirm == 'y' || confirm == 'Y') {
                        running = 0;
                        break;
                    } else if (confirm == 'n' || confirm == 'N') {
                        nodelay(win, TRUE);
                        break;
                    }
                }
            } else if (ch == '1') {
                color_mode = 1;
            } else if (ch == '2') {
                color_mode = 2;
            } else if (ch == '3') {
                color_mode = 3;
            } else if (ch == '4') {
                base_delay -= 0.01;
                if (base_delay < 0.01) base_delay = 0.01;
            } else if (ch == '5') {
                base_delay += 0.01;
                if (base_delay > 0.3) base_delay = 0.3;
            }
        }

        if (!running) break;

        erase();

        for (int x = 0; x < max_x; x++) {
            if (frand() > 0.1) {
                int head_y = columns[x].head_y;
                if (head_y >= 0 && head_y < max_y) {
                    TRAIL(head_y, x).ch = get_random_char();
                    TRAIL(head_y, x).timestamp = current_time;
                }

                columns[x].head_y += 1;
                if (columns[x].head_y >= max_y + 10) {
                    columns[x].head_y = rand_range(-10, 0);
                }
            }

            for (int y = 0; y < max_y; y++) {
                Cell *cell = &TRAIL(y, x);
                if (cell->timestamp < 0.0) continue;

                double age = current_time - cell->timestamp;
                if (age > 0.5) {
                    cell->timestamp = -1.0;
                    continue;
                }

                wchar_t wch[2] = { cell->ch, L'\0' };
                cchar_t cc;

                if (age < 0.15) {
                    int pair;
                    if (color_mode == 1) {
                        pair = 1;
                    } else if (color_mode == 2) {
                        pair = (rand() % 2 == 0) ? 1 : 2;
                    } else {
                        pair = 2;
                    }
                    setcchar(&cc, wch, A_BOLD, (short)pair, NULL);
                } else {
                    setcchar(&cc, wch, A_NORMAL, 3, NULL);
                }
                mvadd_wch(y, x, &cc);
            }
        }

        refresh();

        struct timespec req;
        req.tv_sec = (time_t)base_delay;
        req.tv_nsec = (long)((base_delay - (double)req.tv_sec) * 1e9);
        nanosleep(&req, NULL);
    }

    free(columns);
    free(trails);
    endwin();
    return 0;
}
/* lets all love lain & sunia */
