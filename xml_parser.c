#include <glib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include "ntweet.h"
#include "xml_parser.h"

/* Custom find function for g_slist_find_custom */
int find_tweet_by_id (tweet_info_t *node, char *id) {
    if (0 == strcmp(node->id, id))  {
        return 0;
    }
    return -1;
}

/* Custom comparison function for g_slist_insert_sorted. Otherwise if we use
 * just prepend or append the data is out of order when new xml feeds are
 * obtained */
int tweet_id_compare(tweet_info_t *first, tweet_info_t *second) {
    int fir = atoi(first->id);
    int sec = atoi(second->id);
    if (fir < sec)
        return 1;
    else if (fir == sec)
        return 0;
    else
        return -1;

}

void dbg_print_tweet (tweet_info_t *tweet, void *userdata) {
    if (!tweet)
        return;
    DEBUGF("Tweet[%s]: %s -- %s\n",
        tweet->id,
        tweet->user->sname,
        tweet->text
    );
}

void xml_parse_twitter (char *buffer, GSList *tweet_list) {
    tweet_info_t *tweet = NULL;
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
            if (0 == g_slist_find_custom(tweet_list, tweet->id, (GCompareFunc) find_tweet_by_id)) {
                DEBUGF("Tweet %s is new, appending\n", tweet->id);
                tweet_list = g_slist_insert_sorted(tweet_list, tweet, (GCompareFunc) tweet_id_compare);
            }
            // Prune tail if > 20 tweets
            if (g_slist_length(tweet_list) > 50) {
                tmp = g_slist_last(tweet_list);
                tweet_list = g_slist_remove(tweet_list, tmp->data);
                DEBUGF("Exceeded 20 entries, dropping tail\n");
            }
        }
        current = current->next;
    }
}

/* Parses an error message out of an xml structure */
void xml_extract_error(xmlDocPtr xml, xmlNode *node) {
}

/* Parses out the fields of a single status hierarchy */
tweet_info_t *xml_parse_status (xmlDocPtr xml, xmlNode *node) {
    tweet_info_t *tweet = malloc(sizeof(tweet_info_t));
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

user_info_t *xml_parse_user (xmlDocPtr xml, xmlNode *node) {
    user_info_t *user = malloc(sizeof(user_info_t));
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
