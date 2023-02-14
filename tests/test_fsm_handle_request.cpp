#include <gtest/gtest.h>

#define private public

#include "RequestParser.h"
#include "Http.h"

std::shared_ptr<EndpointConfig> endpoint_config = std::make_shared<EndpointConfig>(EndpointConfig(
        "127.0.0.1", 8080, {std::make_shared<ServerConfig>(ServerConfig("Kabun", "127.0.0.1", 8080, "", "", {}, 1000, 1000, 3, 4, {}, {}))}));


TEST(RequestParser, Check_Init) {
    Http::RequestParser parser(endpoint_config);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleFirstLine);
    ASSERT_EQ(parser._body, "");
    ASSERT_EQ(parser._raw_request, "");
    ASSERT_EQ(parser._http_headers.size(), 0);
    ASSERT_EQ(parser._parsed_size, 0);
    ASSERT_EQ(parser._content_length.has_value(), false);
    ASSERT_EQ(parser._target.path, "");
    ASSERT_EQ(parser._target.query_string, "");
}

TEST(Request, Handle_FSM_First_Line) {
    Http::RequestParseResult res;

    {
        Http::RequestParser parser(endpoint_config);

        const std::string first_line = "\r\n\r\n\r\nGet / HTTP/1.1\r\n";
        size_t total_size = first_line.size();
        const std::string& full_raw_request = first_line;
        EXPECT_NO_THROW(res = parser.Parse(first_line));
        ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);
        ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleHeader);
        ASSERT_EQ(parser._parsed_size, total_size);
        ASSERT_EQ(parser._raw_request, full_raw_request);
        ASSERT_EQ(parser._http_method, Http::Method::Get);
        ASSERT_EQ(parser._target.path, "/");
        ASSERT_EQ(parser._target.extension, "");
        ASSERT_EQ(parser._target.query_string, "");
        ASSERT_EQ(parser._http_version, Http::Version::Http1_1);
        ASSERT_EQ(parser._http_headers.size(), 0);
    }


    Http::RequestParser parser(endpoint_config);

    const std::string first_line_part1 = "\r\n\r\n\r\nGet /cgi-";
    std::string full_raw_request = first_line_part1;
    EXPECT_NO_THROW(res = parser.Parse(first_line_part1));
    ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleFirstLine);
    ASSERT_EQ(parser._parsed_size, 6); /// empty lines
    ASSERT_EQ(parser._raw_request, full_raw_request);
    ASSERT_EQ(parser._target.path, "");
    ASSERT_EQ(parser._http_headers.size(), 0);


    const std::string first_line_part2 = "bin/a/a/a/a.lua??\?\?////a=asd//\?\?/?? HTTP/1.1\r\n";

    size_t total_size = first_line_part1.size() + first_line_part2.size();
    full_raw_request += first_line_part2;
    EXPECT_NO_THROW(res = parser.Parse(first_line_part2));
    ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleHeader);
    ASSERT_EQ(parser._parsed_size, total_size);
    ASSERT_EQ(parser._raw_request, full_raw_request);
    ASSERT_EQ(parser._http_method, Http::Method::Get);
    ASSERT_EQ(parser._target.path, "/cgi-bin/a/a/a/a.lua");
    ASSERT_EQ(parser._target.extension, ".lua");
    ASSERT_EQ(parser._target.query_string, "?\?\?////a=asd//\?\?/??");
    ASSERT_EQ(parser._http_version, Http::Version::Http1_1);
    ASSERT_EQ(parser._http_headers.size(), 0);
}

