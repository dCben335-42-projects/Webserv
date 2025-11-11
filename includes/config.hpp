#ifndef CONFIG_HPP
#define CONFIG_HPP

# define PROJECT_NAME "WebServ"

# define DEFAULT_URL            "http://localhost:9058/"
# define SOCKET_IPV4            AF_INET
# define SOCKET_TCP             SOCK_STREAM
# define SOCKET_PROTOCOL        IPPROTO_TCP
# define SOCKET_INTERFACE       INADDR_ANY

# define MAX_CONNECTIONS        1024



// ANCHOR   Std Colors
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define RESET "\033[0m"


#endif