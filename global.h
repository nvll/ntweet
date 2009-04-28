#ifdef DEBUG
#define Debug(...) fprintf(stderr, "Thread %i, %s:%i ", (int) pthread_self(), __FUNCTION__, __LINE__), fprintf(stderr, __VA_ARGS__);
#else
#define Debug(...)
#endif

struct userInfo {
    char *name;
    char *sname;
    char *id;
    char *location;
};

struct tweetInfo {
    struct userInfo *user;
    char *text;
    char *datetime;
    char *id;
};

GSList *lstTweets = NULL;
CURL *cHandle;
int newUpdate = 0, screen_x = 100, screen_y = 50, selected=0;
WINDOW *ui, *mainw;
pthread_mutex_t threadlock = PTHREAD_MUTEX_INITIALIZER;
char *statusMsg = "";