TEST(Request, Handle_FSM_Headers) {
    Http::RequestParseResult res;
    Http::RequestParser parser(endpoint_config);

    size_t total_size = 0;
    std::string full_raw_request;

    const std::string first_line = "\r\n\r\n\r\nGet / HTTP/1.1\r\n";
    total_size = first_line.size();
    full_raw_request = first_line;
    res = parser.Parse(first_line);

    /// _http_headers
    const std::string host_header = "hoST: 123\r\n";
    total_size += host_header.size();
    full_raw_request += host_header;
    EXPECT_NO_THROW(res = parser.Parse((host_header)));
    ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleHeader);
    ASSERT_EQ(parser._parsed_size, total_size);
    ASSERT_EQ(parser._raw_request, full_raw_request);
    ASSERT_EQ(parser._http_headers.size(), 1);
    ASSERT_EQ(parser._http_headers["host"][0], "123");

    const std::string check1_header = "check1:     1  \t  2  3    \r\n";
    total_size += check1_header.size();
    full_raw_request += check1_header;
    EXPECT_NO_THROW(res = parser.Parse((check1_header)));
    ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleHeader);
    ASSERT_EQ(parser._parsed_size, total_size);
    ASSERT_EQ(parser._raw_request, full_raw_request);
    ASSERT_EQ(parser._http_headers.size(), 2);
    ASSERT_EQ(parser._http_headers["check1"][0], "1  \t  2  3"); /// because of strip

    const std::string check2_header_part1 = "check2: \t first part, ";
    full_raw_request += check2_header_part1;
    EXPECT_NO_THROW(res = parser.Parse((check2_header_part1)));
    ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleHeader);
    ASSERT_EQ(parser._parsed_size, total_size); /// handled_size is not changed
    ASSERT_EQ(parser._raw_request, full_raw_request);
    ASSERT_EQ(parser._http_headers.size(), 2);

    const std::string check2_header_part2 = "second part \t\r\n";
    full_raw_request += check2_header_part2;
    total_size += check2_header_part1.size() + check2_header_part2.size();
    EXPECT_NO_THROW(res = parser.Parse((check2_header_part2)));
    ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleHeader);
    ASSERT_EQ(parser._parsed_size, total_size);
    ASSERT_EQ(parser._raw_request, full_raw_request);
    ASSERT_EQ(parser._http_headers.size(), 3);
    ASSERT_EQ(parser._http_headers["check2"][0], "first part, second part");

    const std::string crln = "\r\n";
    full_raw_request += crln;
    total_size += crln.size();
    EXPECT_NO_THROW(res = parser.Parse((crln)));
    ASSERT_EQ(res.status, Http::RequestParseStatus::Finish);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleFirstLine); /// it was reset
    ASSERT_EQ(parser._parsed_size, 0);
    ASSERT_EQ(parser._raw_request.size(), 0);
}

TEST(Request, Handle_FSM_Body_By_Content_Length) {
    Http::RequestParseResult res;
    Http::RequestParser parser(endpoint_config);

    size_t total_size = 0;
    std::string full_raw_request;

    const std::string first_line = "\r\n\r\n\r\nGet / HTTP/1.1\r\n";
    total_size = first_line.size();
    full_raw_request = first_line;
    EXPECT_NO_THROW(res = parser.Parse(first_line));

    const std::string content_length_header = "Content-Length: 10\r\n";
    total_size += content_length_header.size();
    full_raw_request += content_length_header;
    EXPECT_NO_THROW(res = parser.Parse(content_length_header));
    ASSERT_EQ(parser._http_headers[CONTENT_LENGTH][0], "10");

    const std::string host_header = "host: a\r\n";
    total_size += host_header.size();
    full_raw_request += host_header;
    EXPECT_NO_THROW(res = parser.Parse(host_header));
    ASSERT_EQ(parser._http_headers[HOST][0], "a");

    total_size += CRLF_LEN;
    full_raw_request += CRLF;
    EXPECT_NO_THROW(res = parser.Parse(std::string(CRLF)));
    ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleBodyByContentLength);
    ASSERT_EQ(parser._parsed_size, total_size);
    ASSERT_EQ(parser._raw_request, full_raw_request);
    ASSERT_EQ(parser._content_length.value(), 10);

    const std::string body = "0123456789";
    total_size += body.size();
    full_raw_request += body;
    EXPECT_NO_THROW(res = parser.Parse(body + "a"));
    ASSERT_EQ(res.status, Http::RequestParseStatus::Finish);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleFirstLine);
    ASSERT_EQ(parser._parsed_size, 0);
    ASSERT_EQ(parser._raw_request.size(), 0);
    ASSERT_EQ(parser._body.size(), 0);
}

