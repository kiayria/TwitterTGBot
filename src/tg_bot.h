#ifndef TWITTERTGBOT_TG_BOT_H
#define TWITTERTGBOT_TG_BOT_H

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <cstring>

#include <tgbot/tgbot.h>
#include <twitterlib/twitterlib.hpp>

class tg_bot {

public:
    tg_bot();
    ~tg_bot();
    void start();

private:
    std::array<std::string, 6> command_list = {
            "start",
            "new",
            "tweets_feed",
            "login",
            "quit",
            "help"
    };

    std::string token = "1206584369:AAEuzd5ax4KB_aMfh5A54nQsQ7Rb1maed7k";
};

    std::string trim_string(std::string tweet);
#endif //TWITTERTGBOT_TG_BOT_H
