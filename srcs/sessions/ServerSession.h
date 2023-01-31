#pragma once

#include "Session.h"
#include "utilities.h"
#include "Config.h"

class ServerSession : public Session {
public:
    ServerSession(const std::shared_ptr<Config>& config, boost::asio::ip::tcp::socket socket,
                  const std::shared_ptr<ServerConfig>& server_config)
            : Session(config, std::move(socket)),
              server_config(server_config) {}

    ~ServerSession() {}

    bool ShouldCloseAfterResponse() const override {
        throw std::logic_error("ServerSession::GetResponseData() should not be called");
    }

    const std::string& GetResponseData() const override {
        throw std::logic_error("ServerSession::GetResponseData() should not be called");
    }

    const std::string& GetName() const override {
        static std::string kName = "ServerSession";
        return kName;
    }

    SessionType::Type GetType() const override {
        return SessionType::Server;
    }

    std::shared_ptr<ServerConfig> server_config;
};