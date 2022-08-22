#include <gtest/gtest.h>

#define private public

#include "Request.h"
#include "SharedPtr.h"

TEST(Request, Check_Init) {
    Request request;
    ASSERT_EQ(request._handle_state, RequestHandleState::HandleFirstLine);
    ASSERT_EQ(request._body, "");
    ASSERT_EQ(request._raw, "");
    ASSERT_EQ(request._headers.size(), 0);
    ASSERT_EQ(request._handled_size, 0);
    ASSERT_EQ(request._content_length, 0);
    ASSERT_EQ(request._method, "");
    ASSERT_EQ(request._resource_target, "");
    ASSERT_EQ(request._http_version.major, 0);
    ASSERT_EQ(request._http_version.minor, 0);
}

TEST(Request, Handle_FSM_First_Line) {
    RequestHandleStatus::Status res;
    Request request;

    size_t total_size = 0;
    std::string full_raw_request;

    const std::string first_line = "\r\n\r\n\r\n   GET  \r /   HTTP/1.1  \r\n";
    total_size = first_line.size();
    full_raw_request = first_line;
    EXPECT_NO_THROW(res = request.Handle(MakeShared(first_line)));
    ASSERT_EQ(res, RequestHandleStatus::WaitMoreData);
    ASSERT_EQ(request._handle_state, RequestHandleState::HandleHeader);
    ASSERT_EQ(request._handled_size, total_size);
    ASSERT_EQ(request._raw, full_raw_request);
    ASSERT_EQ(request._method, "GET");
    ASSERT_EQ(request._resource_target, "/");
    ASSERT_EQ(request._http_version.major, 1);
    ASSERT_EQ(request._http_version.minor, 1);
    ASSERT_EQ(request._headers.size(), 0);


    request = Request();  // reset request

    const std::string first_line_part1 = "\r\n\r\n\r\n   GET   \r/cgi-";
    full_raw_request = first_line_part1;
    EXPECT_NO_THROW(res = request.Handle(MakeShared(first_line_part1)));
    ASSERT_EQ(res, RequestHandleStatus::WaitMoreData);
    ASSERT_EQ(request._handle_state, RequestHandleState::HandleFirstLine);
    ASSERT_EQ(request._handled_size, 6); /// empty lines
    ASSERT_EQ(request._raw, full_raw_request);
    ASSERT_EQ(request._method, "");
    ASSERT_EQ(request._resource_target, "");
    ASSERT_EQ(request._http_version.major, 0);
    ASSERT_EQ(request._http_version.minor, 0);
    ASSERT_EQ(request._headers.size(), 0);


    const std::string first_line_part2 = "bin   HTTP/1.1  \r\n";
    total_size = first_line_part1.size() + first_line_part2.size();
    full_raw_request += first_line_part2;
    EXPECT_NO_THROW(res = request.Handle(MakeShared(first_line_part2)));
    ASSERT_EQ(res, RequestHandleStatus::WaitMoreData);
    ASSERT_EQ(request._handle_state, RequestHandleState::HandleHeader);
    ASSERT_EQ(request._handled_size, total_size);
    ASSERT_EQ(request._raw, full_raw_request);
    ASSERT_EQ(request._method, "GET");
    ASSERT_EQ(request._resource_target, "/cgi-bin");
    ASSERT_EQ(request._http_version.major, 1);
    ASSERT_EQ(request._http_version.minor, 1);
    ASSERT_EQ(request._headers.size(), 0);
}

