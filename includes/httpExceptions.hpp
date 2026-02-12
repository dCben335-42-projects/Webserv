#ifndef HTTP_EXCEPTIONS_HPP
#define HTTP_EXCEPTIONS_HPP

#define HTTP_200_MSG "OK"
#define HTTP_201_MSG "Created"
#define HTTP_204_MSG "No Content"
#define HTTP_300_MSG "Multiple Choices"
#define HTTP_301_MSG "Moved Permanently"
#define HTTP_302_MSG "Found"

#include "Http300Exception.hpp"
#include "Http301Exception.hpp"
#include "Http400Exception.hpp"
#include "Http401Exception.hpp"
#include "Http403Exception.hpp"
#include "Http404Exception.hpp"
#include "Http405Exception.hpp"
#include "Http406Exception.hpp"
#include "Http408Exception.hpp"
#include "Http409Exception.hpp"
#include "Http413Exception.hpp"
#include "Http500Exception.hpp"
#include "Http501Exception.hpp"
#include "Http504Exception.hpp"

#endif
