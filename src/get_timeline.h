#ifndef NEW_GET_TIMELINE_H
#define NEW_GET_TIMELINE_H
#include <string>
#include <networklib/response.hpp>

namespace twitter {
    struct App;

    network::Response get_timeline(const App &app);

    network::Response get_timeline(const App &app, std::string &id);

}
#endif //NEW_GET_TIMELINE_H
