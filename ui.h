#ifndef __NTWEET_UI_H
#define __NTWEET_UI_H
#include <glib.h>
#include "ntweet.h"

void draw_element(tweet_info_t *tweet, void *userdata);
void draw_list(GSList *tweet_list, screen_t *screen);
void print_status(screen_t *screen);
void print_header();

#endif

