#include "FileHandler.h"

#include <fcntl.h>

void FileHandler::Handle(const std::string& file) {
    int fd = open(file.c_str(), O_RDONLY);
    if (fd == -1) {
        throw std::logic_error("FileHandler::Handle: open failed, but file was checked before");
    }
}