TEST(Request, Handle_FSM_Chunked_Body) {
    Http::RequestParseResult res;
    Http::RequestParser parser(endpoint_config);

    size_t total_size = 0;
    std::string full_raw_request;
    std::string full_body;

    const std::string first_line = "\r\n\r\n\r\nGet / HTTP/1.1\r\n";
    total_size = first_line.size();
    full_raw_request = first_line;
    EXPECT_NO_THROW(res = parser.Parse(first_line));

    const std::string host_header = "host: a\r\n";
    total_size += host_header.size();
    full_raw_request += host_header;
    EXPECT_NO_THROW(res = parser.Parse(host_header));
    ASSERT_EQ(parser._http_headers[HOST][0], "a");

    const std::string transfer_encoding_header = "Transfer-Encoding: chunked\r\n\r\n";
    total_size += transfer_encoding_header.size();
    full_raw_request += transfer_encoding_header;
    EXPECT_NO_THROW(res = parser.Parse(transfer_encoding_header));
    ASSERT_EQ(parser._http_headers[TRANSFER_ENCODING][0], "chunked");
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleChunkSize);

    const std::string chunk1 = "5\r\n01234\r\n";
    total_size += chunk1.size();
    full_raw_request += chunk1;
    full_body += "01234";
    EXPECT_NO_THROW(res = parser.Parse(chunk1));
    ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleChunkSize);
    ASSERT_EQ(parser._parsed_size, total_size);
    ASSERT_EQ(parser._raw_request, full_raw_request);
    ASSERT_EQ(parser._body, "01234");
    ASSERT_EQ(parser._chunk_body_size, 5);

    const std::string chunk2_size = "E aaaa \t\t bbb \r\r\r ccc  \r\n";
    total_size += chunk2_size.size();
    full_raw_request += chunk2_size;
    EXPECT_NO_THROW(res = parser.Parse(chunk2_size));
    ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleChunkBody);
    ASSERT_EQ(parser._parsed_size, total_size);
    ASSERT_EQ(parser._raw_request, full_raw_request);
    ASSERT_EQ(parser._chunk_body_size, 14);

    const std::string chunk2_body = "123456789abcde";
    full_body += chunk2_body;
    total_size += chunk2_body.size();
    full_raw_request += chunk2_body;

    /// check error
    Http::RequestParser check_incorrect_chunk_parser = parser;
    EXPECT_NO_THROW(res = check_incorrect_chunk_parser.Parse(chunk2_body + "f"));
    ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);
    EXPECT_ANY_THROW(res = check_incorrect_chunk_parser.Parse(std::string(CRLF)));
    /// end check error

    EXPECT_NO_THROW(res = parser.Parse(chunk2_body));
    ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);

    total_size += CRLF_LEN;
    full_raw_request += CRLF;
    EXPECT_NO_THROW(res = parser.Parse(std::string(CRLF)));
    ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleChunkSize);
    ASSERT_EQ(parser._parsed_size, total_size);
    ASSERT_EQ(parser._raw_request, full_raw_request);
    ASSERT_EQ(parser._body, full_body);

    const std::string chunk_end = "0\r\n\r\n";
    total_size += chunk_end.size();
    full_raw_request += chunk_end;
    EXPECT_NO_THROW(res = parser.Parse(chunk_end + "a"));
    ASSERT_EQ(res.status, Http::RequestParseStatus::Finish);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleFirstLine);
    ASSERT_EQ(parser._parsed_size, 0);
    ASSERT_EQ(parser._raw_request.size(), 0);
    ASSERT_EQ(parser._body.size(), 0);
    ASSERT_EQ(parser._content_length.has_value(), false);
}

TEST(Request, Request_Target_Parse) {
    Http::RequestParseResult res;
    Http::RequestParser parser(endpoint_config);

    size_t total_size = 0;

    std::string full_raw_request;
    std::string full_body;
    const std::string first_line = "\r\n\r\n\r\nGet /? HTTP/1.0\r\n";
    total_size = first_line.size();
    full_raw_request = first_line;
    EXPECT_NO_THROW(res = parser.Parse(first_line));

    ASSERT_EQ(res.status, Http::RequestParseStatus::WaitMoreData);
    ASSERT_EQ(parser._parse_state, Http::RequestParseState::HandleHeader);
    ASSERT_EQ(parser._parsed_size, total_size);
    ASSERT_EQ(parser._raw_request, full_raw_request);
    ASSERT_EQ(parser._http_method, Http::Method::Get);
    ASSERT_EQ(parser._target.path, "/");
    ASSERT_EQ(parser._target.query_string, "");
    ASSERT_EQ(parser._http_version, Http::Version::Http1_0);
    ASSERT_EQ(parser._http_headers.size(), 0);
}