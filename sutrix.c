#define _XOPEN_SOURCE_EXTENDED 1

#include <curses.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

#define KATAKANA_START 0x30A0
#define KATAKANA_END   0x30FF
#define KATAKANA_COUNT (KATAKANA_END - KATAKANA_START + 1)

#define FADE_TIME       0.50
#define HEAD_TIME       0.15
#define SPEED_STEP      0.01
#define MIN_SPEED       0.01
#define MAX_SPEED       0.30

static const wchar_t symbols[] = L"!@#$%^&*()_+-=[]{}|;':\",./<>?";
static const wchar_t numbers[] = L"0123456789";

typedef struct {
    wchar_t ch;
    double time;
} Cell;

static double
get_time(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
}

static int
random_range(int min, int max)
{
    return min + rand() % (max - min + 1);
}

static wchar_t
random_char(void)
{
    int r = rand() % 100;

    if (r < 75)
        return KATAKANA_START + rand() % KATAKANA_COUNT;

    if (r < 90)
        return symbols[rand() % (sizeof(symbols) / sizeof(symbols[0]) - 1)];

    return numbers[rand() % (sizeof(numbers) / sizeof(numbers[0]) - 1)];
}

int
main(void)
{
    setlocale(LC_ALL, "");
    srand((unsigned)time(NULL));

    WINDOW *win = initscr();

    if (!win)
        return 1;

    cbreak();
    noecho();
    curs_set(0);
    nodelay(win, TRUE);
    keypad(win, TRUE);

    start_color();
    use_default_colors();

    init_pair(1, COLOR_WHITE, -1);
    init_pair(2, COLOR_RED, -1);
    
    if (COLORS > 245)
        init_pair(3, 245, -1);
    else
        init_pair(3, COLOR_WHITE, -1);

    int height, width;
    getmaxyx(win, height, width);

    int *heads = calloc((size_t)width, sizeof(*heads));
    Cell *trail = calloc((size_t)height * width, sizeof(*trail));

    if (!heads || !trail) {
        free(heads);
        free(trail);
        endwin();
        return 1;
    }

    for (int x = 0; x < width; x++)
        heads[x] = random_range(-height, 0);

    for (int i = 0; i < height * width; i++)
        trail[i].time = -1.0;

    int color_mode = 1;
    double delay = 0.07;
    int running = 1;

    while (running) {
        double current = get_time();
        int key = wgetch(win);

        if (key != ERR) {
            switch (key) {
            case '1':
                color_mode = 1;
                break;

            case '2':
                color_mode = 2;
                break;

            case '3':
                color_mode = 3;
                break;

            case '4':
                delay -= SPEED_STEP;
                if (delay < MIN_SPEED)
                    delay = MIN_SPEED;
                break;

            case '5':
                delay += SPEED_STEP;
                if (delay > MAX_SPEED)
                    delay = MAX_SPEED;
                break;

            case 'q':
            case 'Q': {
                int cx = width / 2;
                int cy = height / 2;
                const char *msg = "Y/N to leave: ";

                nodelay(win, FALSE);

                attron(COLOR_PAIR(1) | A_REVERSE);
                mvaddstr(cy, cx - 7, msg);
                attroff(COLOR_PAIR(1) | A_REVERSE);
                refresh();

                for (;;) {
                    int confirm = wgetch(win);

                    if (confirm == 'y' || confirm == 'Y') {
                        running = 0;
                        break;
                    }

                    if (confirm == 'n' || confirm == 'N') {
                        nodelay(win, TRUE);
                        break;
                    }
                }

                break;
            }
            }
        }

        if (!running)
            break;

        erase();

        for (int x = 0; x < width; x++) {
            if ((double)rand() / ((double)RAND_MAX + 1.0) > 0.1)
                continue;

            int y = heads[x];

            if (y >= 0 && y < height) {
                Cell *cell = &trail[(size_t)y * width + x];

                cell->ch = random_char();
                cell->time = current;
            }

            heads[x]++;

            if (heads[x] >= height + 10)
                heads[x] = random_range(-10, 0);
        }

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Cell *cell = &trail[(size_t)y * width + x];

                if (cell->time < 0.0)
                    continue;

                double age = current - cell->time;

                if (age > FADE_TIME) {
                    cell->time = -1.0;
                    continue;
                }

                wchar_t text[2] = { cell->ch, L'\0' };
                cchar_t character;

                if (age < HEAD_TIME) {
                    short pair = 1;

                    if (color_mode == 2)
                        pair = (rand() & 1) ? 1 : 2;
                    else if (color_mode == 3)
                        pair = 2;

                    setcchar(&character, text, A_BOLD, pair, NULL);
                } else {
                    setcchar(&character, text, A_NORMAL, 3, NULL);
                }

                mvadd_wch(y, x, &character);
            }
        }

        refresh();

        struct timespec sleep_time = {
            .tv_sec = (time_t)delay,
            .tv_nsec = (long)((delay - (time_t)delay) * 1000000000L)
        };

        nanosleep(&sleep_time, NULL);
    }

    free(heads);
    free(trail);

    endwin();
    return 0;
}

// i am a stinky furry and im not joking i am a furry but you have to be miserable to read this and hate on it
