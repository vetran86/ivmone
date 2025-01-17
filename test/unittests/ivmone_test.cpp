// ivmone: Fast Ethereum Virtual Machine implementation
// Copyright 2019-2020 The ivmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include <ivmc/ivmc.hpp>
#include <ivmone/ivmone.h>
#include <gtest/gtest.h>

TEST(ivmone, info)
{
    auto vm = ivmc::VM{ivmc_create_ivmone()};
    EXPECT_STREQ(vm.name(), "ivmone");
    EXPECT_STREQ(vm.version(), PROJECT_VERSION);
    EXPECT_TRUE(vm.is_abi_compatible());
}

TEST(ivmone, capabilities)
{
    auto vm = ivmc_create_ivmone();
    EXPECT_EQ(vm->get_capabilities(vm), ivmc_capabilities_flagset{IVMC_CAPABILITY_EVM1});
    vm->destroy(vm);
}

TEST(ivmone, set_option_invalid)
{
    auto vm = ivmc_create_ivmone();
    ASSERT_NE(vm->set_option, nullptr);
    EXPECT_EQ(vm->set_option(vm, "", ""), IVMC_SET_OPTION_INVALID_NAME);
    EXPECT_EQ(vm->set_option(vm, "o", ""), IVMC_SET_OPTION_INVALID_NAME);
    EXPECT_EQ(vm->set_option(vm, "0", ""), IVMC_SET_OPTION_INVALID_NAME);
    vm->destroy(vm);
}

TEST(ivmone, set_option_optimization_level)
{
    auto vm = ivmc::VM{ivmc_create_ivmone()};
    EXPECT_EQ(vm.set_option("O", ""), IVMC_SET_OPTION_INVALID_VALUE);
    EXPECT_EQ(vm.set_option("O", "0"), IVMC_SET_OPTION_SUCCESS);
    EXPECT_EQ(vm.set_option("O", "1"), IVMC_SET_OPTION_INVALID_VALUE);
    EXPECT_EQ(vm.set_option("O", "2"), IVMC_SET_OPTION_SUCCESS);
    EXPECT_EQ(vm.set_option("O", "3"), IVMC_SET_OPTION_INVALID_VALUE);

    EXPECT_EQ(vm.set_option("O", "20"), IVMC_SET_OPTION_INVALID_VALUE);
    EXPECT_EQ(vm.set_option("O", "21"), IVMC_SET_OPTION_INVALID_VALUE);
    EXPECT_EQ(vm.set_option("O", "22"), IVMC_SET_OPTION_INVALID_VALUE);
}
