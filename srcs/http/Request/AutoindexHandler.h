#include "Request.h"
#include "Exception.h"

#include <string>

namespace Http {

class AutoindexHandler {
public:
    static std::string Handle(const std::shared_ptr<Location>& location, const std::string& full_path, const std::string& path_after_matching, const std::shared_ptr<Request>& request);
};

}