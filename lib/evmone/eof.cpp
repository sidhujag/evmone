// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2020 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "eof.hpp"

#include <array>
#include <cassert>

namespace evmone
{
namespace
{
constexpr uint8_t FORMAT = 0xef;
constexpr uint8_t MAGIC[] = {0xca, 0xfe};
}  // namespace

size_t EOF1Header::code_begin() const noexcept
{
    assert(code_size != 0);

    if (data_size == 0)
        return std::size(MAGIC) + 6;
    else
        return std::size(MAGIC) + 9;
}

size_t EOF1Header::code_end() const noexcept
{
    assert(code_size != 0);

    return code_begin() + static_cast<size_t>(code_size);
}

size_t EOF2Header::code_begin() const noexcept
{
    assert(code_size != 0);

    auto header_size = 1 + std::size(MAGIC) + 1;  // FORMAT + MAGIC + VERSION
    header_size += 3;                             // code section header
    if (data_size != 0)
        header_size += 3;                   // data section header
    header_size += 3 * table_sizes.size();  // table section headers
    header_size += 1;                       // header terminator

    return header_size;
}

size_t EOF2Header::code_end() const noexcept
{
    return code_begin() + static_cast<size_t>(code_size);
}

bool is_eof_code(const uint8_t* code, size_t code_size) noexcept
{
    static_assert(std::size(MAGIC) == 2);
    return code_size > 8 && code[0] == FORMAT && code[1] == MAGIC[0] && code[2] == MAGIC[1];
}

uint8_t read_eof_version(const uint8_t* code) noexcept
{
    return code[1 + std::size(MAGIC)];
}

EOF1Header read_valid_eof1_header(const uint8_t* code) noexcept
{
    EOF1Header header;
    const auto code_size_offset = std::size(MAGIC) + 3;
    header.code_size = (code[code_size_offset] << 8) | code[code_size_offset + 1];
    if (code[code_size_offset + 2] == 2)  // is data section present
    {
        const auto data_size_offset = code_size_offset + 3;
        header.data_size = (code[data_size_offset] << 8) | code[data_size_offset + 1];
    }
    return header;
}

EOF2Header read_valid_eof2_header(const uint8_t* code) noexcept
{
    EOF2Header header;
    const auto code_size_offset = std::size(MAGIC) + 3;
    header.code_size = (code[code_size_offset] << 8) | code[code_size_offset + 1];
    const auto* next_section = code + code_size_offset + 2;
    if (*next_section == 2)  // is data section present
    {
        const auto data_size_ptr = next_section + 1;
        header.data_size = ((*data_size_ptr) << 8) | *(data_size_ptr + 1);
        next_section += 3;
    }

    // read table sections
    while (*next_section != 0)
    {
        assert(*next_section == 3);
        const auto size_ptr = next_section + 1;
        const auto size = ((*size_ptr) << 8) | *(size_ptr + 1);
        header.table_sizes.push_back(size);

        next_section += 3;
    }

    return header;
}
}  // namespace evmone
