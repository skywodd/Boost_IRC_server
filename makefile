# Bootstraping eclipse makefile 

all: 
	$(MAKE) -C Debug all
	cp ./Debug/Boost_IRC_server.exe serveur_irc

clean: 
	$(MAKE) -C Debug clean

.PHONY: Debug