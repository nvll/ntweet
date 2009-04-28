void xml_parse_twitter (char *);
struct tweetInfo *xml_parse_status (xmlDocPtr, xmlNode *);
struct userInfo *xml_parse_user (xmlDocPtr, xmlNode *);
void xml_extract_error(xmlDocPtr, xmlNode *);
void dbg_print_tweet (struct tweetInfo *, void *);
int find_tweet_by_id (struct tweetInfo *, char *); 
int tweet_id_compare(struct tweetInfo *first, struct tweetInfo *second);

/* Custom find function for g_slist_find_custom */
int find_tweet_by_id (struct tweetInfo *node, char *id) {
    if (0 == strcmp(node->id, id))  {
        return 0;
    }
    return -1;
}

/* Custom comparison function for g_slist_insert_sorted. Otherwise if we use
 * just prepend or append the data is out of order when new xml feeds are
 * obtained */
int tweet_id_compare(struct tweetInfo *first, struct tweetInfo *second) {
    int fir = atoi(first->id);
    int sec = atoi(second->id);
    if (fir < sec)
        return 1;
    else if (fir == sec)
        return 0;
    else
        return -1;

}

void dbg_print_tweet (struct tweetInfo *tweet, void *userdata) {
    if (!tweet)
        return;
    Debug("Tweet[%s]: %s -- %s\n",
        tweet->id,
        tweet->user->sname,
        tweet->text
    );
}

void xml_parse_twitter (char *buffer) {
    struct tweetInfo *tweet = NULL;
    GSList *tmp = NULL;
    xmlNode *current = NULL, *root_node = NULL;
    xmlDocPtr xml = NULL;
    
    xml = xmlReadMemory(buffer, strlen(buffer), "none", NULL, 0);
    if (!xml)
        return; 
    root_node = xmlDocGetRootElement(xml);
    current = root_node->xmlChildrenNode;
    while (0 != current) {
        // Check for a new status. If the first node is a hash then we have
        // an error and won't have any statuses
        if (0 == xmlStrcmp(current->name, (const xmlChar *) "hash")) {
            xml_extract_error(xml, current->xmlChildrenNode);
            break;
        } else if (0 == xmlStrcmp(current->name, (const xmlChar *) "status")) {
            // Only add a tweet if it isn't in the list already
            tweet = xml_parse_status(xml, current->xmlChildrenNode);
            if (0 == g_slist_find_custom(lstTweets, tweet->id, (GCompareFunc) find_tweet_by_id)) {
                Debug("Tweet %s is new, appending\n", tweet->id);
                lstTweets = g_slist_insert_sorted(lstTweets, tweet, (GCompareFunc) tweet_id_compare);
            }
            // Prune tail if > 20 tweets
            if (g_slist_length(lstTweets) > 50) {
                tmp = g_slist_last(lstTweets);
                lstTweets = g_slist_remove(lstTweets, tmp->data);
                Debug("Exceeded 20 entries, dropping tail\n");
            }
        }
        current = current->next;
    }
}

/* Parses an error message out of an xml structure */
void xml_extract_error(xmlDocPtr xml, xmlNode *node) {
}

/* Parses out the fields of a single status hierarchy */
struct tweetInfo *xml_parse_status (xmlDocPtr xml, xmlNode *node) {
    struct tweetInfo *tweet = malloc(sizeof(struct tweetInfo));
    if (!tweet) 
        return NULL;
    while (node) {
        // Tweet bodies are in a field called "text", which also is the name
        // of the text container in any node, so checking the type is
        // necessary
        if (0 == xmlStrcmp(node->name, (const xmlChar *) "text") && node->type == XML_ELEMENT_NODE)
            tweet->text = (char *) xmlNodeListGetString(xml, node->xmlChildrenNode, 1);
        if (0 == xmlStrcmp(node->name, (const xmlChar *) "created_at"))
            tweet->datetime = (char *) xmlNodeListGetString(xml, node->xmlChildrenNode, 1);
        if (0 == xmlStrcmp(node->name, (const xmlChar *) "id"))
            tweet->id = (char *) xmlNodeListGetString(xml, node->xmlChildrenNode, 1);
        if (0 == xmlStrcmp(node->name, (const xmlChar *) "user"))
            tweet->user = xml_parse_user (xml, node->xmlChildrenNode);
        node = node->next;
    }
    return tweet;
}

struct userInfo *xml_parse_user (xmlDocPtr xml, xmlNode *node) {
    struct userInfo *user = malloc(sizeof(struct userInfo));
    if (!user)
        return NULL;
    while (node) {
        if (0 == xmlStrcmp(node->name, (const xmlChar *) "id"))
            user->id = (char *) xmlNodeListGetString(xml, node->xmlChildrenNode, 1);
        if (0 == xmlStrcmp(node->name, (const xmlChar *) "name"))
            user->name = (char *) xmlNodeListGetString(xml, node->xmlChildrenNode, 1);
        if (0 == xmlStrcmp(node->name, (const xmlChar *) "screen_name"))
            user->sname = (char *) xmlNodeListGetString(xml, node->xmlChildrenNode, 1);
        if (0 == xmlStrcmp(node->name, (const xmlChar *) "location"))
            user->location = (char *) xmlNodeListGetString(xml, node->xmlChildrenNode, 1);
        node = node->next;
    }
    return user;
}
