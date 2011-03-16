#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "yield/http/http_response.hpp"
#include "yield/http/http_response_parser.hpp"
#include "yunit.hpp"

TEST_SUITE(HTTPResponseParser);

namespace yield {
namespace http {
TEST(HTTPResponseParser, well_formed_reason_phrase_split) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 415 Unsupported Media Type\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_ne(http_response, NULL);
  throw_assert_eq(http_response->get_status_code(), 415);
  throw_assert_eq((*http_response)["Date"], "Fri, 31 Dec 1999 23:59:59 GMT");
  throw_assert_eq(http_response->get_body(), NULL);
  HTTPResponse::dec_ref(*http_response);
}

TEST(HTTPResponseParser, well_formed_pipelined_normal_body) {
  HTTPResponseParser http_response_parser("HTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\nContent-Length: 2\r\n\r\n12HTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\nContent-Length: 2\r\n\r\n12");
  vector<HTTPResponse*> http_responses;

  for (uint32_t i = 0; i < 2; i++) {
    HTTPResponse* http_response = object_cast<HTTPResponse>(http_response_parser.parse());

    if (http_response != NULL)
      http_responses.push_back(http_response);
    else {
      while (!http_responses.empty()) {
        HTTPResponse::dec_ref(*http_responses.back());
        http_responses.pop_back();
      }

      throw_assert(false);
    }
  }

  throw_assert_eq(http_responses.size(), 2);
  for (size_t i = 0; i < 2; i++) { throw_assert_eq(http_responses[i]->get_status_code(), 200); }
  for (size_t i = 0; i < 2; i++) { throw_assert_eq((*http_responses[i])["Date"], "Fri, 31 Dec 1999 23:59:59 GMT"); }
  for (size_t i = 0; i < 2; i++) { throw_assert_ne(http_responses[i]->get_body(), NULL); }
  for (size_t i = 0; i < 2; i++) { throw_assert_eq(http_responses[i]->get_content_length(), 2); }
  for (size_t i = 0; i < 2; i++) HTTPResponse::dec_ref(*http_responses[i]);
}

TEST(HTTPResponseParser, well_formed_pipelined_no_body) {
  HTTPResponseParser http_response_parser("HTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\nHTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n");
  vector<HTTPResponse*> http_responses;

  for (uint32_t i = 0; i < 2; i++) {
    HTTPResponse* http_response = object_cast<HTTPResponse>(http_response_parser.parse());

    if (http_response != NULL)
      http_responses.push_back(http_response);
    else {
      while (!http_responses.empty()) {
        HTTPResponse::dec_ref(*http_responses.back());
        http_responses.pop_back();
      }

      throw_assert(false);
    }
  }

  throw_assert_eq(http_responses.size(), 2);
  for (size_t i = 0; i < 2; i++) { throw_assert_eq(http_responses[i]->get_status_code(), 200); }
  for (size_t i = 0; i < 2; i++) { throw_assert_eq((*http_responses[i])["Date"], "Fri, 31 Dec 1999 23:59:59 GMT"); }
  for (size_t i = 0; i < 2; i++) { throw_assert_eq(http_responses[i]->get_body(), NULL); }
  for (size_t i = 0; i < 2; i++) HTTPResponse::dec_ref(*http_responses[i]);
}

TEST(HTTPResponseParser, well_formed_normal_body) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\nContent-Length: 2\r\n\r\n12").parse());
  throw_assert_ne(http_response, NULL);
  throw_assert_eq(http_response->get_status_code(), 200);
  throw_assert_eq((*http_response)["Date"], "Fri, 31 Dec 1999 23:59:59 GMT");
  throw_assert_ne(http_response->get_body(), NULL);
  throw_assert_eq(http_response->get_content_length(), 2);
  HTTPResponse::dec_ref(*http_response);
}

TEST(HTTPResponseParser, well_formed_no_body) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_ne(http_response, NULL);
  throw_assert_eq(http_response->get_status_code(), 200);
  throw_assert_eq((*http_response)["Date"], "Fri, 31 Dec 1999 23:59:59 GMT");
  throw_assert_eq(http_response->get_body(), NULL);
  HTTPResponse::dec_ref(*http_response);
}

TEST(HTTPResponseParser, malformed_status_line_missing) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_eq(http_response, NULL);
}

TEST(HTTPResponseParser, malformed_status_code_missing) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_eq(http_response, NULL);
}

TEST(HTTPResponseParser, malformed_status_code_alpha) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 XX OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_eq(http_response, NULL);
}

TEST(HTTPResponseParser, malformed_reason_phrase_missing) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 200\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_eq(http_response, NULL);
}

}
}
