#include "common.h"

class HttpErrorCodePages {
    static const char* GetByStatus(RequestErrorStatus status);

    static const char* GetByStatusCode(int status_code) {
        return GetByStatus(static_cast<RequestErrorStatus>(status_code));
    }
};