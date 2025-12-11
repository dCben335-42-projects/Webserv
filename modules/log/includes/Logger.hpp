#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "../srcs/Log/Log.hpp"
#include "../srcs/logger.tpp"
#include "../srcs/vector.tpp"
#include "../srcs/map.tpp"

typedef enum LogFormat
{
	LOG_FORMAT_PLAIN,
	LOG_FORMAT_JSON,
	LOG_FORMAT_XML
} LogFormat;

void log(const char *s);

#endif