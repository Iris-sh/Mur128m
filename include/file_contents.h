#ifndef FILE_CONTENTS_H
#define FILE_CONTENTS_H
#include <utility>
#include <vector>
#include <cstdint>


/** Return codes from the function get_contents. */
enum class Get_contents_return_code{
    Normal,           ///< This code means success.
    Impossible_open,  ///< This code means that file could not open.
    Read_error        ///< This code means that an error occurred while reading the file.
};

using Contents = std::pair<Get_contents_return_code, std::vector<std::uint8_t>>;

Contents get_contents(const char* name);
#endif
