#pragma once

#include "Logging.h"

#include <utility>
#include <boost/asio.hpp>
#include <queue>

class Session {
public:
    typedef typename std::unordered_map<boost::asio::ip::tcp::socket, std::shared_ptr<Session>>::iterator It;

    typedef std::shared_ptr<Session> Ptr;

public:
    Session()
            : available(true),
              last_activity_time(time(nullptr)) {}

    virtual ~Session() = default;

    [[nodiscard]] virtual const std::string& GetName() const = 0;

    void UpdateLastActivityTime() {
        last_activity_time = time(nullptr);
    }

public:
    bool available;
    time_t last_activity_time; /// TODO change to std::chrono::time_point
};
