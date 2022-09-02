#pragma once

#include <fstream>
#include "FileHandler.h"
#include "SharedPtr.h"
#include "Session.h"

#include <fcntl.h>
#include <queue>

class FileHandler {
public:
    template<class CoreModule>
    static void Handle(SharedPtr<Session<CoreModule> > session, const std::string& file, std::queue<SharedPtr<Event> >* event_queue);
};

template<class CoreModule>
void FileHandler::Handle(SharedPtr<Session<CoreModule> > session, const std::string& file, std::queue<SharedPtr<Event> >* event_queue) {
    int fd = open(file.c_str(), O_RDONLY);
    if (fd == -1) {
        throw std::logic_error("FileHandler::Handle: open failed, but file was checked before");
    }

}
