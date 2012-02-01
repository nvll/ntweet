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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <unistd.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <pthread.h>
#include <curses.h>
#include <signal.h>
#include <time.h>

#include "ntweet.h"
#include "xml_parser.h"
#include "twitter_api.h"
#include "ui.h"

GSList *tweet_list = NULL;
screen_t screen = {100, 50, 0, NULL};
WINDOW *ui, *mainw;


void resize_callback(int signal) {
    endwin();
    refresh();
    getmaxyx(stdscr, screen.y, screen.x);
    draw_list(tweet_list, &screen);
}

/* Primary child thread work is done here. Mainly reading from twitter and
 * turning it into a GSList for the main thread */ 
void child_worker () {
    time_t cur_time;
    char *xml_feed = NULL;
    CURL *curl_handle = curl_easy_init();
    
	while (1) {
        xml_feed = get_twitter_updates(curl_handle, tweet_list);
        if (!xml_feed) { 
            screen.status = "Couldn't connect to server";
            goto csleep;
        }
        time(&cur_time);
        screen.status = g_strconcat("Last update ", ctime(&cur_time), NULL);

        xml_parse_twitter(xml_feed, tweet_list);
csleep:
        draw_list(tweet_list, &screen);
        sleep(60);
    }
}

int main (int argc, char *argv[]) {
    pthread_t child;
    int rc;

    DEBUGF("Initializing libraries / etc\n");
    LIBXML_TEST_VERSION;
    
	initscr();
    getmaxyx(stdscr, screen.y, screen.x);
    noecho();
    curs_set(0);
    DEBUGF("window dimensions: %i x %i\n", screen.x, screen.y);
    keypad(stdscr, 1);

    DEBUGF("spawning child\n");
    rc = pthread_create(&child, NULL, (void *) child_worker, NULL);
    if (rc) {
        printf("Failed to spawn a thread\n");
        return -1;
    }
    signal(SIGWINCH, &resize_callback);
    int input;
    while (1) {
        input = getch();
        switch (input) {
            case 'q':
                goto exit;
            case 'v':
                endwin();
                break; 
        }
        usleep(1);
    }

    exit:

    // XML cleanup methods
    endwin();
    xmlCleanupParser();
    xmlMemoryDump();
    return 0;
}
