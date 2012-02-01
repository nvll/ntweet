/*
 * Copyright (c) 2012, Christopher Anderson
 * All rights reserved.
 *  
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies, 
 * either expressed or implied, of the FreeBSD Project.
 */
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
