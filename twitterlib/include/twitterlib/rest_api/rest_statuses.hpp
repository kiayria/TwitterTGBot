#ifndef TWITTERLIB_REST_API_REST_STATUSES_HPP
#define TWITTERLIB_REST_API_REST_STATUSES_HPP
#include <string>
#include <networklib/response.hpp>

namespace twitter {
struct App;

void update_status(const App& app, const std::string& message);
network::Response get_timeline(const App& app);
network::Response get_timeline(const App& app, std::string& id);


}  // namespace twitter
#endif  // TWITTERLIB_REST_API_REST_STATUSES_HPP
