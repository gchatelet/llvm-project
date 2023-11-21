//===-- Properties of floating point numbers --------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_FPUTIL_FLOATREPRESENTATION_H
#define LLVM_LIBC_SRC___SUPPORT_FPUTIL_FLOATREPRESENTATION_H

#include "src/__support/CPP/expected.h"
#include "src/__support/CPP/type_traits.h"
#include "src/__support/UInt128.h"
#include <stdint.h>

namespace LIBC_NAMESPACE {
namespace fputil {

#ifndef P
#define P(var)
#endif
#ifndef D
#define D(var)
#endif

enum class FPType {
  IEEE754_Binary16,
  IEEE754_Binary32,
  IEEE754_Binary64,
  IEEE754_Binary128,
  X86_Binary80,
};

enum class FPEncoding {
  IEEE754,
  X86_ExtendedPrecision,
};

enum class fp_error { overflow, underflow };

// template <typename FP> static constexpr FPType fp_type() {
//   static_assert(cpp::is_floating_point_v<FP>);        // floating point
//   static_assert(cpp::numeric_limits<FP>::radix == 2); // binary
//   switch (cpp::numeric_limits<FP>::digits) {
//   default:
//     __builtin_unreachable();
//   case 11:
//     return FPType::IEEE754_Binary16;
//   case 24:
//     return FPType::IEEE754_Binary32;
//   case 53:
//     return FPType::IEEE754_Binary64;
//   case 64:
//     return FPType::X86_Binary80;
//   case 113:
//     return FPType::IEEE754_Binary128;
//   }
// }

// TODO: move these utilities elsewhere
template <typename T, size_t count> static constexpr T mask_trailing_ones() {
  static_assert(cpp::is_unsigned_v<T>);
  constexpr unsigned t_bits = CHAR_BIT * sizeof(T);
  static_assert(count <= t_bits && "Invalid bit index");
  return count == 0 ? 0 : (T(-1) >> (t_bits - count));
}

template <typename T, size_t count> static constexpr T mask_leading_ones() {
  constexpr unsigned t_bits = CHAR_BIT * sizeof(T);
  return ~mask_trailing_ones<T, t_bits - count>();
}

template <typename T, size_t count> static constexpr T mask_trailing_zeros() {
  constexpr unsigned t_bits = CHAR_BIT * sizeof(T);
  return mask_leading_ones<T, t_bits - count>();
}

template <typename T, size_t count> static constexpr T mask_leading_zeros() {
  constexpr unsigned t_bits = CHAR_BIT * sizeof(T);
  return mask_trailing_ones<T, t_bits - count>();
}
template <typename T> int leading_zeroes(T value);
template <> LIBC_INLINE int leading_zeroes(uint16_t value) {
  return value == 0 ? sizeof(uint16_t) * 8 : __builtin_clzs(value);
}
template <> LIBC_INLINE int leading_zeroes(uint32_t value) {
  return value == 0 ? sizeof(uint32_t) * 8 : __builtin_clz(value);
}
template <> LIBC_INLINE int leading_zeroes(uint64_t value) {
  return value == 0 ? sizeof(uint64_t) * 8 : __builtin_clzl(value);
}
template <> LIBC_INLINE int leading_zeroes(UInt128 value) {
  return value == 0 ? sizeof(UInt128) * 8
                    : (value > UInt128(UINT64_MAX)
                           ? __builtin_clzl(value >> 64)
                           : __builtin_clzl(value & UINT64_MAX) + 64);
}

template <FPType> struct fp_type_traits_base {};

template <> struct fp_type_traits_base<FPType::IEEE754_Binary16> {
  LIBC_INLINE_VAR static constexpr int FP_BITS = 16;
  LIBC_INLINE_VAR static constexpr int SIG_BITS = 10;
  LIBC_INLINE_VAR static constexpr int EXP_BITS = 5;
  LIBC_INLINE_VAR static constexpr auto ENCODING = FPEncoding::IEEE754;
  using UIntType = uint16_t;
};

template <> struct fp_type_traits_base<FPType::IEEE754_Binary32> {
  LIBC_INLINE_VAR static constexpr int FP_BITS = 32;
  LIBC_INLINE_VAR static constexpr int SIG_BITS = 23;
  LIBC_INLINE_VAR static constexpr int EXP_BITS = 8;
  LIBC_INLINE_VAR static constexpr auto ENCODING = FPEncoding::IEEE754;
  using UIntType = uint32_t;
};

template <> struct fp_type_traits_base<FPType::IEEE754_Binary64> {
  LIBC_INLINE_VAR static constexpr int FP_BITS = 64;
  LIBC_INLINE_VAR static constexpr int SIG_BITS = 52;
  LIBC_INLINE_VAR static constexpr int EXP_BITS = 11;
  LIBC_INLINE_VAR static constexpr auto ENCODING = FPEncoding::IEEE754;
  using UIntType = uint64_t;
};

template <> struct fp_type_traits_base<FPType::IEEE754_Binary128> {
  LIBC_INLINE_VAR static constexpr int FP_BITS = 128;
  LIBC_INLINE_VAR static constexpr int SIG_BITS = 112;
  LIBC_INLINE_VAR static constexpr int EXP_BITS = 15;
  LIBC_INLINE_VAR static constexpr auto ENCODING = FPEncoding::IEEE754;
  using UIntType = UInt128;
};

template <> struct fp_type_traits_base<FPType::X86_Binary80> {
  LIBC_INLINE_VAR static constexpr int FP_BITS = 80;
  LIBC_INLINE_VAR static constexpr int SIG_BITS = 64;
  LIBC_INLINE_VAR static constexpr int EXP_BITS = 15;
  LIBC_INLINE_VAR static constexpr auto ENCODING =
      FPEncoding::X86_ExtendedPrecision;
  using UIntType = UInt128;
};

template <FPType fp_type>
struct fp_repesentation : public fp_type_traits_base<fp_type> {
  using base = fp_type_traits_base<fp_type>;
  using base::ENCODING;
  using base::EXP_BITS;
  using base::FP_BITS;
  using base::SIG_BITS;
  using typename base::UIntType;
  static_assert(cpp::is_unsigned_v<UIntType>);

