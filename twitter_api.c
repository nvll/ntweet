#include <curl/curl.h>
#include <curl/types.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include "ui.h"
#include "xml_parser.h"

/* Called each time curl fetches a chunk of data, so keep expanding the
 * buffer and appending the new data to it */
size_t get_curl_data(void *ptr, size_t size, size_t nmemb, char **buffer) {
    // For the first call buffer won't be allocated
    if (!(*buffer)) {
        *buffer = strdup((char *)ptr);
    } else {
        // Easier to let glib handle the memory allocation for this
        *buffer = g_strconcat(*buffer, (char *) ptr, NULL);
    }
    DEBUGF("child: Received %i bytes\n", (int)(size*nmemb));
    return size * nmemb;
}

char *get_twitter_updates(CURL *curl_handle, GSList *tweet_list) {
    int ret;
    char *buffer = NULL;
    char *last_update = NULL, *api_path, *base_url = "http://twitter.com/statuses/friends_timeline.xml?count=50";
    tweet_info_t *tmp;

    if (tweet_list != NULL) {
        tmp = g_slist_nth_data(tweet_list, 0);
        last_update = tmp->id; 
    }
    if (!last_update)
        api_path = base_url;
    else
        api_path = g_strconcat(base_url, "&since_id=", last_update, NULL);
    //api_path = "http://twitter.com/statuses/public_timeline.xml";
    DEBUGF("api path: %s\n", api_path);
    curl_easy_setopt(curl_handle, CURLOPT_USERPWD, "usernamehere:password");
    curl_easy_setopt(curl_handle, CURLOPT_URL, api_path);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, get_curl_data);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &buffer);

    // Main child loop
    DEBUGF("fetching data\n");
    ret = curl_easy_perform(curl_handle);
    
	DEBUGF("received %i bytes total\n", (int) strlen(buffer));
    DEBUGF("buffer Received: \n%s\n", buffer);
    return buffer;
}

