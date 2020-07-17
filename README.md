# TwitterTGBot

This bot shows latest tweets from user's timeline and allows user to send tweets via Telegram.

##
Add your Twitter API consumer keys and secrets to the text file "keys".
##

#### Using libraries:
 + [Twitter](https://github.com/a-n-t-h-o-n-y/Twitter-API-C-Library/tree/master) - using as a submodule
 + [Telegram](https://github.com/reo7sp/tgbot-cpp) - using as s submodule
 + [Mongo db](https://hub.docker.com/_/mongo)
 + [Mongo driver](http://mongocxx.org/) 


## Using code:
    git clone https://github.com/kiayria/TwitterTGBot.git
    cd TwitterTGBot/
    git submodule init
    git submodule update
    mkdir build
    cmake . ./build && make
    ./TwitterTGBot
    
After add key for Twitter api and telegram bot token

### Maintainers
-   [Karina Maikenova](https://github.com/kiayria)
-   [Sergey Ovchinnikov](https://github.com/ovc-serega)
