#ifndef TWITTERTGBOT_TG_BOT_H
#define TWITTERTGBOT_TG_BOT_H

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <tgbot/tgbot.h>
#include <twitterlib/twitterlib.hpp>

class tg_bot {

public:
    tg_bot();
    ~tg_bot();

private:
    std::array<std::string, 5> command_list = {
            "start",
            "new",
            "tweets_feed",
            "login",
            "quit"
    };

    std::string token = "";




};

#endif //TWITTERTGBOT_TG_BOT_H
