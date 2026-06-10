#include "../request_parser/httpRequestParser.hpp"

void printResult(const std::string& testName, bool passed)
{
    std::cout << (passed ? "✅ PASS" : "❌ FAIL") << " — " << testName << std::endl;
}

// ─── VALID REQUESTS ───────────────────────────────────────────────────────────

void test_valid_GET()
{
    HttpRequest req;
    std::string raw =
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n";

    req.parse(raw);
    req.validate();

    printResult("Valid GET request",
        req.getMethod() == "GET" &&
        req.getUri()    == "/index.html" &&
        req.getVersion()== "HTTP/1.1" &&
        req.getStatusCode() == 200);
}

void test_valid_POST()
{
    HttpRequest req;
    std::string raw =
        "POST /upload HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 27\r\n"
        "\r\n"
        "username=john&password=1234";

    req.parse(raw);
    req.validate();

    printResult("Valid POST request",
        req.getMethod() == "POST" &&
        req.getBody()   == "username=john&password=1234" &&
        req.getStatusCode() == 200);
}

void test_valid_DELETE()
{
    HttpRequest req;
    std::string raw =
        "DELETE /resource/42 HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n";

    req.parse(raw);
    req.validate();

    printResult("Valid DELETE request",
        req.getMethod() == "DELETE" &&
        req.getStatusCode() == 200);
}

// ─── METHOD TESTS ─────────────────────────────────────────────────────────────

void test_invalid_method()
{
    HttpRequest req;
    std::string raw =
        "PATCH /resource HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n";

    req.parse(raw);
    req.validate();

    printResult("Invalid method → 405",
        req.getStatusCode() == 405);
}

void test_lowercase_method()
{
    HttpRequest req;
    std::string raw =
        "get /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n";

    req.parse(raw);
    req.validate();

    printResult("Lowercase method → 405",
        req.getStatusCode() == 405);
}

// ─── VERSION TESTS ────────────────────────────────────────────────────────────

void test_invalid_version()
{
    HttpRequest req;
    std::string raw =
        "GET /index.html HTTP/2.0\r\n"
        "Host: localhost\r\n"
        "\r\n";

    req.parse(raw);
    req.validate();

    printResult("Invalid version → 505",
        req.getStatusCode() == 505);
}

void test_valid_HTTP10()
{
    HttpRequest req;
    std::string raw =
        "GET /index.html HTTP/1.0\r\n"
        "Host: localhost\r\n"
        "\r\n";

    req.parse(raw);
    req.validate();

    printResult("Valid HTTP/1.0 → 200",
        req.getStatusCode() == 200);
}

// ─── URI TESTS ────────────────────────────────────────────────────────────────

void test_missing_uri_slash()
{
    HttpRequest req;
    std::string raw =
        "GET index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n";

    req.parse(raw);
    req.validate();

    printResult("URI missing leading slash → 400",
        req.getStatusCode() == 400);
}

void test_root_uri()
{
    HttpRequest req;
    std::string raw =
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n";

    req.parse(raw);
    req.validate();

    printResult("Root URI '/' → 200",
        req.getStatusCode() == 200);
}

// ─── HOST HEADER TESTS ────────────────────────────────────────────────────────

void test_missing_host_header()
{
    HttpRequest req;
    std::string raw =
        "GET /index.html HTTP/1.1\r\n"
        "Content-Type: text/html\r\n"
        "\r\n";

    req.parse(raw);
    req.validate();

    printResult("Missing Host header → 400",
        req.getStatusCode() == 400);
}

// ─── BODY TESTS ───────────────────────────────────────────────────────────────

void test_body_trimmed_to_content_length()
{
    HttpRequest req;
    std::string raw =
        "POST /upload HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 5\r\n"
        "\r\n"
        "helloEXTRAGARBAGE";

    req.parse(raw);
    req.validate();

    printResult("Body trimmed to Content-Length",
        req.getBody() == "hello");
}

void test_empty_body_GET()
{
    HttpRequest req;
    std::string raw =
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n";

    req.parse(raw);
    req.validate();

    printResult("GET has empty body",
        req.getBody().empty());
}

void test_post_empty_body()
{
    HttpRequest req;
    std::string raw =
        "POST /upload HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 0\r\n"
        "\r\n";

    req.parse(raw);
    req.validate();

    printResult("POST with Content-Length: 0 → 200",
        req.getStatusCode() == 200 &&
        req.getBody().empty());
}

// ─── HEADER TESTS ─────────────────────────────────────────────────────────────

void test_header_whitespace_trimmed()
{
    HttpRequest req;
    std::string raw =
        "GET /index.html HTTP/1.1\r\n"
        "Host:   localhost\r\n"   // extra spaces after colon
        "\r\n";

    req.parse(raw);
    req.validate();

    printResult("Header value whitespace trimmed → 200",
        req.getStatusCode() == 200);
}

// ─── MALFORMED REQUESTS ───────────────────────────────────────────────────────

void test_empty_request()
{
    HttpRequest req;
    req.parse("");
    req.validate();

    printResult("Empty request → not 200",
        req.getStatusCode() != 200);
}

void test_missing_request_line()
{
    HttpRequest req;
    std::string raw =
        "Host: localhost\r\n"
        "\r\n";

    req.parse(raw);
    req.validate();

    printResult("Missing request line → not 200",
        req.getStatusCode() != 200);
}

// ─── MAIN ─────────────────────────────────────────────────────────────────────

int main()
{
    std::cout << "\n=== HTTP Request Parser Tests ===\n" << std::endl;

    test_valid_GET();
    test_valid_POST();
    test_valid_DELETE();

    test_invalid_method();
    test_lowercase_method();

    test_invalid_version();
    test_valid_HTTP10();

    test_missing_uri_slash();
    test_root_uri();

    test_missing_host_header();

    test_body_trimmed_to_content_length();
    test_empty_body_GET();
    test_post_empty_body();

    test_header_whitespace_trimmed();

    test_empty_request();
    test_missing_request_line();

    std::cout << "\n=================================\n" << std::endl;
}