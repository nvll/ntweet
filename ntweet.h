#ifndef __NTWEET_H
#define __NTWEET_H
#include <stdint.h>

#define DEBUG 0
#define DEBUGF(x...) do { if (DEBUG) { fprintf(stderr, x); } } while(0)

struct user_info {
    char *name;
    char *sname;
    char *id;
    char *location;
};

struct tweet_info {
    struct user_info *user;
    char *text;
    char *datetime;
    char *id;
};

struct screen {
	uint32_t x;
	uint32_t y;
	int selected;
	char *status;
};

typedef struct tweet_info tweet_info_t;
typedef struct user_info user_info_t;
typedef struct screen screen_t;
#endif
