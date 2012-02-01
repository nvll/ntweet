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
