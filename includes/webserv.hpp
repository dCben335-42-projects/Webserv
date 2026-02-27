#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "constants.hpp"

#define FLAG_LOCATION_ADD_UP 1

#define BNF_FILE_PATH "config/conf.bnf"
#define DEFAULT_CONFIG_PATH "config/default.conf"
#define DEFAULT_CONFIG_MSG "No configuration file specified. Using default: " DEFAULT_CONFIG_PATH

#define SERVER_NAME "i_am_the_one_who_ports"
#define SERVER_VERSION "1.0"
#define SERVER_FULL_NAME SERVER_NAME "/" SERVER_VERSION

#define PROTOCOL "HTTP/1.1"
#define GATEWAY_INTERFACE "CGI/1.1"

#define READ_CLIENT_BUFFER_SIZE 8192u
#define WRITE_CLIENT_BUFFER_SIZE 8192u

#define READ_CGI_BUFFER_SIZE 8192u
#define WRITE_CGI_BUFFER_SIZE 8192u

#define FALLBACK_MAX_BODY_SIZE (100 * MB)
#define FALLBACK_MAX_HEADERS_SIZE (8 * KB)

#define FALLBACK_INDEX "index.html"
#define FALLBACK_KEEP_ALIVE_TIMEOUT 50u
#define FALLBACK_HEADER_TIMEOUT 5u
#define FALLBACK_BODY_TIMEOUT 5u
#define POLL_TIMEOUT 500u


// Cookie Duration in seconds
#define COOKIE_EXPIRE_DURATION 36000u
#define COOKIE_SESSION_MIN 600u

// Usefuls Cookies defines
#define COOKIE_FIELD_SEP "; "
#define COOKIE_KEY_TOKEN "token"
#define COOKIE_KEY_PATH "path"
#define COOKIE_KEY_EXPIRES "expires"
#define COOKIE_VALUE_PATH "/"
#define COOKIE_KEY_HTTPONLY "HttpOnly"
#define COOKIE_VALUE_SEP "="

#define FORM_FIELD_SEP "&"

#define REGISTER_SUCCESS_MSG "Register Successful"
#define EDIT_SUCCESS_MSG "Filed edited successfully"
#define DELETE_SUCCESS_MSG "File deleted successfully"
#define LOGIN_SUCCESS_MSG "Login Successful"
#define UPLOAD_SUCCESS_MSG "File uploaded successfully"
#define INVALID_UPLOAD_DATA_MSG "Invalid file upload data"

#endif
