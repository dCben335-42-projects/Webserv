#ifndef HTTP_EXCEPTIONS_HPP
#define HTTP_EXCEPTIONS_HPP

#define HTTP_200_MSG "OK"
#define HTTP_201_MSG "Created"
#define HTTP_202_MSG "Accepted"
#define HTTP_203_MSG "Non-Authoritative Information"
#define HTTP_204_MSG "No Content"
#define HTTP_205_MSG "Reset Content"
#define HTTP_206_MSG "Partial Content"

#define HTTP_300_MSG "Multiple Choices"
#define HTTP_301_MSG "Moved Permanently"
#define HTTP_302_MSG "Found"
#define HTTP_303_MSG "See Other"
#define HTTP_307_MSG "Temporary Redirect"
#define HTTP_308_MSG "Permanent Redirect"

#define HTTP_410_MSG "Gone"
#define HTTP_418_MSG "I'm a teapot"
#define HTTP_429_MSG "Too Many Requests"

#define HTTP_502_MSG "Bad Gateway"
#define HTTP_503_MSG "Service Unavailable"

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
#include "Http411Exception.hpp"
#include "Http413Exception.hpp"

#include "Http500Exception.hpp"
#include "Http501Exception.hpp"
#include "Http504Exception.hpp"
#include "Http505Exception.hpp"

#endif