  LIBC_INLINE_VAR static constexpr bool HAS_IMPLICIT_SIG_BIT =
      (ENCODING == FPEncoding::IEEE754);

  LIBC_INLINE_VAR static constexpr int STORAGE_BITS =
      sizeof(UIntType) * CHAR_BIT;
  static_assert(STORAGE_BITS >= FP_BITS);

  // The number of bits to represent sign.
  // For documentation purpose, always 1.
  LIBC_INLINE_VAR static constexpr int SIGN_BITS = 1;
  static_assert(SIGN_BITS + EXP_BITS + SIG_BITS == FP_BITS);

  // The exponent bias. Always positive.
  LIBC_INLINE_VAR static constexpr int32_t EXP_BIAS =
      (1U << (EXP_BITS - 1U)) - 1U;
  static_assert(EXP_BIAS > 0);

  // The exponent value for denormal numbers.
  LIBC_INLINE_VAR static constexpr int32_t EXP_DENORM = -EXP_BIAS;
  // The minimum exponent value for normal numbers.
  LIBC_INLINE_VAR static constexpr int32_t EXP_MIN = EXP_DENORM + 1;
  // The maximum exponent value for normal numbers.
  LIBC_INLINE_VAR static constexpr int32_t EXP_MAX = EXP_BIAS;

  // Shifts
  LIBC_INLINE_VAR static constexpr int SIG_MASK_SHIFT = 0;
  LIBC_INLINE_VAR static constexpr int EXP_MASK_SHIFT = SIG_BITS;
  LIBC_INLINE_VAR static constexpr int SIGN_MASK_SHIFT = SIG_BITS + EXP_BITS;
  LIBC_INLINE_VAR static constexpr int SIG_HIGH_BIT_SHIFT = SIG_BITS - 1;

