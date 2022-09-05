#pragma once

#include "Session.h"

template<class CoreModule>
class HttpFileSession : public Session<CoreModule> {
public:
    HttpFileSession(int core_module_index, CoreModule* core_module, SocketFd socket,
                    const SharedPtr<Session<CoreModule> >& main_session)
            : Session<CoreModule>(core_module_index, core_module, socket),
              main_http_session(main_session) {}

    ~HttpFileSession() {}

    bool ShouldCloseAfterResponse() const OVERRIDE {
        return true;
    }

    SessionType::Type GetType() const override {
        return SessionType::File;
    }

    const std::string& GetResponseData() const override {
        throw std::logic_error("HttpFileSession::GetResponseData() should not be called");
    }

    const std::string& GetName() const override {
        static std::string kName = "HttpFileSession";
        return kName;
    }

    SharedPtr<Session<CoreModule> > main_http_session;

    std::string read_data;
};