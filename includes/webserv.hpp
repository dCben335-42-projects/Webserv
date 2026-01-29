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

#define READ_BUFFER_SIZE 8192
#define WRITE_BUFFER_SIZE 8192
#define READ_CGI_BUFFER_SIZE 8192
#define WRITE_CGI_BUFFER_SIZE 8192

#define FALLBACK_INDEX "index.html"

// ANCHOR   Std Colors
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"
#define CYAN "\033[36m"
#define MAGENTA "\033[35m"

#endif
