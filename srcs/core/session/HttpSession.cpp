#include "HttpSession.h"

namespace HttpSessionState {
    const std::string& ToString(State state) {
        static std::unordered_map<int, std::string> kStateToString = {
                {HttpSessionState::ReadRequest, "ReadRequest"},
                {HttpSessionState::ProcessRequest, "ProcessRequest"},
                {HttpSessionState::ProcessResource, "ProcessResource"},
                {HttpSessionState::ResponseToClient, "ResponseToClient"}
        };
        return kStateToString[state];
    }
}