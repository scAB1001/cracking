/* REFERENCE
 * https://github.com/abuharth/snake-term/tree/main
 * $ sudo apt-get install libncurses5-dev libncursesw5-dev
 */

#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>
#include <string.h>

#define MAX_SCORE 100
#define FRAME_TIME 110000
#define SCORE_OUTPUT sprintf(score_message, "[ Score: %d ]", score)

typedef struct
{
    int x;
    int y;
} vec2;

uint8_t score = 0;
char score_message[16];

bool skip = false;
bool is_running = true;

uint8_t screen_width = 25;
uint8_t screen_height = 20;

// initialize screen
WINDOW *win;

// snake
vec2 head = {0, 0};
vec2 segments[MAX_SCORE + 1];
vec2 dir = {1, 0};
// berry
vec2 berry;

bool collide(vec2 a, vec2 b)
{
    return (a.x == b.x && a.y == b.y);
}

bool collide_snake_body(vec2 point)
{
    for (uint8_t i = 0; i < score; i++)
    {
        if (collide(point, segments[i]))
        {
            return true;
        }
    }
    return false;
}

vec2 spawn_berry()
{
    uint8_t vp_width = screen_width - 2;
    uint8_t vp_height = screen_height - 2;
    // spawn a new berry with 1 pixel padding from edges and not inside of the snake
    vec2 berry = {1 + rand() % vp_width, 1 + rand() % vp_height};
    while (collide(head, berry) || collide_snake_body(berry))
    {
        berry.x = 1 + rand() % vp_width;
        berry.y = 1 + rand() % vp_height;
    }
    return berry;
}

void draw_border(uint8_t y, uint8_t x, uint8_t width, uint8_t height)
{
    // top row
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + width * 2 + 1, ACS_URCORNER);
    for (uint8_t i = 1; i < width * 2 + 1; i++)
    {
        mvaddch(y, x + i, ACS_HLINE);
    }
    // vertical lines
    for (uint8_t i = 1; i < height + 1; i++)
    {
        mvaddch(y + i, x, ACS_VLINE);
        mvaddch(y + i, x + width * 2 + 1, ACS_VLINE);
    }
    // bottom row
    mvaddch(y + height + 1, x, ACS_LLCORNER);
    mvaddch(y + height + 1, x + width * 2 + 1, ACS_LRCORNER);
    for (uint8_t i = 1; i < width * 2 + 1; i++)
    {
        mvaddch(y + height + 1, x + i, ACS_HLINE);
    }
}

void quit_game()
{
    // exit cleanly from application
    endwin();
    // clear screen, place cursor on top, and un-hide cursor
    printf("\e[1;1H\e[2J");
    printf("\e[?25h");

    exit(0);
}

void restart_game()
{
    head.x = 0;
    head.y = 0;
    dir.x = 0x1;
    dir.y = 0;
    score = 0;
    SCORE_OUTPUT;
    is_running = true;
}

void init()
{
    srand(time(NULL));
    // initialize window
    win = initscr();
    // take player input and hide cursor
    keypad(win, true);
    noecho();
    nodelay(win, true);
    curs_set(0);

    // initialize color
    if (has_colors() == FALSE)
    {
        endwin();
        fprintf(stderr, "Your terminal does not support color\n");
        exit(1);
    }
    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_GREEN, -1);
    init_pair(3, COLOR_YELLOW, -1);

    berry.x = rand() % screen_width;
    berry.y = rand() % screen_height;

    // update score message
    SCORE_OUTPUT;
}

void process_input()
{
    int pressed = wgetch(win);
    switch( pressed )
    {
        case KEY_LEFT:
        {
            if (dir.x == 1)
            {
                return;
                skip = true;
            }
            dir.x = -1;
            dir.y = 0;
            break;
        }
        case KEY_RIGHT:
        {
            if (dir.x == -1)
            {
                return;
                skip = true;
            }
            dir.x = 1;
            dir.y = 0;
            break;
        }
        case KEY_UP:
        {
            if (dir.y == 1)
            {
                return;
                skip = true;
            }
            dir.x = 0;
            dir.y = -1;
            break;
        }
        case KEY_DOWN:
        {
            if (dir.y == -1)
            {
                return;
                skip = true;
            }
            dir.x = 0;
            dir.y = 1;
            break;
        }
        case ' ':
        {
            if (!is_running)
                restart_game();
            break;
        }
        case '\e':
        {
            is_running = false;
            quit_game();
            break;
        }
    }
}

void game_over()
{
    while (is_running == false)
    {
        process_input();
        // TODO: Make method for getting centre
        // Prints in center of screen
        mvaddstr(screen_height / 2, screen_width - 16, "              Game Over          ");
        mvaddstr(screen_height / 2 + 1, screen_width - 16, "[SPACE] to restart, [ESC] to quit ");
        attron(COLOR_PAIR(3));
        draw_border(screen_height / 2 - 1, screen_width - 17, 17, 2);
        attroff(COLOR_PAIR(3));

        usleep(FRAME_TIME);
    }
}

void update()
{
    // update snake segments
    for (uint8_t i = score; i > 0; i--)
    {
        segments[i] = segments[i - 1];
    }
    segments[0] = head;

    // move snake
    head.x += dir.x;
    head.y += dir.y;

    // collide with body or walls
    if (collide_snake_body(head) || head.x < 0 || head.y < 0 || head.x >= screen_width || head.y >= screen_height)
    {
        is_running = false;
        game_over();
    }

    // eating a berry
    if (collide(head, berry))
    {
        if (score < MAX_SCORE)
        {
            score++;
            SCORE_OUTPUT;
        }
        else
        {
            // WIN!
            printf("You Win!");
        }
        berry = spawn_berry();
    }

    usleep(FRAME_TIME);
}

void draw()
{
    erase();

    attron(COLOR_PAIR(1));
    mvaddch(berry.y + 1, berry.x * 2 + 1, '@');
    attroff(COLOR_PAIR(1));

    // draw snake
    attron(COLOR_PAIR(2));
    for (uint8_t i = 0; i < score; i++)
    {
        mvaddch(segments[i].y + 1, segments[i].x * 2 + 1, ACS_DIAMOND);
    }
    mvaddch(head.y + 1, head.x * 2 + 1, 'O');
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(3));
    draw_border(0, 0, screen_width, screen_height);
    attroff(COLOR_PAIR(3));
    mvaddstr(0, screen_width - 5, score_message);
}

int main(int argc, char *argv[])
{
    // process user args
    if (argc == 1)
    {
    }
    else if (argc == 3)
    {
        if (!strcmp(argv[1], "-d"))
        {
            if (sscanf(argv[2], "%c%c", &screen_width, &screen_height) != 2)
            {
                printf("Usage: snake [options]\nOptions:\n -d [width]x[height]"
                       "\tdefine dimensions of the screen\n\nDefault dimensions are 25x20\n");
                exit(1);
            }
        }
    }
    else
    {
        printf("Usage: snake [options]\nOptions:\n -d [width]x[height]"
               "\tdefine dimensions of the screen\n\nDefault dimensions are 25x20\n");
        exit(1);
    }

    init();
    while (is_running)
    {
        process_input();
        if (skip)
        {
            skip = false;
            continue;
        }

        update();
        draw();
    }
    quit_game();
    return 0;
}