// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2020 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <evmc/utils.h>
#include <stddef.h>
#include <cstdint>
#include <vector>

namespace evmone
{
struct EOF1Header
{
    int code_size = 0;
    int data_size = 0;

    EVMC_EXPORT size_t code_begin() const noexcept;
    EVMC_EXPORT size_t code_end() const noexcept;
};

struct EOF2Header
{
    int code_size = 0;
    int data_size = 0;
    std::vector<int> table_sizes;

    size_t code_begin() const noexcept;
    size_t code_end() const noexcept;
};

// Checks if code starts with EOF FORMAT + MAGIC, doesn't validate the format.
bool is_eof_code(const uint8_t* code, size_t code_size) noexcept;

uint8_t read_eof_version(const uint8_t* code) noexcept;

// Reads the section sizes assuming that code has valid format.
// (must be true for all EOF contracts on-chain)
EVMC_EXPORT EOF1Header read_valid_eof1_header(const uint8_t* code) noexcept;

// Reads the section sizes assuming that code has valid format.
// (must be true for all EOF contracts on-chain)
EOF2Header read_valid_eof2_header(const uint8_t* code) noexcept;
}  // namespace evmone
