#include "Request.h"
#include "Exception.h"

#include <string>

namespace Http {

class AutoindexHandler {
public:
    static std::string Handle(const std::string& path, const std::string& trim_path, const std::shared_ptr<Request>& request);
};

}