  // Masks
  LIBC_INLINE_VAR static constexpr UIntType SIG_MASK =
      mask_trailing_ones<UIntType, SIG_BITS>() << SIG_MASK_SHIFT;
  LIBC_INLINE_VAR static constexpr UIntType EXP_MASK =
      mask_trailing_ones<UIntType, EXP_BITS>() << EXP_MASK_SHIFT;
  LIBC_INLINE_VAR static constexpr UIntType SIGN_MASK =
      mask_trailing_ones<UIntType, SIGN_BITS>() << SIGN_MASK_SHIFT;
  LIBC_INLINE_VAR static constexpr UIntType FP_MASK =
      mask_trailing_ones<UIntType, FP_BITS>();
  // Masks are disjoint
  static_assert((SIG_MASK & EXP_MASK & SIGN_MASK) == 0);
  // Masks cover all the bits
  static_assert((SIG_MASK | EXP_MASK | SIGN_MASK) == FP_MASK);

  // Representation observer
  static UIntType sign_bits(UIntType rep) { return rep & SIGN_MASK; }
  static UIntType exp_bits(UIntType rep) { return rep & EXP_MASK; }
  static UIntType sig_bits(UIntType rep) { return rep & SIG_MASK; }

  // Default NaN Payload is the top most significant bit
  LIBC_INLINE_VAR static constexpr UIntType DEFAULT_NAN_PAYLOAD =
      mask_trailing_ones<UIntType, 1>() << SIG_HIGH_BIT_SHIFT;

