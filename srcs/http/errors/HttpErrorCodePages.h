#include "common.h"
#include "HttpException.h"

class HttpErrorCodePages {
    static const char* GetByErrorCode(HttpError::Code code);

    static const char* GetByStatusCode(int status_code) {
        return GetByErrorCode(static_cast<HttpError::Code>(status_code));
    }
};