#include <sys/poll.h>
#include "EventPresets.h"

#include "HttpModule.h"

Event EventPresets::HttpParseRequestEvent(const ServerInstance& server_instance,
                                          const ConnectionInstance& connection_instance,
                                          const std::shared_ptr<std::string>& raw_request) {

    auto parse_http = [raw_request, server_instance, connection_instance](){
        HttpModule::ProcessParseHttpRequest(server_instance, connection_instance, raw_request);
//        connection_instance.events = POLLOUT;
    };

    return {EventType::ParseHttpRequest, parse_http};
}
