#include "Request.h"
#include "utilities.h"

RequestParseState Request::ParseFirstLineHandler() {
    _raw_parsed_size = 0;
//    _raw

    return ParseHeaders;
    return Error;
}

RequestParseState Request::ParseHeaderHandler() {
    return ParseHeaders;
    return FinishParseHeaders;
    return Error;
}

RequestParseState Request::ParseBodyByChunkHandler() {
    return Error;
}

RequestParseState Request::ParseBodyByContentLengthHandler() {
    return Error;
}

RequestParseState Request::AnalyzeBodyHeadersHandler() {
    return FinishParseHeaders;
}

RequestParseStatus Request::Parse(SharedPtr<std::string> raw_request_part) {
    _raw += *raw_request_part;

    /// если стейт ParseHeaders, то вызываем хендлер для парсинга заголовков (вернет стейт ParseHeaders).

    /// если стейт ParseBodyByChunk, то вызываем хендлер для парсинга тела по чанкам (вернет стейт ParseBodyByChunk или Finish).
    /// если стейт ParseBodyByContentLength, то вызываем хендлер для парсинга тела по длине (вернет стейт ParseBodyByContentLength или Finish).

    size_t pos = _raw_parsed_size;
    while (true) {
        switch (_parse_state) {
            case ParseFirstLine:
                _parse_state = ParseFirstLineHandler();
                break;
            case ParseHeaders:
                _parse_state = ParseHeaderHandler();
                break;
            case FinishParseHeaders:
                _parse_state = AnalyzeBodyHeadersHandler();
                break;
            case ParseBodyByChunk:
                _parse_state = ParseBodyByChunkHandler();
                break;
            case ParseBodyByContentLength:
                _parse_state = ParseBodyByContentLengthHandler();
                break;
            case Finish:
                break;
            case Error:
                break;
        }
    }

    /// парсим первую строку
    /// парсим хедеры в мапку [string ключ, string значение]
    /// после двух CRLF - проверяем есть хедер host, если нет - bad request
    /// в зависимости от

}

void Request::AddHeader(const std::string& key, const std::string& value) {
    _headers[key] = StripString(value);
}
