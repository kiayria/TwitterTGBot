#include "tg_bot.h"
#include "get_timeline.h"


namespace pt = boost::property_tree;

#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>


const int MIN_MESSAGE_LENGTH = 5;
const int MAX_TWEET_LENGTH = 140;
const int MAX_MESSAGE_LENGTH = 4096;

tg_bot::tg_bot() {
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

    bsoncxx::builder::stream::document document{};

    auto collection = conn["testdb"]["testcollection"];
    document << "hello" << "world";
    document << "its" << "work";

    collection.insert_one(document.view());
    auto cursor = collection.find({});

    for (auto&& doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
    }
}

tg_bot::~tg_bot(){

}
void tg_bot::start() {

    std::string token(getenv("TOKEN"));
    printf("Token: %s\n", token.c_str());
//    std::string webhookUrl(getenv("URL"));
//    printf("Webhook url: %s\n", webhookUrl.c_str());

    TgBot::Bot bot(token);
    // start
    bot.getEvents().onCommand(command_list[0], [&bot](TgBot::Message::Ptr message) {

        bot.getApi().sendMessage(message->chat->id, "To start working with the bot, you need "
                                                    "to log in to Twitter (/login). Create a new tweet(/new), by writing a message after"
                                                    "the command. View the tweet feed (/tweets_feed). Log out of your account(/quit)."
                                                    );
    });

    // new tweet
    bot.getEvents().onCommand(command_list[1], [&bot](TgBot::Message::Ptr message) {

        network::Keys keys{network::read_keys("keys")};
        twitter::App app{keys.consumer_key, keys.consumer_secret};
        twitter::Account account{keys.user_token, keys.token_secret};
        app.account = account;

        // empty tweet
        if(message->text.size() < MIN_MESSAGE_LENGTH) {
            bot.getApi().sendMessage(message->chat->id, "Enter a tweet in the form: /new [message]");

        }else {
            std::string tweet = trim_string((message->text).erase(0, 4));

            if (tweet.length() > MAX_TWEET_LENGTH) {
                bot.getApi().sendMessage(message->chat->id, "Sorry! Your tweet is too long.");
            } else {
                twitter::update_status(app, tweet);
                bot.getApi().sendMessage(message->chat->id, "Your tweet was uploaded successfully!");
            }
        }
    });

    // tweet feed
    bot.getEvents().onCommand(command_list[2], [&bot](TgBot::Message::Ptr message) {

        network::Keys keys{network::read_keys("keys")};
        twitter::App app{keys.consumer_key, keys.consumer_secret};
        twitter::Account account{keys.user_token, keys.token_secret};
        app.account = account;
        auto res = get_timeline(app);
        //auto res = get_timeline(app, last_seen_id);



        pt::ptree root = res.ptree();
        pt::ptree::const_iterator end = root.end();

        // строка с сообщением
        std::string str = std::string("Your tweets from the feed:\n\n");

        for (pt::ptree::const_iterator it = root.begin(); it != end; ++it) {

            if(it != root.begin() && !str.empty()){
                str+= "\n-------------------\n\n";
            }


            //extracting id, username and tweet from JSON
            pt::ptree tmp = it->second;
            std::string id = tmp.get<std::string>("id");
            std::string user = tmp.get<std::string>("user.screen_name");
            std::string tweet = tmp.get<std::string>("text");

            //new tweets appear first, so we save the first tweet's id as the last seen one
            /*  if (it == root.begin()) {
                  last_seen_id = id;
                  //I will later insert last_seen_id into working DB
              }*/

            str+=  "Author: " + user + "\n";
            //str+= "_date_: \n";
            str+= tweet + "\n";


            if(str.length() > (MAX_MESSAGE_LENGTH - MAX_TWEET_LENGTH) && it!= root.end()){
                bot.getApi().sendMessage(message->chat->id,str,true,0, std::make_shared< TgBot::GenericReply >());
                str.clear();
            }

        }

        bot.getApi().sendMessage(message->chat->id,str,true,0, std::make_shared< TgBot::GenericReply >());


    });

    // login
    bot.getEvents().onCommand(command_list[3], [&bot](TgBot::Message::Ptr message) {

        std::string str = "To log in, follow the link: ";
        //TODO autorization

        bot.getApi().sendMessage(message->chat->id, str, true);
    });

    // quit autorization
    bot.getEvents().onCommand(command_list[4], [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Log out of your account");
        //TODO
    });

    // help command
    bot.getEvents().onCommand(command_list[5], [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Super help in a super bot:\n"
                                                    " - /new [message] - create new tweet\n"
                                                    " - /tweets_feed - tweets from your Twitter feed\n"
                                                    " - /login - log in to your account\n"
                                                    " - /quit - log out of your account\n"
                                                    );
    });

    // unknown command
    bot.getEvents().onUnknownCommand( [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Sorry I have never heard this command before. Here's what I support: "
                                                    "\n /help - about me");
    });

    // non command messages
    bot.getEvents().onNonCommandMessage( [&bot] (TgBot::Message::Ptr message){
        bot.getApi().sendMessage(message->chat->id, "I only understand commands. See them in /help.");
    });


    // start bot
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());

        bot.getApi().deleteWebhook();
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }


//    signal(SIGINT, [](int s) {
//        printf("SIGINT got\n");
//        exit(0);
//    });
//
//    try {
//        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
//
//        TgBot::TgWebhookTcpServer webhookServer(8080, bot);
//
//        printf("Server starting\n");
//        bot.getApi().setWebhook(webhookUrl);
//        webhookServer.start();
//    } catch (std::exception& e) {
//        printf("error: %s\n", e.what());
//    }

}


std::string trim_string(std::string tweet){
    char* str  = const_cast<char *>(tweet.c_str());
    int length = strlen(str);

    while(std::isspace(str[length-1]))
        --length;
    while (*str && isspace(*str))
        ++str, --length;

    return std::string(strndup(str,length));
}
