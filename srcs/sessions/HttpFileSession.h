#pragma once

#include "Session.h"

class HttpFileSession : public Session {
public:
    HttpFileSession(const std::shared_ptr<Config>& config, boost::asio::ip::tcp::socket socket,
                    const std::shared_ptr<Session>& main_session)
            : Session(config),
              main_http_session(main_session) {}

    ~HttpFileSession() {}

    [[nodiscard]] bool ShouldCloseAfterResponse() const override {
        return true;
    }

    [[nodiscard]] SessionType::Type GetType() const override {
        return SessionType::File;
    }

    [[nodiscard]] const std::string& GetResponseData() const override {
        throw std::logic_error("HttpFileSession::GetResponseData() should not be called");
    }

    [[nodiscard]] const std::string& GetName() const override {
        static std::string kName = "HttpFileSession";
        return kName;
    }

    std::shared_ptr<Session> main_http_session;

    std::string read_data;
};