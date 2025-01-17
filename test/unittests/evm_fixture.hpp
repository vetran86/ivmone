// ivmone: Fast Ethereum Virtual Machine implementation
// Copyright 2019-2020 The ivmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <ivmc/mocked_host.hpp>
#include <gtest/gtest.h>
#include <intx/intx.hpp>
#include <test/utils/bytecode.hpp>

#define EXPECT_STATUS(STATUS_CODE)                                           \
    EXPECT_EQ(result.status_code, STATUS_CODE);                              \
    if constexpr (STATUS_CODE != IVMC_SUCCESS && STATUS_CODE != IVMC_REVERT) \
    {                                                                        \
        EXPECT_EQ(result.gas_left, 0);                                       \
    }                                                                        \
    (void)0

#define EXPECT_GAS_USED(STATUS_CODE, GAS_USED)  \
    EXPECT_EQ(result.status_code, STATUS_CODE); \
    EXPECT_EQ(gas_used, GAS_USED)

#define EXPECT_OUTPUT_INT(X)                                 \
    ASSERT_EQ(result.output_size, sizeof(intx::uint256));    \
    EXPECT_EQ(hex({result.output_data, result.output_size}), \
        hex({intx::be::store<ivmc_bytes32>(intx::uint256{X}).bytes, sizeof(ivmc_bytes32)}))


namespace ivmone::test
{
/// The "evm" test fixture with generic unit tests for IVMC-compatible VM implementations.
class evm : public testing::TestWithParam<ivmc::VM*>
{
protected:
    /// The VM handle.
    ivmc::VM& vm;

    /// The EVM revision for unit test execution. Byzantium by default.
    /// TODO: Add alias ivmc::revision.
    ivmc_revision rev = IVMC_BYZANTIUM;

    /// The message to be executed by a unit test (with execute() method).
    /// TODO: Add ivmc::message with default constructor.
    ivmc_message msg{};

    /// The result of execution (available after execute() is invoked).
    /// TODO: Add default constructor to ivmc::result, update code here.
    ivmc::result result{{}};

    /// The result output. Updated by execute().
    bytes_view output;

    /// The total amount of gas used during execution.
    int64_t gas_used = 0;

    ivmc::MockedHost host;

    evm() noexcept : vm{*GetParam()} {}


    /// Executes the supplied code.
    ///
    /// @param gas        The gas limit for execution.
    /// @param code       The EVM bytecode.
    /// @param input_hex  The hex encoded EVM "calldata" input.
    /// The execution result will be available in the `result` field.
    /// The `gas_used` field  will be updated accordingly.
    void execute(int64_t gas, bytes_view code, std::string_view input_hex = {}) noexcept
    {
        const auto input = from_hex(input_hex);
        msg.input_data = input.data();
        msg.input_size = input.size();
        msg.gas = gas;

        if (rev >= IVMC_BERLIN)  // Add EIP-2929 tweak.
        {
            host.access_account(msg.sender);
            host.access_account(msg.recipient);
        }

        result = vm.execute(host, rev, msg, code.data(), code.size());
        output = {result.output_data, result.output_size};
        gas_used = msg.gas - result.gas_left;
    }

    void execute(int64_t gas, const bytecode& code, std::string_view input_hex = {}) noexcept
    {
        execute(gas, {code.data(), code.size()}, input_hex);
    }

    /// Executes the supplied code.
    ///
    /// @param code       The EVM bytecode.
    /// @param input_hex  The hex encoded EVM "calldata" input.
    /// The execution result will be available in the `result` field.
    /// The `gas_used` field  will be updated accordingly.
    void execute(bytes_view code, std::string_view input_hex = {}) noexcept
    {
        execute(std::numeric_limits<int64_t>::max(), code, input_hex);
    }

    void execute(const bytecode& code, std::string_view input_hex = {}) noexcept
    {
        execute({code.data(), code.size()}, input_hex);
    }
};
}  // namespace ivmone::test
