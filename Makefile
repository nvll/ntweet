CC = gcc 
CFLAGS = -0s -W -Wall -std=c99 -Wno-unused-parameter \
	    `pkg-config libxml-2.0 glib-2.0 libcurl gthread-2.0 --cflags`
LIBFLAGS = `pkg-config libxml-2.0 glib-2.0 libcurl gthread-2.0 --libs` -lcurses
OUTPUT = ntweet
OBJDIR := build
DEFINES =
INCLUDE =
ECHO = @ 

OBJS := \
		main.o \
		ui.o \
		twitter_api.o \
		xml_parser.o


MKDIR = if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi

OBJS := $(addprefix $(OBJDIR)/, $(OBJS))

.PHONY: all

all: $(OBJS)
	$(ECHO) echo "linking $(OUTPUT)"
	$(ECHO) $(CC) $(LIBFLAGS) $^ -o $(OUTPUT)

$(OBJDIR)/%.o: %.c
	@$(MKDIR)
	$(ECHO) echo "compiling $<"
	$(ECHO) $(CC) $(DEFINES) $(CFLAGS) $(INCLUDE) $< -c -o $@
	

clean:
	rm -rf $(OBJDIR) $(OUTPUT)


