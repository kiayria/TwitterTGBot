#include <iostream>
#include <twitterlib/twitterlib.hpp>

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>

using bsoncxx::builder::stream::close_array;
namespace pt = boost::property_tree;

int main() {
    //reading Twitter API consumer keys and secrets from a text file "keys"
    network::Keys keys{network::read_keys("keys")};

    twitter::App app{keys.consumer_key, keys.consumer_secret};
    twitter::Account account{keys.user_token, keys.token_secret};
    app.account = account;

    //sending Tweet
    twitter::update_status(app, "Testing API...");


    //connecting to DB
    mongocxx::instance instance{};
    mongocxx::client client{mongocxx::uri{"mongodb://admin:admin@localhost:27017"}};

    mongocxx::database db = client["testDB"];
    mongocxx::collection coll = db["testcollection"];

    //
    std::string last_seen_id = "1280410703959134209";
    //auto res = get_timeline(app); //timeline without since_id parameter
    auto res = get_timeline(app, last_seen_id);
    pt::ptree root = res.ptree();

    pt::ptree::const_iterator end = root.end();
    for (pt::ptree::const_iterator it = root.begin(); it != end; ++it)
    {

        //extracting id, username and tweet from JSON
        pt::ptree tmp = it->second;
        std::string id = tmp.get<std::string>("id");
        std::string user = tmp.get<std::string>("user.screen_name");
        std::string tweet = tmp.get<std::string>("text");

        //new tweets appear first, so we save the first tweet's id as the last seen one
        if (it == root.begin()) {
            last_seen_id = id;
            //I will later insert last_seen_id into working DB
        }
        //output
        std::cout << id << std::endl;
        std::cout << user << ": "  << tweet << std::endl;
        std::cout << "----- " << std::endl;

        //creating a document for db
        auto builder = bsoncxx::builder::stream::document{};
        bsoncxx::document::value doc_value = builder
                << "id" << id
                << "user" << user
                << "tweet" << tweet
                << bsoncxx::builder::stream::finalize;
        //inserting a document into db
        bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(doc_value.view());
    }

    return 0;
}
