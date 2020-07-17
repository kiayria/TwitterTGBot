#ifndef TWITTERTGBOT_LOGIN_WITH_TWITTER_H
#define TWITTERTGBOT_LOGIN_WITH_TWITTER_H

#include <string>
#include <networklib/response.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

namespace twitter {
    struct App;

    network::Response request_token(const App& app);
    std::string gen_login_link(const App& app, mongocxx::collection& coll, const int64_t& chat_id);

}  // namespace twitter

#endif //TWITTERTGBOT_LOGIN_WITH_TWITTER_H
