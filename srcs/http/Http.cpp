#include "Http.h"

#include <unordered_map>

namespace Http {
    Method GetMethod(const std::string& method) {

        static std::unordered_map<std::string, Method> methods_map = {
                {"GET",     GET},
                {"POST",    POST},
                {"HEAD",    HEAD},
                {"PUT",     PUT},
                {"DELETE",  DELETE},
                {"CONNECT", CONNECT},
                {"OPTIONS", OPTIONS},
                {"TRACE",   TRACE},
                {"PATCH",   PATCH}};

        std::unordered_map<std::string, Method>::iterator it = methods_map.find(method);
        if (it == methods_map.end()) {
            return UNKNOWN;
        }
        return it->second;
    }
}