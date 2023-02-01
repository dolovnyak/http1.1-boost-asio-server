#include "Config.h"

Location::Location(const std::string& location, const std::string& root,
                   const bool autoindex, const std::string& index,
                   const std::vector<std::string>& available_methods, const std::string& redirect)
        : location(location), root(root),
          autoindex(autoindex), index(index),
          available_methods(), redirect(redirect) {
/// cast available methods to Http::Method
/// fill full_path
}
