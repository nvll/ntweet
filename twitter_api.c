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

