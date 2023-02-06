#pragma once

#include "Event.h"
#include "Logging.h"

#include <utility>
#include <boost/asio.hpp>
#include <queue>

namespace SessionType {
    enum Type {
        Server,
        Http,
        File,
    };
}

class Session {
public:
    typedef typename std::unordered_map<boost::asio::ip::tcp::socket, std::shared_ptr<Session>>::iterator It;

    typedef std::shared_ptr<Session> Ptr;

public:
    Session()
            : available(true),
              last_activity_time(time(nullptr)) {}

    virtual ~Session() {}

    [[nodiscard]] virtual const std::string& GetName() const = 0;

    [[nodiscard]] virtual SessionType::Type GetType() const = 0;

    void UpdateLastActivityTime() {
        last_activity_time = time(nullptr);
    }

public:
    bool available;
    time_t last_activity_time; /// TODO change to std::chrono::time_point
};


//void LogSession(Event* event, const std::shared_ptr<Session>& session) {
//    LOG_INFO(event->GetName(), " on ", session->GetName(), "_remote_ep_", session->socket.remote_endpoint());
//}
//
//void LogSession(const std::shared_ptr<Session>& session, const std::string& message) {
//    LOG_INFO(session->GetName(), "_remote_ep_", session->socket.remote_endpoint(), ": ", message);
//}