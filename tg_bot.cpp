#include "tg_bot.h"
#include "get_timeline.h"


namespace pt = boost::property_tree;

const int MIN_MESSAGE_LENGTH = 4;
const int MAX_TWEET_LENGTH = 140;

tg_bot::tg_bot() {

    TgBot::Bot bot(token);
    // start
    bot.getEvents().onCommand(command_list[0], [&bot](TgBot::Message::Ptr message) {

        bot.getApi().sendMessage(message->chat->id, "To start working with the bot, you need\\
        to log in to Twitter (/login). Create a new tweet(/new), by writing a message after \\
        the command. View the tweet feed (/tweets_feed). Log out of your account(/quit).");
    });

    // new tweet
    bot.getEvents().onCommand(command_list[1], [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id,message->text);

        network::Keys keys{network::read_keys("keys")};
        twitter::App app{keys.consumer_key, keys.consumer_secret};
        twitter::Account account{keys.user_token, keys.token_secret};
        app.account = account;
        // текст твита
        if(message->text.size() > MIN_MESSAGE_LENGTH) {
            std::string tweet = (message->text).erase(0,4);
            if (tweet.length() > MAX_TWEET_LENGTH) {
                bot.getApi().sendMessage(message->chat->id,"Your tweet is too long.");
            } else {
                int user_id = message->from->id;
                twitter::update_status(app, tweet);
                bot.getApi().sendMessage(message->chat->id, tweet);
            }
        }else{
            bot.getApi().sendMessage(message->chat->id,"Enter a tweet in the form: /new [message]");
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
        bot.getApi().sendMessage(message->chat->id, "Your tweets from the feed:");
        pt::ptree root = res.ptree();
        pt::ptree::const_iterator end = root.end();

        // строка с сообщением
        std::string str = std::string();

        for (pt::ptree::const_iterator it = root.begin(); it != end; ++it) {

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

            str+=  "Author: @" + user + "\n";
            //str+= "_date_: \n";
            str+= tweet + "\n --- \n";
        }
        bot.getApi().sendMessage(message->chat->id,str,true,0, std::make_shared< TgBot::GenericReply >());


    });

    // login
    bot.getEvents().onCommand(command_list[3], [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Log in to your account:");
        //

    });

    // quit autorization
    bot.getEvents().onCommand(command_list[4], [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Log out of your account");
        //TODO
    });

    // unknown command
    bot.getEvents().onUnknownCommand( [&bot](TgBot::Message::Ptr message) {
        if (StringTools::startsWith(message->text, "/help")) {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Sorry I have never heard this command before. Here's what I support: \\
        \n /help - about me");

        //TODO
    });

    bot.getEvents().onNonCommandMessage( [&bot] (TgBot::Message::Ptr message){
        bot.getApi().sendMessage(message->chat->id, "Why did you write this text, I will destroy it");
        //bot.getApi().deleteMessage(message->chat->id,message->messageId);
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


}

tg_bot::~tg_bot(){

}

