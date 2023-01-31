#pragma once

#include "Event.h"
#include "Logging.h"
#include "core_helpers.h"
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
    Session(const std::shared_ptr<Config>& config, boost::asio::ip::tcp::socket socket)
            : available(true),
              last_activity_time(time(nullptr)),
              config(config),
              socket(std::move(socket)) {}

    virtual ~Session() {}

    [[nodiscard]] virtual bool ShouldCloseAfterResponse() const = 0;

    [[nodiscard]] virtual const std::string& GetResponseData() const = 0;

    [[nodiscard]] virtual const std::string& GetName() const = 0;

    [[nodiscard]] virtual SessionType::Type GetType() const = 0;

    void Close() {
        socket.close();
    }

    void UpdateLastActivityTime() {
        last_activity_time = time(nullptr);
    }

public:
    bool available;
    time_t last_activity_time;
    std::shared_ptr<Config> config;
    boost::asio::ip::tcp::socket socket;
};


void LogSession(Event* event, const std::shared_ptr<Session>& session) {
    LOG_INFO(event->GetName(), " on ", session->GetName(), "_remote_ep_", session->socket.remote_endpoint());
}

void LogSession(const std::shared_ptr<Session>& session, const std::string& message) {
    LOG_INFO(session->GetName(), "_remote_ep_", session->socket.remote_endpoint(), ": ", message);
}