#include "tg_bot.h"
#include "get_timeline.h"
#include "login_with_twitter.h"
#include <twitterlib/twitterlib.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <time.h>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;


namespace pt = boost::property_tree;

const int MIN_MESSAGE_LENGTH = 5;
const int MAX_TWEET_LENGTH = 140;
const int MAX_MESSAGE_LENGTH = 4096;


mongocxx::instance instance{};
mongocxx::client client{mongocxx::uri{"mongodb://admin:admin@localhost:27017"}};

mongocxx::database db = client["twibotdb"];
mongocxx::collection coll = db["users"];

tg_bot::tg_bot() {

}

tg_bot::~tg_bot(){

}
void tg_bot::start() {
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

        bsoncxx::stdx::optional<bsoncxx::document::value> chat_id =
                coll.find_one(document{} << "chat_id" << message->chat->id << finalize);
        if(chat_id) {
            std::cout << bsoncxx::to_json(*chat_id) << "\n";
            bsoncxx::document::view view = chat_id->view();
            bsoncxx::document::element token = view["oauth_token"];
            if (token.type() != bsoncxx::type::k_utf8 || token.get_utf8().value.to_string().empty()) {
                std::cout << "error 1" << std::endl;
            } else {
                app.account.token = token.get_utf8().value.to_string();
            }

            bsoncxx::document::element token_secret = view["oauth_token_secret"];
            if (token.type() != bsoncxx::type::k_utf8 || token_secret.get_utf8().value.to_string().empty()) {
                std::cout << "error 1" << std::endl;
            } else {
                app.account.secret = token_secret.get_utf8().value.to_string();
            }

            // empty tweet
            if(message->text.size() < MIN_MESSAGE_LENGTH) {
                bot.getApi().sendMessage(message->chat->id, "Enter a tweet in the form: /new [message]");

            }else {
                std::string tweet = trim_string((message->text).erase(0, 4));

                if (tweet.length() > MAX_TWEET_LENGTH) {
                    bot.getApi().sendMessage(message->chat->id, "Sorry! Your tweet is too long.");
                } else {
                    twitter::update_status(app, tweet);
                    bot.getApi().sendMessage(message->chat->id, "Your tweet was sent successfully!");
                }
            }

        } else {
            std::string str = "You must be logged in.";
            bot.getApi().sendMessage(message->chat->id, str,true,0, std::make_shared< TgBot::GenericReply >());
        }

    });

    // tweet feed
    bot.getEvents().onCommand(command_list[2], [&bot](TgBot::Message::Ptr message) {
        network::Keys keys{network::read_keys("keys")};
        twitter::App app{keys.consumer_key, keys.consumer_secret};

        std::string last_seen_id;
        bsoncxx::stdx::optional<bsoncxx::document::value> chat_id =
                coll.find_one(document{} << "chat_id" << message->chat->id << finalize);
        network::Response res;
        std::string str;
        if(chat_id) {
            std::cout << bsoncxx::to_json(*chat_id) << "\n";
            bsoncxx::document::view view = chat_id->view();
            bsoncxx::document::element token = view["oauth_token"];
            if(token.type() != bsoncxx::type::k_utf8 || token.get_utf8().value.to_string().empty()) {
                std::cout << "error 1" << std::endl;
            } else {
                app.account.token = token.get_utf8().value.to_string();
            }

            bsoncxx::document::element token_secret = view["oauth_token_secret"];
            if(token.type() != bsoncxx::type::k_utf8 || token_secret.get_utf8().value.to_string().empty()) {
                std::cout << "error 1" << std::endl;
            } else {
                app.account.secret = token_secret.get_utf8().value.to_string();
            }

            bsoncxx::document::element element = view["last_seen_id"];
            if(element.type() != bsoncxx::type::k_utf8 || element.get_utf8().value.to_string().empty()) {
                res = get_timeline(app);
            } else {
                last_seen_id = element.get_utf8().value.to_string();
                res = get_timeline(app, last_seen_id);
            }
            pt::ptree root = res.ptree();
            pt::ptree::const_iterator end = root.end();

            // строка с сообщением
            str = std::string("Your tweets from the feed:\n\n");

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
                if (it == root.begin()) {
                    last_seen_id = id;
                    coll.update_one(document{} << "chat_id" << message->chat->id << finalize,
                                    document{} << "$set" << open_document <<
                                               "last_seen_id" << last_seen_id << close_document << finalize);
                }

                str+=  "Author: @" + user + "\n";
                //str+= "_date_: \n";
                str+= tweet + "\n";


                if(str.length() > (MAX_MESSAGE_LENGTH - MAX_TWEET_LENGTH) && it!= root.end()){
                    bot.getApi().sendMessage(message->chat->id,str,true,0, std::make_shared< TgBot::GenericReply >());
                    str.clear();
                }

            }

            bot.getApi().sendMessage(message->chat->id,str,true,0, std::make_shared< TgBot::GenericReply >());

        } else {
            str = "You must be logged in.";
            bot.getApi().sendMessage(message->chat->id, str, true);
        }


    });



    // login
    bot.getEvents().onCommand(command_list[3], [&bot](TgBot::Message::Ptr message) {
        network::Keys keys{network::read_keys("keys")};
        twitter::App app{keys.consumer_key, keys.consumer_secret};
       // twitter::Account account{keys.user_token, keys.token_secret};
       // app.account = account;

        std::string str = "To log in, follow the link and click on 'Authorize app': ";
        //TODO autorization

        bsoncxx::stdx::optional<bsoncxx::document::value> chat_id =
                coll.find_one(document{} << "chat_id" << message->chat->id << finalize);
        if(chat_id) {
            bot.getApi().sendMessage(message->chat->id, "You are already logged in.", true);
        } else {
            str += twitter::gen_login_link(app, coll, message->chat->id);
            bot.getApi().sendMessage(message->chat->id, str, true);
            std::string username;
            time_t t = time(NULL) + 60;
            while (time(NULL) < t) {
                chat_id = coll.find_one(document{} << "chat_id" << message->chat->id << finalize);
                if(chat_id) {
                    bsoncxx::document::view view = chat_id->view();
                    bsoncxx::document::element id = view["user_id"];
                    bsoncxx::document::element name = view["screen_name"];
                    if(id.get_utf8().value.to_string().empty() && name.get_utf8().value.to_string().empty()) {
                        continue;
                    } else {
                        username = name.get_utf8().value.to_string();
                        break;
                    }
                }
            }
            if (!username.empty()) {
                str = "Welcome, " + username;
                bot.getApi().sendMessage(message->chat->id, str, true);

            } else {
                str = "You didn't log in";
                bot.getApi().sendMessage(message->chat->id, str, true);
                coll.delete_one(document{} << "chat_id" << message->chat->id << finalize);
            }
        }

    });

    // quit autorization
    bot.getEvents().onCommand(command_list[4], [&bot](TgBot::Message::Ptr message) {
        coll.delete_one(document{} << "chat_id" << message->chat->id << finalize);
        bot.getApi().sendMessage(message->chat->id, "You logged out.");
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
//        TgWebhookTcpServer webhookServer(8080, bot);
//
//        printf("Server starting\n");
//        bot.getApi().setWebhook(webhookUrl);
//        webhookServer.start();
//    } catch (exception& e) {
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
