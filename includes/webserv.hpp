#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define SERVER_NAME "i_am_the_one_who_ports"
#define SERVER_VERSION "1.0"
#define SERVER_FULL_NAME SERVER_NAME "/" SERVER_VERSION

#define DEFAULT_URL "http://localhost:8080/"
#define DEFAULT_PORT 8080

#define SOCKET_IPV4 AF_INET
#define SOCKET_TCP SOCK_STREAM
#define SOCKET_PROTOCOL IPPROTO_TCP
#define SOCKET_INTERFACE INADDR_ANY
#define MAX_CONNECTIONS 1024

#define PROTOCOL "HTTP/1.1"
#define GATEWAY_INTERFACE "CGI/1.1"

#define READ_BUFFER_SIZE 8192u
#define WRITE_BUFFER_SIZE 8192u
#define READ_CGI_BUFFER_SIZE 8192u
#define WRITE_CGI_BUFFER_SIZE 8192u

#define KB 1024
#define MB (1024 * KB)
#define GB (1024 * MB)
#define MAX_BODY_SIZE (100 * MB)
#define MAX_HEADERS_SIZE (8 * KB)

#define FALLBACK_INDEX "index.html"
#define FALLBACK_KEEP_ALIVE_TIMEOUT 50u
#define FALLBACK_HEADER_TIMEOUT 5u
#define FALLBACK_BODY_TIMEOUT 5u
#define POLL_TIMEOUT 500u

// ANCHOR   Std Colors
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"
#define CYAN "\033[36m"
#define MAGENTA "\033[35m"

// Fowler-Noll-Vo magic constants
#define FNV_PRIME_32 16777619u
#define FNV_OFFSET_BASIS_32 2166136261u
// Same but for 64bit encryption
#define FNV_OFFSET_BASIS_64 14695981039346656037ul
#define FNV_PRIME_64 1099511628211ul

// MurmurHash constant (that happens to work well apparently)
#define MURMUR_HASH 1540483477u
#define MURMUR_FIRST_SHIFT 13u
#define MURMUR_LAST_SHIFT 15u

// Cookie Duration in seconds
#define COOKIE_EXPIRE_DURATION 120u
#define COOKIE_SESSION_MIN 60u

// Usefuls Cookies defines
#define COOKIE_FIELD_SEP "; "
#define COOKIE_KEY_TOKEN "token"
#define COOKIE_KEY_PATH "path"
#define COOKIE_KEY_EXPIRES "expires"
#define COOKIE_VALUE_PATH "/"
#define COOKIE_KEY_HTTPONLY "HttpOnly"
#define COOKIE_VALUE_SEP "="

#define FORM_FIELD_SEP "&"
#endif
