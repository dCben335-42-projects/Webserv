#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define FLAG_LOCATION_ADD_UP 1
#define FLAG_LOG_CONF 1

#define BNF_FILE_PATH "config/conf.bnf"
#define DEFAULT_CONFIG_PATH "config/default.conf"

#define SERVER_NAME "i_am_the_one_who_ports"
#define SERVER_VERSION "1.1"
#define SERVER_FULL_NAME SERVER_NAME "/" SERVER_VERSION

#define PROTOCOL "HTTP/1.1"
#define PREV_PROTOCOL "HTTP/1.0"
#define GATEWAY_INTERFACE "CGI/1.1"
#define PROTOCOL_HTTP_URL_PREFIX "http://"
#define PROTOCOL_HTTPS_URL_PREFIX "https://"

#define READ_CLIENT_BUFFER_SIZE 819200u
#define WRITE_CLIENT_BUFFER_SIZE 8192u

#define READ_CGI_BUFFER_SIZE 8192u
#define WRITE_CGI_BUFFER_SIZE 8192u

#define POLL_TIMEOUT 500u

#define COOKIE_EXPIRE_DURATION 36000u
#define COOKIE_SESSION_MIN 600u

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
