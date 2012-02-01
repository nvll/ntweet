#include <curses.h>
#include <glib.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include "ui.h"

pthread_mutex_t window_lock = PTHREAD_MUTEX_INITIALIZER;

/* Draws an individual username / tweet combo on the screen */
void draw_element(tweet_info_t *tweet, void *userdata) {
    uint32_t pos, x, row = 1;
	screen_t *screen = userdata;
    attron(A_BOLD);
    mvprintw(row++, 0, "%15s: ", tweet->user->sname);
    attroff(A_BOLD);

    /* The fun part begins here. We need to align the tweet body, so to do
     * that we're first checking if it takes 2 lines, then splitting it if it
     * does */
    if (strlen(tweet->text) < (screen->x-16)) {
        printw("%s ", tweet->text);
    } else {
        /* Find where we can split the lines evenly */
        for (pos = screen->x-17; pos > 0; pos--) {
            if (tweet->text[pos] == ' ')
                break;
        }
        for (x = 0; x < pos; x++) {
            addch((tweet->text)[x]);
        }
        move(row++, 16);
        for (x = pos; x < strlen(tweet->text); x++) {
            addch((tweet->text)[x]);
        }
    }
}

/* Primary display function, renders the window */
void draw_list(GSList *tweet_list, screen_t *screen) {
	pthread_mutex_lock(&window_lock);
    erase();

    g_slist_foreach(tweet_list, (GFunc) draw_element, screen);
    
    print_header("nTweet");
    print_status(screen);
    refresh();
	pthread_mutex_unlock(&window_lock);
}

/* Renders the status message based on a global string */
void print_status(screen_t *screen) {
    attron(A_STANDOUT);
    mvprintw(screen->y-1, 0, " %-*s", screen->x, screen->status);
    attroff(A_STANDOUT);
}

/* Renders the top header */
void print_header(screen_t *screen) {
    attron(A_STANDOUT | A_BOLD);
    mvprintw(screen->selected, 0, "%-*s%s%-*s", (screen->x/2)-3, "", "ntweet", (screen->x/2)-3, "");
    attroff(A_STANDOUT | A_BOLD);
}
