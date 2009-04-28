void drawList();
void drawElement(struct tweetInfo *, int *);
void printHeader();
void printStatus();

/* Draws an individual username / tweet combo on the screen */
void drawElement(struct tweetInfo *tweet, int *userdata) {
    int pos, x, row = (*userdata);
    attron(A_BOLD);
    mvprintw(row++, 0, "%15s: ", tweet->user->sname);
    attroff(A_BOLD);
    Debug("Printing Element %s on row %i\n", tweet->id, row); 
    printw("%s ", "YAAAARRRRRRRRRR");
    /* The fun part begins here. We need to align the tweet body, so to do
     * that we're first checking if it takes 2 lines, then splitting it if it
     * does */
    //if (strlen(tweet->text) < (screen_x-16)) {
    //    printw("%s ", tweet->text);
    //} else {
        /* Find where we can split the lines evenly */
      /*  for (pos = screen_x-17; pos > 0; pos--) {
            if (tweet->text[pos] == ' ')
                break;
        }
        for (x = 0; x < pos; x++) {
            addch((tweet->text)[x]);
        }
        move(row++, 16);
        for (x = pos; x < strlen(tweet->text); x++) {
            addch((tweet->text)[x]);
        }
    }*/
    (*userdata) = row;
}

/* Primary display function, renders the window */
void drawList() {
    erase();
    int y = 1;

    g_slist_foreach(lstTweets, (GFunc) drawElement, &y);
    
    printHeader("nTweet");
    printStatus();
    refresh();
}

/* Renders the status message based on a global string */
void printStatus() {
    attron(A_STANDOUT);
    mvprintw(screen_y-1, 0, " %-*s", screen_x, statusMsg);
    attroff(A_STANDOUT);
}

/* Renders the top header */
void printHeader() {
    attron(A_STANDOUT | A_BOLD);
    mvprintw(selected, 0, "%-*s%s%-*s", (screen_x/2)-3, "", "nTweet", (screen_x/2)-3, "");
    attroff(A_STANDOUT | A_BOLD);
}
