#ifndef LOG_HPP
#define LOG_HPP

#define LOG_GLOBAL_SEPARATOR " "

#define LOG_MSG_CONFIG_PREFIX "Config - "
#define WARNING_MSG_CONFIG_PREFIX LOG_MSG_CONFIG_PREFIX "Warning - "
#define ERROR_MSG_CONFIG_PREFIX LOG_MSG_CONFIG_PREFIX "Error - "
#define LOG_MSG_CONFIG_DUPLICATE_DIRECTIVE_START WARNING_MSG_CONFIG_PREFIX "Duplicate directive: "
#define LOG_MSG_CONFIG_DUPLICATE_DIRECTIVE_END " at line "
#define ERROR_MSG_CONFIG_INVALID_PORT LOG_MSG_CONFIG_PREFIX "Invalid port number in listen directive: "

#define ERROR_MSG_RESPONSE_SENDING_FAILED "An error occured while sending response to the client"
#define ERROR_MSG_UNKNOWN "An Unknown error occured"
#define LOG_MSG_CLIENT_PREFIX "Client - "
#define LOG_MSG_REPONSE_SENT LOG_MSG_CLIENT_PREFIX "Response sent"
#define LOG_MSG_REPONSE_READY LOG_MSG_CLIENT_PREFIX "Response ready"
#define LOG_MSG_CONNECTION_TERMINATED LOG_MSG_CLIENT_PREFIX "Connection terminated by a peer"
#define LOG_MSG_CLIENT_CONNECTED LOG_MSG_CLIENT_PREFIX "Connected"
#define LOG_MSG_CLIENT_DISCONNECTED LOG_MSG_CLIENT_PREFIX "Disconnected"
#define ERROR_MSG_CLIENT_TIMED_OUT "Keepalive Timed out"
#define ERROR_MSG_CLIENT_BODY_TIMED_OUT "Headers Timed out"
#define ERROR_MSG_CLIENT_HEADERS_TIMED_OUT "Body Timed out"

#define LOG_MSG_CGIPIPE_PREFIX "CGIPipe - "
#define LOG_MSG_PIPE_CLOSED LOG_MSG_CGIPIPE_PREFIX "Pipe closed from the other end, stream closed"
#define ERROR_MSG_CGI_TIMEOUT LOG_MSG_CGIPIPE_PREFIX "CGI timed out"
#define LOG_MSG_CGI_CLOSED LOG_MSG_CGIPIPE_PREFIX "Closed"

#define LOG_MSG_SERVER_PREFIX "Virtual_Server - "
#define ERROR_MSG_FCNTL_FAILED LOG_MSG_SERVER_PREFIX "Fcntl failed to set the fd to non-blocking mode"
#define ERROR_MSG_ACCEPT_FAILED LOG_MSG_SERVER_PREFIX "Accept failed on the client fd"
#define LOG_MSG_SERVER_DISCONNECTED LOG_MSG_SERVER_PREFIX "Closed"
#define LOG_MSG_SERVER_IGNORED LOG_MSG_SERVER_PREFIX "Ignoring"

#endif