size_t get_curl_data(void *, size_t, size_t, char **);
char *get_twitter_updates();

extern void printHeader();
extern void printStatus();
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
    Debug("Child: Received %i bytes\n", (int)(size*nmemb));
    return size * nmemb;
}

char *get_twitter_updates() {
    int ret;
    char *buffer = NULL;
    char *last_update = NULL, *api_path, *base_url = "http://twitter.com/statuses/friends_timeline.xml?count=50";
    struct tweetInfo *tmp;

    if (lstTweets != NULL) {
        tmp = g_slist_nth_data(lstTweets, 0);
        last_update = tmp->id; 
    }
    if (!last_update)
        api_path = base_url;
    else
        api_path = g_strconcat(base_url, "&since_id=", last_update, NULL);
    //api_path = "http://twitter.com/statuses/public_timeline.xml";
    Debug("Api Path: %s\n", api_path);
    curl_easy_setopt(cHandle, CURLOPT_USERPWD, "nvll:asc11nUll");
    curl_easy_setopt(cHandle, CURLOPT_URL, api_path);
    curl_easy_setopt(cHandle, CURLOPT_WRITEFUNCTION, get_curl_data);
    curl_easy_setopt(cHandle, CURLOPT_WRITEDATA, &buffer);

    // Main child loop
    Debug("Fetching data\n");
    ret = curl_easy_perform(cHandle);
    if (ret != 0) { 
        printStatus("Error accessing twitter servers\n");
        return NULL;
    }
    Debug("Received %i bytes total\n", (int) strlen(buffer));
    Debug("Buffer Received: \n%s\n", buffer);
    return buffer;
}
