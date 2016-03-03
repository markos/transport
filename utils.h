#ifndef DCENGINES_UTILS_H
#define DCENGINES_UTILS_H

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include "common.h"

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

void hexdump(void *ptr, size_t len);
std::string random_string( size_t length );

transport::MessageItem makeRandomMessage(uint64_t id, size_t size);

#endif