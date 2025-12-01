#ifndef CONFIG_HPP
#define CONFIG_HPP

# define PROJECT_NAME "WebServ"

# define DEFAULT_URL            "http://localhost:8080/"
# define DEFAULT_PORT           8080
# define SOCKET_IPV4            AF_INET
# define SOCKET_TCP             SOCK_STREAM
# define SOCKET_PROTOCOL        IPPROTO_TCP
# define SOCKET_INTERFACE       INADDR_ANY
# define MAX_CONNECTIONS        1024

# define PROTOCOL               "HTTP/1.1"
# define HEADER_ENDING          "\r\n\r\n"
# define HEADER_LINE_ENDING     "\r\n"
# define HEADER_SEPARATOR       ": "
# define HEADER_BUFFER_SIZE     2048

# define HEADER_CONTENT_LENGTH  "Content-Length"
# define HEADER_CONTENT_TYPE    "Content-Type"
# define HEADER_CONNECTION      "Connection"
# define HEADER_HOST            "Host"
# define HEADER_USER_AGENT      "User-Agent"
# define HEADER_ACCEPT          "Accept"
# define HEADER_ACCEPT_LANGUAGE "Accept-Language"
# define HEADER_ACCEPT_ENCODING "Accept-Encoding"
# define HEADER_COOKIE          "Cookie"


// ANCHOR   Content Types
// Text Types
# define HEADER_TYPES_HTML      "text/html"
# define HEADER_TYPES_PLAIN     "text/plain"
# define HEADER_TYPES_CSS       "text/css"
# define HEADER_TYPES_CSV       "text/csv"
// Application Types
# define HEADER_TYPES_JSON      "application/json"
# define HEADER_TYPES_JS        "application/javascript"
# define HEADER_TYPES_EOT       "application/vnd.ms-fontobject"
# define HEADER_TYPES_PDF       "application/pdf"
# define HEADER_TYPES_XML       "application/xml"
# define HEADER_TYPES_ZIP       "application/zip"
# define HEADER_TYPES_FORM      "application/x-www-form-urlencoded"
# define HEADER_TYPES_YAML      "application/x-yaml"
# define HEADER_TYPES_TOML      "application/toml"
# define HEADER_TYPES_RTF       "application/rtf"
// Font Types
# define HEADER_TYPES_WOFF      "font/woff"
# define HEADER_TYPES_WOFF2     "font/woff2"
# define HEADER_TYPES_TTF       "font/ttf"
# define HEADER_TYPES_OTF       "font/otf"
// Multipart Types
# define HEADER_TYPES_MULTIPART "multipart/form-data"
// Audio Types
# define HEADER_TYPES_MP3       "audio/mpeg"
# define HEADER_TYPES_WAV       "audio/wav"
# define HEADER_TYPES_FLAC      "audio/flac"
// Video Types
# define HEADER_TYPES_MP4       "video/mp4"
# define HEADER_TYPES_WEBM      "video/webm"
// Image Types
# define HEADER_TYPES_PNG       "image/png"
# define HEADER_TYPES_JPEG      "image/jpeg"
# define HEADER_TYPES_GIF       "image/gif"
# define HEADER_TYPES_SVG       "image/svg+xml"
# define HEADER_TYPES_AVIF      "image/avif"
# define HEADER_TYPES_HEIC      "image/heic"
# define HEADER_TYPES_ICNS      "image/icns"
# define HEADER_TYPES_BMP       "image/bmp"
# define HEADER_TYPES_TIFF      "image/tiff"

// ANCHOR   Method
# define M_GET                   "GET"
# define M_POST                  "POST"
# define M_DELETE                "DELETE"
# define M_PUT                   "PUT"
# define M_PATCH                 "PATCH"


// ANCHOR   Std Colors
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define RESET "\033[0m"


#endif