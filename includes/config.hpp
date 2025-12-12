#ifndef CONFIG_HPP
#define CONFIG_HPP

#define PROJECT_NAME "WebServ"

#define DEFAULT_URL "http://localhost:8080/"
#define DEFAULT_PORT 8080
#define SOCKET_IPV4 AF_INET
#define SOCKET_TCP SOCK_STREAM
#define SOCKET_PROTOCOL IPPROTO_TCP
#define SOCKET_INTERFACE INADDR_ANY
#define MAX_CONNECTIONS 1024

#define PROTOCOL "HTTP/1.1"
#define HEADER_ENDING "\r\n\r\n"
#define HEADER_LINE_ENDING "\r\n"
#define HEADER_SEPARATOR ": "
#define READ_BUFFER_SIZE 8192
#define WRITE_BUFFER_SIZE 8192

#define HEADER_CONTENT_LENGTH "Content-Length"
#define HEADER_CONTENT_TYPE "Content-Type"
#define HEADER_CONNECTION "Connection"
#define HEADER_HOST "Host"
#define HEADER_USER_AGENT "User-Agent"
#define HEADER_ACCEPT "Accept"
#define HEADER_ACCEPT_LANGUAGE "Accept-Language"
#define HEADER_ACCEPT_ENCODING "Accept-Encoding"
#define HEADER_COOKIE "Cookie"


// ANCHOR   Content Types
// Text Types
#define HEADER_TYPE_HTML "text/html"
#define HEADER_TYPE_PLAIN "text/plain"
#define HEADER_TYPE_CSS "text/css"
#define HEADER_TYPE_CSV "text/csv"
// Application Types
#define HEADER_TYPE_JSON "application/json"
#define HEADER_TYPE_JS "application/javascript"
#define HEADER_TYPE_EOT "application/vnd.ms-fontobject"
#define HEADER_TYPE_PDF "application/pdf"
#define HEADER_TYPE_XML "application/xml"
#define HEADER_TYPE_ZIP "application/zip"
#define HEADER_TYPE_FORM "application/x-www-form-urlencoded"
#define HEADER_TYPE_YAML "application/x-yaml"
#define HEADER_TYPE_TOML "application/toml"
#define HEADER_TYPE_RTF "application/rtf"
#define HEADER_TYPE_OCTET "application/octet-stream"

// Font Types
#define HEADER_TYPE_WOFF "font/woff"
#define HEADER_TYPE_WOFF2 "font/woff2"
#define HEADER_TYPE_TTF "font/ttf"
#define HEADER_TYPE_OTF "font/otf"
// Multipart Types
#define HEADER_TYPE_MULTIPART "multipart/form-data"
// Audio Types
#define HEADER_TYPE_MP3 "audio/mpeg"
#define HEADER_TYPE_WAV "audio/wav"
#define HEADER_TYPE_FLAC "audio/flac"
// Video Types
#define HEADER_TYPE_MP4 "video/mp4"
#define HEADER_TYPE_WEBM "video/webm"
// Image Types
#define HEADER_TYPE_PNG "image/png"
#define HEADER_TYPE_JPEG "image/jpeg"
#define HEADER_TYPE_WEBP "image/webp"
#define HEADER_TYPE_GIF "image/gif"
#define HEADER_TYPE_SVG "image/svg+xml"
#define HEADER_TYPE_AVIF "image/avif"
#define HEADER_TYPE_HEIC "image/heic"
#define HEADER_TYPE_ICNS "image/icns"
#define HEADER_TYPE_BMP "image/bmp"
#define HEADER_TYPE_TIFF "image/tiff"
#define HEADER_TYPE_ICO "image/x-icon"

// ANCHOR   Method
#define M_GET "GET"
#define M_POST "POST"
#define M_DELETE "DELETE"
#define M_PUT "PUT"
#define M_PATCH "PATCH"


// ANCHOR   Std Colors
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define RESET "\033[0m"



#endif