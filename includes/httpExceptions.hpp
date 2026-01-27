#ifndef HTTP_EXCEPTIONS_HPP
#define HTTP_EXCEPTIONS_HPP

#define HTTP_200_MSG "OK"
#define HTTP_201_MSG "Created"
#define HTTP_204_MSG "No Content"
#define HTTP_400_MSG "Bad Request"
#define HTTP_401_MSG "Unauthorized"
#define HTTP_403_MSG "Forbidden"
#define HTTP_404_MSG "Not Found"
#define HTTP_405_MSG "Method Not Allowed"
#define HTTP_500_MSG "Internal Server Error"
#define HTTP_501_MSG "Not Implemented"

#include "Http400Exception.hpp"
#include "Http401Exception.hpp"
#include "Http403Exception.hpp"
#include "Http404Exception.hpp"
#include "Http405Exception.hpp"
#include "Http500Exception.hpp"
#include "Http501Exception.hpp"

#endif
