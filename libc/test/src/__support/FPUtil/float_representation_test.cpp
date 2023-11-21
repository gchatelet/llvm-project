//===-- Unittests for FloatRepresentation ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/bit.h"
#include "src/__support/CPP/type_traits.h"
#include "src/__support/FPUtil/FloatRepresentation.h"
#include "test/UnitTest/Test.h"

namespace LIBC_NAMESPACE {
namespace fputil {
namespace {

constexpr __uint128_t operator""_u128(const char *x) {
  __uint128_t y = 0;
  for (int i = 2; x[i] != '\0'; ++i) {
    y *= 16ull;
    if ('0' <= x[i] && x[i] <= '9')
      y += x[i] - '0';
    else if ('A' <= x[i] && x[i] <= 'F')
      y += x[i] - 'A' + 10;
    else if ('a' <= x[i] && x[i] <= 'f')
      y += x[i] - 'a' + 10;
  }
  return y;
}

enum FPValue : uint8_t {
  PosInf,
  PosLargestNormal,
  Pos1,
  Pos0_1,
  PosSmallestNormal,
  PosLargestDenormal,
  PosSmallestDenormal,
  Pos0,
  Neg0,
  NegSmallestDenormal,
  NegLargestDenormal,
  NegSmallestNormal,
  Neg0_1,
  Neg1,
  NegLargestNormal,
  NegInf,
  QuietNaN,
  SignalingNaN,
};

enum class FPCategory { Inf, NaN, Normal, Denormal, Zero };

FPCategory getFPCategory(FPValue value) {
  switch (value) {
  case PosInf:
  case NegInf:
    return FPCategory::Inf;
  case PosLargestNormal:
  case Pos1:
  case Pos0_1:
  case PosSmallestNormal:
  case NegSmallestNormal:
  case Neg0_1:
  case Neg1:
  case NegLargestNormal:
    return FPCategory::Normal;
  case PosLargestDenormal:
  case PosSmallestDenormal:
  case NegSmallestDenormal:
  case NegLargestDenormal:
    return FPCategory::Denormal;
  case Pos0:
  case Neg0:
    return FPCategory::Zero;
  case QuietNaN:
  case SignalingNaN:
    return FPCategory::NaN;
  }
}

struct IEEE754_Binary16_TestCase {
  FPValue semantic;
  uint16_t representation;
} kIEEE754_Binary16TestCases[] = {
    {PosInf, /*                                                      */ 0x7C00},
    {PosLargestNormal, /*                                            */ 0x7BFF},
    {Pos1, /*                                                        */ 0x3C00},
    {Pos0_1, /*                                                      */ 0x2E66},
    {PosSmallestNormal, /*                                           */ 0x0400},
    {PosLargestDenormal, /*                                          */ 0x03FF},
    {PosSmallestDenormal, /*                                         */ 0x0001},
    {Pos0, /*                                                        */ 0x0000},
    {Neg0, /*                                                        */ 0x8000},
    {NegSmallestDenormal, /*                                         */ 0x8001},
    {NegLargestDenormal, /*                                          */ 0x83FF},
    {NegSmallestNormal, /*                                           */ 0x8400},
    {Neg0_1, /*                                                      */ 0xAE66},
    {Neg1, /*                                                        */ 0xBC00},
    {NegLargestNormal, /*                                            */ 0xFBFF},
    {NegInf, /*                                                      */ 0xFC00},
    {QuietNaN, /*                                                    */ 0x7E00},
    {SignalingNaN, /*                                                */ 0x7D00},
};

struct IEEE754_Binary32_TestCase {
  FPValue semantic;
  uint32_t representation;
} kIEEE754_Binary32TestCases[] = {
    {PosInf, /*                                                  */ 0x7F800000},
    {PosLargestNormal, /*                                        */ 0x7F7FFFFF},
    {Pos1, /*                                                    */ 0x3F800000},
    {Pos0_1, /*                                                  */ 0x3DCCCCCD},
    {PosSmallestNormal, /*                                       */ 0x00800000},
    {PosLargestDenormal, /*                                      */ 0x007FFFFF},
    {PosSmallestDenormal, /*                                     */ 0x00000001},
    {Pos0, /*                                                    */ 0x00000000},
    {Neg0, /*                                                    */ 0x80000000},
    {NegSmallestDenormal, /*                                     */ 0x80000001},
    {NegLargestDenormal, /*                                      */ 0x807FFFFF},
    {NegSmallestNormal, /*                                       */ 0x80800000},
    {Neg0_1, /*                                                  */ 0xBDCCCCCD},
    {Neg1, /*                                                    */ 0xBF800000},
    {NegLargestNormal, /*                                        */ 0xFF7FFFFF},
    {NegInf, /*                                                  */ 0xFF800000},
    {QuietNaN, /*                                                */ 0x7FC00000},
    {SignalingNaN, /*                                            */ 0x7FA00000},
};

struct IEEE754_Binary64_TestCase {
  FPValue semantic;
  uint64_t representation;
} kIEEE754_Binary64TestCases[] = {
    {PosInf, /*                                          */ 0x7FF0000000000000},
    {PosLargestNormal, /*                                */ 0x7FEFFFFFFFFFFFFF},
    {Pos1, /*                                            */ 0x3FF0000000000000},
    {Pos0_1, /*                                          */ 0x3FB999999999999A},
    {PosSmallestNormal, /*                               */ 0x0010000000000000},
    {PosLargestDenormal, /*                              */ 0x000FFFFFFFFFFFFF},
    {PosSmallestDenormal, /*                             */ 0x0000000000000001},
    {Pos0, /*                                            */ 0x0000000000000000},
    {Neg0, /*                                            */ 0x8000000000000000},
    {NegSmallestDenormal, /*                             */ 0x8000000000000001},
    {NegLargestDenormal, /*                              */ 0x800FFFFFFFFFFFFF},
    {NegSmallestNormal, /*                               */ 0x8010000000000000},
    {Neg0_1, /*                                          */ 0xBFB999999999999A},
    {Neg1, /*                                            */ 0xBFF0000000000000},
    {NegLargestNormal, /*                                */ 0xFFEFFFFFFFFFFFFF},
    {NegInf, /*                                          */ 0xFFF0000000000000},
    {QuietNaN, /*                                        */ 0x7FF8000000000000},
    {SignalingNaN, /*                                    */ 0x7FF4000000000000},
};

struct IEEE754_Binary128_TestCase {
  FPValue semantic;
  UInt128 representation;
} kIEEE754_Binary128TestCases[] = {
    {PosInf, /*                     */ 0x7FFF0000000000000000000000000000_u128},
    {PosLargestNormal, /*           */ 0x7FFEFFFFFFFFFFFFFFFFFFFFFFFFFFFF_u128},
    {Pos1, /*                       */ 0x3FFF0000000000000000000000000000_u128},
    {Pos0_1, /*                     */ 0x3FFB999999999999A000000000000000_u128},
    {PosSmallestNormal, /*          */ 0x00010000000000000000000000000000_u128},
    {PosLargestDenormal, /*         */ 0x0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF_u128},
    {PosSmallestDenormal, /*        */ 0x00000000000000000000000000000001_u128},
    {Pos0, /*                       */ 0x00000000000000000000000000000000_u128},
    {Neg0, /*                       */ 0x80000000000000000000000000000000_u128},
    {NegSmallestDenormal, /*        */ 0x80000000000000000000000000000001_u128},
    {NegLargestDenormal, /*         */ 0x8000FFFFFFFFFFFFFFFFFFFFFFFFFFFF_u128},
    {NegSmallestNormal, /*          */ 0x80010000000000000000000000000000_u128},
    {Neg0_1, /*                     */ 0xBFFB999999999999A000000000000000_u128},
    {Neg1, /*                       */ 0xBFFF0000000000000000000000000000_u128},
    {NegLargestNormal, /*           */ 0xFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFF_u128},
    {NegInf, /*                     */ 0xFFFF0000000000000000000000000000_u128},
    {QuietNaN, /*                   */ 0x7FFF8000000000000000000000000000_u128},
    {SignalingNaN, /*               */ 0x7FFF4000000000000000000000000000_u128},
};

struct X86_Binary80_TestCase {
  FPValue semantic;
  UInt128 representation;
} kX86_Binary80TestCases[] = {
    {PosInf, /*                     */ 0x0000000000007FFF8000000000000000_u128},
    {PosLargestNormal, /*           */ 0x0000000000007FFEFFFFFFFFFFFFFFFF_u128},
    {Pos1, /*                       */ 0x0000000000003FFF8000000000000000_u128},
    {Pos0_1, /*                     */ 0x0000000000003FFBCCCCCCCCCCCCD000_u128},
    {PosSmallestNormal, /*          */ 0x00000000000000018000000000000000_u128},
    {PosLargestDenormal, /*         */ 0x00000000000000007FFFFFFFFFFFFFFF_u128},
    {PosSmallestDenormal, /*        */ 0x00000000000000000000000000000001_u128},
    {Pos0, /*                       */ 0x00000000000000000000000000000000_u128},
    {Neg0, /*                       */ 0x00000000000080000000000000000000_u128},
    {NegSmallestDenormal, /*        */ 0x00000000000080000000000000000001_u128},
    {NegLargestDenormal, /*         */ 0x00000000000080007FFFFFFFFFFFFFFF_u128},
    {NegSmallestNormal, /*          */ 0x00000000000080018000000000000000_u128},
    {Neg0_1, /*                     */ 0x000000000000BFFBCCCCCCCCCCCCD000_u128},
    {Neg1, /*                       */ 0x000000000000BFFF8000000000000000_u128},
    {NegLargestNormal, /*           */ 0x000000000000FFFEFFFFFFFFFFFFFFFF_u128},
    {NegInf, /*                     */ 0x000000000000FFFF8000000000000000_u128},
    {QuietNaN, /*                   */ 0x0000000000007FFFC000000000000000_u128},
    {SignalingNaN, /*               */ 0x0000000000007FFFA000000000000000_u128},
};

#define FP_TEST(FP_TYPE)                                                       \
  TEST(LlvmLibcFloatRepresentationTest, FP_TYPE) {                             \
    constexpr auto type = FPType::FP_TYPE;                                     \
    using FP = fp_repesentation<type>;                                         \
    using UIntType = typename FP::UIntType;                                    \
    for (auto tc : k##FP_TYPE##TestCases) {                                    \
      const UIntType in_rep = tc.representation;                               \
      const auto category = getFPCategory(tc.semantic);                        \
      switch (category) {                                                      \
      case FPCategory::Inf:                                                    \
        EXPECT_TRUE(FP::is_inf(in_rep));                                       \
        break;                                                                 \
      case FPCategory::NaN:                                                    \
        EXPECT_TRUE(FP::is_nan(in_rep));                                       \
        break;                                                                 \
      case FPCategory::Normal:                                                 \
        EXPECT_TRUE(FP::is_normal(in_rep));                                    \
        break;                                                                 \
      case FPCategory::Denormal:                                               \
        EXPECT_TRUE(FP::is_denorm(in_rep));                                    \
        break;                                                                 \
      case FPCategory::Zero:                                                   \
        EXPECT_TRUE(FP::is_zero(in_rep));                                      \
        break;                                                                 \
      }                                                                        \
      if (category == FPCategory::Normal ||                                    \
          category == FPCategory::Denormal || category == FPCategory::Zero) {  \
        const bool sign = FP::sign(in_rep);                                    \
        const UIntType sig = FP::get_significand(in_rep);                      \
        const int32_t exponent = FP::get_exponent(in_rep);                     \
        const UIntType out_rep = FP::number(sign, exponent, sig);              \
        ASSERT_EQ(in_rep, out_rep);                                            \
      }                                                                        \
    }                                                                          \
  }

FP_TEST(IEEE754_Binary16)
FP_TEST(IEEE754_Binary32)
FP_TEST(IEEE754_Binary64)
FP_TEST(IEEE754_Binary128)
FP_TEST(X86_Binary80)

} // namespace
} // namespace fputil
} // namespace LIBC_NAMESPACE
