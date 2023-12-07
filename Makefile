CC=gcc
CFLAGS=-c -Wextra -g
LDFLAGS=-pthread
COMMONSOURCES=deck.c handCalculator.c
TABLESOURCES=tableManager.c dealer.c bouncer.c playerServant.c
PLAYERSOURCES=player.c
COMMONOBJECTS=$(COMMONSOURCES:.c=.o)
TABLEOBJECTS=$(TABLESOURCES:.c=.o)
PLAYEROBJECTS=$(PLAYERSOURCES:.c=.o)
TABLEEXECUTABLE=thePokerTable
PLAYEREXECUTABLE=pokerPlayer

all: $(COMMONSOURCES) $(TABLESOURCES) $(PLAYERSOURCES) $(TABLEEXECUTABLE) $(PLAYEREXECUTABLE)

# pull in dependency info for *existing* .o files
-include $(COMMONOBJECTS:.o=.d) $(TABLEOBJECTS:.o=.d) $(PLAYEROBJECTS:.o=.d)

# create the table executable
$(TABLEEXECUTABLE): $(TABLEOBJECTS) $(COMMONOBJECTS)
	$(CC) $(TABLEOBJECTS) $(COMMONOBJECTS) $(LDFLAGS) -o $@

# create the player executale
$(PLAYEREXECUTABLE): $(PLAYEROBJECTS) $(COMMONOBJECTS)
	$(CC) $(PLAYEROBJECTS) $(COMMONOBJECTS) $(LDFLAGS) -o $@

# compile all .c files into .o and .d files
.c.o: 
	$(CC) $(CFLAGS) $< -o $@
	$(CC) -MM $< > $*.d

# remove all built files
clean:
	rm -rf $(COMMONOBJECTS) $(TABLEOBJECTS) $(PLAYEROBJECTS) $(TABLEEXECUTABLE) $(PLAYEREXECUTABLE) *.d