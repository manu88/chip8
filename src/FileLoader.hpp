//
//  FileLoader.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 25/03/2025.
//

#pragma once
#include <stdint.h>
#include <vector>

namespace Chip8 {
using Bytes = std::vector<uint16_t>;

Bytes loadFile(const std::string &path);
} // namespace Chip8
