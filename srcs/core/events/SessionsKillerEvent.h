#pragma once

#include "Event.h"
#include "Session.h"
#include "SharedPtr.h"
#include "Http.h"
#include "utilities.h"
#include "HttpSession.h"
#include "HttpFileSession.h"

template<class CoreModule>
class SessionsKillerEvent : public Event {
public:
    SessionsKillerEvent(std::unordered_map<SocketFd, SharedPtr<Session<CoreModule> > >* sessions,
                        std::queue<SharedPtr<Event> >* event_queue, int invoke_timeout)
            : _sessions(sessions),
              _event_queue(event_queue),
              _last_action_time(time(nullptr)),
              _invoke_delay(invoke_timeout) {}

    ~SessionsKillerEvent() {};

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;

    bool Ready() const;

private:
    std::unordered_map<SocketFd, SharedPtr<Session<CoreModule> > >* _sessions;
    std::queue<SharedPtr<Event> >* _event_queue;
    time_t _last_action_time;
    int _invoke_delay;
};

template<class CoreModule>
const std::string& SessionsKillerEvent<CoreModule>::GetName() const {
    static std::string kName = "SessionsKillerEvent";
    return kName;
}

template<class CoreModule>
void SessionsKillerEvent<CoreModule>::Process() {
    _last_action_time = time(nullptr);

    for (typename Session<CoreModule>::It it = _sessions->begin(); it != _sessions->end();) {
        if (!it->second->available) {
            it = _sessions->erase(it);
            continue;
        }

        switch (it->second->GetType()) {

            case SessionType::Http: {
                HttpSession<CoreModule>* http_session = dynamic_cast<HttpSession<CoreModule>*>(it->second.Get());

                if (http_session->keep_alive) {
                    if (difftime(time(nullptr), http_session->last_activity_time) > http_session->keep_alive_timeout) {
                        http_session->Close();
                    }
                }
                else if (difftime(time(nullptr), http_session->last_activity_time) >
                         http_session->port_servers_config->hang_session_timeout_s) {
                    http_session->Close();
                }
                break;
            }

            case SessionType::File: {
                HttpFileSession<CoreModule>* file_session = dynamic_cast<HttpFileSession<CoreModule>*>(it->second.Get());
                HttpSession<CoreModule>* main_http_session = dynamic_cast<HttpSession<CoreModule>*>(file_session->main_http_session.Get());

                if (difftime(time(nullptr), it->second->last_activity_time) >
                    main_http_session->port_servers_config->hang_session_timeout_s) {
                    file_session->Close();
                }
                break;
            }

            case SessionType::Server: {
                /// never close server session
                break;
            }
        }

        ++it;
    }
}

template<class CoreModule>
bool SessionsKillerEvent<CoreModule>::Ready() const {
    return difftime(time(nullptr), _last_action_time) > _invoke_delay;
}