  static bool exp_all_zeroes(UIntType rep) { return exp_bits(rep) == 0; }
  static bool exp_all_ones(UIntType rep) { return exp_bits(rep) == EXP_MASK; }
  static bool sig_all_zeroes(UIntType rep) { return sig_bits(rep) == 0; }

public:
  static bool sign(UIntType rep) { return sign_bits(rep); }
  static bool is_nan(UIntType rep) {
    return exp_all_ones(rep) && !sig_all_zeroes(rep);
  }
  static bool is_inf(UIntType rep) {
    if constexpr (ENCODING == FPEncoding::X86_ExtendedPrecision) {
      constexpr UIntType BITS_62_0 = DEFAULT_NAN_PAYLOAD - 1;
      return exp_all_ones(rep) && ((rep & BITS_62_0) == 0);
    }
    return exp_all_ones(rep) && sig_all_zeroes(rep);
  }
  static bool is_zero(UIntType rep) {
    return exp_all_zeroes(rep) && sig_all_zeroes(rep);
  }
  static bool is_normal(UIntType rep) {
    return !exp_all_ones(rep) && !exp_all_zeroes(rep);
  }
  static bool is_denorm(UIntType rep) {
    return exp_all_zeroes(rep) && !sig_all_zeroes(rep);
  }
  static int32_t get_exponent(UIntType rep) {
    return int32_t((rep & EXP_MASK) >> SIG_BITS) - EXP_BIAS - SIG_BITS;
  }
  static UIntType get_significand(UIntType rep) {
    if (ENCODING == FPEncoding::IEEE754 && !is_denorm(rep))
      return (UIntType(1) << SIG_BITS) | sig_bits(rep);
    return sig_bits(rep);
  }
  static UIntType zero(bool sign) { return sign ? SIGN_MASK : 0; }
  static UIntType inf(bool sign) {
    P("inf()");
    return zero(sign) | EXP_MASK;
  }
  static UIntType nan(bool sign, UIntType payload = 0) {
    P("nan()");
    if (payload == 0)
      payload = DEFAULT_NAN_PAYLOAD;
    else
      payload &= SIG_MASK;
    return inf(sign) | payload;
  }
  // Simply pack provided values without performing any checks. This may result
  // in invalid representation.
  static UIntType assemble(bool sign, int32_t exponent, UIntType significand) {
    // assert(exponent >= EXP_DENORM && exponent <= EXP_MAX);
    P("assemble()");
    D(sign);
    D(exponent);
    D(significand);
    D(significand & SIG_MASK);
    const UIntType biased_exponent = exponent + EXP_BIAS;
    D(biased_exponent);
    return zero(sign) | (biased_exponent << SIG_BITS) |
           (significand & SIG_MASK);
  }
  // This normalizes exponent and significand for the underlying representation.
  static cpp::expected<UIntType, fp_error> number(bool sign, int32_t exponent,
                                                  UIntType significand) {
    if (significand == 0)
      return zero(sign);
    static_assert(sizeof(UIntType) * CHAR_BIT > SIG_BITS);
    constexpr int HIDDEN_SIG_BITS = HAS_IMPLICIT_SIG_BIT ? 1 : 0;
    constexpr int WORKING_SIG_EXTRA_BITS =
        STORAGE_BITS - SIG_BITS - HIDDEN_SIG_BITS;
    constexpr UIntType MIN_WORKING_SIG =
        mask_trailing_ones<UIntType, WORKING_SIG_EXTRA_BITS>();
    constexpr UIntType MAX_WORKING_SIG =
        mask_trailing_zeros<UIntType, WORKING_SIG_EXTRA_BITS>();
    P("constants");
    D(STORAGE_BITS);
    D(HIDDEN_SIG_BITS);
    D(FP_BITS);
    D(SIG_BITS);
    D(EXP_BITS);
    D(WORKING_SIG_EXTRA_BITS);
    D(SIG_MASK);
    D(EXP_MASK);
    D(SIGN_MASK);
    D(FP_MASK);
    D(MIN_WORKING_SIG);
    D(MAX_WORKING_SIG);
    D(EXP_BIAS);
    D(EXP_DENORM);
    D(EXP_MIN);
    D(EXP_MAX);

    P("arguments");
    D(sign);
    D(exponent);
    D(significand);
    P("start");
    // The working significand format is a normalized form where the leading bit
    // is moved to the 'UIntType' highest bit. It is stored in 'UIntType' which
    // size is larger than 'SIG_BITS', this provides extra bits to detect
    // overflow / underflow. For IEEE754 Binary format we remove one extra bit
    // to account for the hidden leading bit.
    // e.g., for IEEE754_Binary16 where SIG_BITS == 10 and UIntType is uint16_t.
    const int sig_shl = leading_zeroes(significand);
    const UIntType working_sig = significand << sig_shl;
    const int32_t working_exp =
        exponent - sig_shl + STORAGE_BITS - HIDDEN_SIG_BITS;
    D(sig_shl);
    D(working_sig);
    D(working_exp);
    if (working_exp < EXP_MIN) {
      P("working_exp < EXP_MIN");
      const bool sig_underflow = working_sig <= MIN_WORKING_SIG;
      const int sig_shr = EXP_MIN - working_exp - 1;
      D(sig_underflow);
      D(sig_shr);
      if (sig_shr > SIG_BITS || (sig_shr == SIG_BITS && sig_underflow))
        return cpp::unexpected(fp_error::underflow);
      // denormal
      return assemble(sign, EXP_DENORM,
                      working_sig >> (sig_shr + WORKING_SIG_EXTRA_BITS));
    }
    P("working_exp >= EXP_MIN");
    const bool sig_overflow = working_sig > MAX_WORKING_SIG;
    D(sig_overflow);
    if (working_exp > EXP_MAX || (working_exp == EXP_MAX && sig_overflow))
      return cpp::unexpected(fp_error::overflow);
    // normal
    return assemble(sign, working_exp, working_sig >> WORKING_SIG_EXTRA_BITS);
  }
};

} // namespace fputil
} // namespace LIBC_NAMESPACE

#endif // LLVM_LIBC_SRC___SUPPORT_FPUTIL_FLOATREPRESENTATION_H
