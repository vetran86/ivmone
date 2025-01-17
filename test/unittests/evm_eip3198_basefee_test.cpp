// ivmone: Fast Ethereum Virtual Machine implementation
// Copyright 2021 The ivmone Authors.
// SPDX-License-Identifier: Apache-2.0

/// This file contains EVM unit tests for EIP-3198 "BASEFEE opcode"
/// https://eips.ethereum.org/EIPS/eip-3198

#include "evm_fixture.hpp"

using namespace ivmc::literals;
using ivmone::test::evm;

TEST_P(evm, basefee_pre_london)
{
    rev = IVMC_BERLIN;
    const auto code = bytecode{OP_BASEFEE};

    execute(code);
    EXPECT_STATUS(IVMC_UNDEFINED_INSTRUCTION);
}

TEST_P(evm, basefee_nominal_case)
{
    // https://eips.ethereum.org/EIPS/eip-3198#nominal-case
    rev = IVMC_LONDON;
    host.tx_context.block_base_fee = ivmc::bytes32{7};

    execute(bytecode{} + OP_BASEFEE + OP_STOP);
    EXPECT_GAS_USED(IVMC_SUCCESS, 2);

    execute(bytecode{} + OP_BASEFEE + ret_top());
    EXPECT_GAS_USED(IVMC_SUCCESS, 17);
    EXPECT_OUTPUT_INT(7);
}
