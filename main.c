#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <unistd.h>
#include "curl/curl.h"
#include "curl/types.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <pthread.h>
#include <curses.h>
#include <signal.h>
#include <time.h>

#include "global.h"
#include "xml_parser.h"
#include "twitter_api.h"
#include "ui.h"

void resize_callback(int signal) {
    endwin();
    refresh();
    getmaxyx(stdscr, screen_y, screen_x);
    //drawList();
}

/* Primary child thread work is done here. Mainly reading from twitter and
 * turning it into a GSList for the main thread */ 
void child_worker (void *ptr, size_t size, size_t nmemb, void *config) {
    time_t cur_time;
    char *xml_feed = NULL;
    cHandle = curl_easy_init();
    int x = 0;
    while (1) {
        xml_feed = get_twitter_updates();
        if (!xml_feed) { 
            statusMsg = "Couldn't connect to server";
            goto csleep;
        }
        time(&cur_time);
        statusMsg = g_strconcat("Last update ", ctime(&cur_time), NULL);

        xml_parse_twitter(xml_feed);
csleep:
        drawList(selected);
        sleep(60);
    }
}

int main (int argc, char *argv[]) {
    pthread_t child;
    int rc;

    Debug("Setting up signals\n");

    Debug("Initializing libraries / etc\n");
    LIBXML_TEST_VERSION;
    cHandle = curl_easy_init();
    initscr();
    getmaxyx(stdscr, screen_y, screen_x);
    noecho();
    curs_set(0);
    Debug("Window dimensions: %i x %i\n", screen_x, screen_y);
    keypad(stdscr, 1);

    Debug("Spawning child\n");
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
