#include <string>
#include <iostream>
#include <networklib/detail/encode.hpp>
#include <networklib/request.hpp>
#include <networklib/send.hpp>
#include <twitterlib/app.hpp>
#include <twitterlib/detail/authorize.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include "login_with_twitter.h"

namespace twitter {

    network::Response request_token(const App& app) {
        network::Request r;
        r.HTTP_method = "POST";
        r.URI = "/oauth/request_token";
        std::string tmp = network::detail::url_encode("http://127.0.0.1:5000");
        r.add_header("oauth_callback", tmp);
        detail::account_authorize(r, app);
        std::cout << std::string(r) << std::endl;
        return network::send(r);
    }

    std::string gen_login_link(const App& app, mongocxx::collection& coll, const int64_t& chat_id) {
        std::string link = "https://api.twitter.com/oauth/authorize?";
        std::string response = std::string(request_token(app));
        std::cout << response << std::endl;
        std::size_t pos = response.find("&");
        std::string str1 = response.substr (0, pos);
        std::string str2 = response.substr(pos+1);
        pos = str2.find("&");
        str2 = str2.substr(0, pos);
        std::string oauth_token = str1.substr(12);
        std::string oauth_token_secret = str2.substr(19);
        auto builder = bsoncxx::builder::stream::document{};
        bsoncxx::document::value doc_value = builder
                << "chat_id" << chat_id
                << "oauth_token" << oauth_token
                << "oauth_token_secret" << oauth_token_secret
                << "last_seen_id" << "1"
                << "user_id" << ""
                << "screen_name" << ""
                << bsoncxx::builder::stream::finalize;
        bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(doc_value.view());
        link += str1;
        return link;
    }

}