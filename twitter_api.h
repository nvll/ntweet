#ifndef __NTWEET_TWITTER_API_H
#define __NTWEET_TWITTER_API_H
#include <curl/types.h>
#include <glib.h>

size_t get_curl_data(void *, size_t, size_t, char **);
char *get_twitter_updates(CURL *curl_handle, GSList *tweet_list);

#endif
