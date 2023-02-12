#pragma once

#include "HttpSession.h"

class SessionManager {
public:
    void CloseSession(const std::shared_ptr<HttpSession>& session) {
        _sessions.erase(session);
        LOG_INFO("Sessions nums: ", _sessions.size());
    }

    void AddSession(const std::shared_ptr<HttpSession>& session) {
        _sessions.emplace(session);
    }

private:
    std::unordered_set<std::shared_ptr<HttpSession>> _sessions;
};