#ifndef __NTWEET_XML_PARSER_H
#define __NTWEET_XML_PARSER_H
#include <glib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "ntweet.h"

void xml_parse_twitter (char *, GSList *tweet_list);
tweet_info_t *xml_parse_status (xmlDocPtr, xmlNode *);
user_info_t *xml_parse_user (xmlDocPtr, xmlNode *);
void xml_extract_error(xmlDocPtr, xmlNode *);
void dbg_print_tweet (tweet_info_t *, void *);
int find_tweet_by_id (tweet_info_t *, char *); 
int tweet_id_compare(tweet_info_t *first, tweet_info_t *second);

#endif