TEST(Request, Handle_FSM_Headers) {
    RequestHandleStatus::Status res;
    Request request;

    size_t total_size = 0;
    std::string full_raw_request;

    const std::string first_line = "\r\n\r\n\r\n   GET \r  /   HTTP/1.1  \r\n";
    total_size = first_line.size();
    full_raw_request = first_line;
    res = request.Handle(MakeShared(first_line));

    /// headers
    const std::string host_header = "hoST: 123\r\n";
    total_size += host_header.size();
    full_raw_request += host_header;
    EXPECT_NO_THROW(res = request.Handle(MakeShared((host_header))));
    ASSERT_EQ(res, RequestHandleStatus::WaitMoreData);
    ASSERT_EQ(request._handle_state, RequestHandleState::HandleHeader);
    ASSERT_EQ(request._handled_size, total_size);
    ASSERT_EQ(request._raw, full_raw_request);
    ASSERT_EQ(request._headers.size(), 1);
    ASSERT_EQ(request._headers["host"], "123");

    const std::string check1_header = "check1:  \r\t   1  \t  2  3    \r\n";
    total_size += check1_header.size();
    full_raw_request += check1_header;
    EXPECT_NO_THROW(res = request.Handle(MakeShared((check1_header))));
    ASSERT_EQ(res, RequestHandleStatus::WaitMoreData);
    ASSERT_EQ(request._handle_state, RequestHandleState::HandleHeader);
    ASSERT_EQ(request._handled_size, total_size);
    ASSERT_EQ(request._raw, full_raw_request);
    ASSERT_EQ(request._headers.size(), 2);
    ASSERT_EQ(request._headers["check1"], "1  \t  2  3"); /// because of strip

    const std::string check2_header_part1 = "check2: \t first part, ";
    full_raw_request += check2_header_part1;
    EXPECT_NO_THROW(res = request.Handle(MakeShared((check2_header_part1))));
    ASSERT_EQ(res, RequestHandleStatus::WaitMoreData);
    ASSERT_EQ(request._handle_state, RequestHandleState::HandleHeader);
    ASSERT_EQ(request._handled_size, total_size); /// handled_size is not changed
    ASSERT_EQ(request._raw, full_raw_request);
    ASSERT_EQ(request._headers.size(), 2);

    const std::string check2_header_part2 = "second part \t\r\n";
    full_raw_request += check2_header_part2;
    total_size += check2_header_part1.size() + check2_header_part2.size();
    EXPECT_NO_THROW(res = request.Handle(MakeShared((check2_header_part2))));
    ASSERT_EQ(res, RequestHandleStatus::WaitMoreData);
    ASSERT_EQ(request._handle_state, RequestHandleState::HandleHeader);
    ASSERT_EQ(request._handled_size, total_size);
    ASSERT_EQ(request._raw, full_raw_request);
    ASSERT_EQ(request._headers.size(), 3);
    ASSERT_EQ(request._headers["check2"], "first part, second part");

    const std::string crln = "\r\n";
    full_raw_request += crln;
    total_size += crln.size();
    EXPECT_NO_THROW(res = request.Handle(MakeShared((crln))));
    ASSERT_EQ(res, RequestHandleStatus::Finish);
    ASSERT_EQ(request._handle_state, RequestHandleState::FinishHandle);
    ASSERT_EQ(request._handled_size, total_size);
    ASSERT_EQ(request._raw, full_raw_request);
}

TEST(Request, Handle_FSM_Body_By_Content_Length) {
    RequestHandleStatus::Status res;
    Request request;

    size_t total_size = 0;
    std::string full_raw_request;

    const std::string first_line = "\r\n\r\n\r\n   GET \r  /   HTTP/1.1  \r\n";
    total_size = first_line.size();
    full_raw_request = first_line;
    EXPECT_NO_THROW(res = request.Handle(MakeShared(first_line)));

    const std::string content_length_header = "Content-Length: 10\r\n";
    total_size += content_length_header.size();
    full_raw_request += content_length_header;
    EXPECT_NO_THROW(res = request.Handle(MakeShared(content_length_header)));
    ASSERT_EQ(request._headers[CONTENT_LENGTH], "10");

    const std::string crln = "\r\n";
    total_size += crln.size();
    full_raw_request += crln;
    EXPECT_NO_THROW(res = request.Handle(MakeShared(crln)));
    ASSERT_EQ(res, RequestHandleStatus::WaitMoreData);
    ASSERT_EQ(request._handle_state, RequestHandleState::HandleBodyByContentLength);
    ASSERT_EQ(request._handled_size, total_size);
    ASSERT_EQ(request._raw, full_raw_request);
    ASSERT_EQ(request._content_length, 10);

    const std::string body = "0123456789";
    total_size += body.size();
    full_raw_request += body;
    EXPECT_NO_THROW(res = request.Handle(MakeShared(body + "a")));
    ASSERT_EQ(res, RequestHandleStatus::Finish);
    ASSERT_EQ(request._handle_state, RequestHandleState::FinishHandle);
    ASSERT_EQ(request._handled_size, total_size + 1);
    ASSERT_EQ(request._raw, full_raw_request + "a");
    ASSERT_EQ(request._body, body);
}

TEST(Request, Handle_FSM_Chunked_Body) {
    RequestHandleStatus::Status res;
    Request request;

    size_t total_size = 0;
    std::string full_raw_request;

    const std::string first_line = "\r\n\r\n\r\n   GET \r  /   HTTP/1.1  \r\n";
    total_size = first_line.size();
    full_raw_request = first_line;
    EXPECT_NO_THROW(res = request.Handle(MakeShared(first_line)));

    const std::string transfer_encoding_header = "Transfer-Encoding: chunked\r\n\r\n";
    total_size += transfer_encoding_header.size();
    full_raw_request += transfer_encoding_header;
    EXPECT_NO_THROW(res = request.Handle(MakeShared(transfer_encoding_header)));
    ASSERT_EQ(request._headers[TRANSFER_ENCODING], "chunked");
    ASSERT_EQ(request._handle_state, RequestHandleState::HandleChunkSize);

    const std::string chunk1 = "5\r\n01234\r\n";
    total_size += chunk1.size();
    full_raw_request += chunk1;
    EXPECT_NO_THROW(res = request.Handle(MakeShared(chunk1)));
    ASSERT_EQ(res, RequestHandleStatus::WaitMoreData);
    ASSERT_EQ(request._handle_state, RequestHandleState::HandleChunkSize);
}