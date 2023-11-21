//===-- A self contained equivalent of std::limits --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_CPP_LIMITS_H
#define LLVM_LIBC_SRC___SUPPORT_CPP_LIMITS_H

#include "src/__support/macros/attributes.h"
#include <float.h>
#include <limits.h>
#include <math.h>

namespace LIBC_NAMESPACE {
namespace cpp {

// Some older gcc distributions don't define these for 32 bit targets.
#ifndef LLONG_MAX
constexpr size_t LLONG_BIT_WIDTH = sizeof(long long) * 8;
constexpr long long LLONG_MAX = ~0LL ^ (1LL << (LLONG_BIT_WIDTH - 1));
constexpr long long LLONG_MIN = 1LL << (LLONG_BIT_WIDTH - 1);
constexpr unsigned long long ULLONG_MAX = ~0ULL;
#endif

template <class T> class numeric_limits {
public:
  static constexpr T max();
  static constexpr T min();
};

// TODO: Add numeric_limits specializations as needed for new types.

template <> class numeric_limits<int> {
public:
  static constexpr int max() { return INT_MAX; }
  static constexpr int min() { return INT_MIN; }
};
template <> class numeric_limits<unsigned int> {
public:
  static constexpr unsigned int max() { return UINT_MAX; }
  static constexpr unsigned int min() { return 0; }
};
template <> class numeric_limits<long> {
public:
  static constexpr long max() { return LONG_MAX; }
  static constexpr long min() { return LONG_MIN; }
};
template <> class numeric_limits<unsigned long> {
public:
  static constexpr unsigned long max() { return ULONG_MAX; }
  static constexpr unsigned long min() { return 0; }
};
template <> class numeric_limits<long long> {
public:
  static constexpr long long max() { return LLONG_MAX; }
  static constexpr long long min() { return LLONG_MIN; }
};
template <> class numeric_limits<unsigned long long> {
public:
  static constexpr unsigned long long max() { return ULLONG_MAX; }
  static constexpr unsigned long long min() { return 0; }
};
template <> class numeric_limits<short> {
public:
  static constexpr short max() { return SHRT_MAX; }
  static constexpr short min() { return SHRT_MIN; }
};
template <> class numeric_limits<unsigned short> {
public:
  static constexpr unsigned short max() { return USHRT_MAX; }
  static constexpr unsigned short min() { return 0; }
};
template <> class numeric_limits<char> {
public:
  static constexpr char max() { return CHAR_MAX; }
  static constexpr char min() { return CHAR_MIN; }
};
template <> class numeric_limits<signed char> {
public:
  static constexpr signed char max() { return SCHAR_MAX; }
  static constexpr signed char min() { return SCHAR_MIN; }
};
template <> class numeric_limits<unsigned char> {
public:
  static constexpr unsigned char max() { return UCHAR_MAX; }
  static constexpr unsigned char min() { return 0; }
};
#ifdef __SIZEOF_INT128__
// On platform where UInt128 resolves to __uint128_t, this specialization
// provides the limits of UInt128.
template <> class numeric_limits<__uint128_t> {
public:
  static constexpr __uint128_t max() { return ~__uint128_t(0); }
  static constexpr __uint128_t min() { return 0; }
};
#endif

// floating point types

template <> struct numeric_limits<float> {
  static constexpr float denorm_min() { return FLT_TRUE_MIN; }
  static constexpr float min() { return FLT_MIN; }
  static constexpr float lowest() { return -FLT_MAX; }
  static constexpr float max() { return FLT_MAX; }
  static constexpr float epsilon() { return FLT_EPSILON; }
  // static constexpr float infinity() { return HUGE_VALF; }
  LIBC_INLINE_VAR static constexpr int digits = FLT_MANT_DIG;
  LIBC_INLINE_VAR static constexpr int digits10 = FLT_DIG;
  LIBC_INLINE_VAR static constexpr int min_exponent = FLT_MIN_EXP;
  LIBC_INLINE_VAR static constexpr int min_exponent10 = FLT_MIN_10_EXP;
  LIBC_INLINE_VAR static constexpr int max_exponent = FLT_MAX_EXP;
  LIBC_INLINE_VAR static constexpr int max_exponent10 = FLT_MAX_10_EXP;
  LIBC_INLINE_VAR static constexpr int radix = FLT_RADIX;
};

template <> struct numeric_limits<double> {
  static constexpr double denorm_min() { return DBL_TRUE_MIN; }
  static constexpr double min() { return DBL_MIN; }
  static constexpr double lowest() { return -DBL_MAX; }
  static constexpr double max() { return DBL_MAX; }
  static constexpr double epsilon() { return DBL_EPSILON; }
  // static constexpr double infinity() { return HUGE_VAL; }
  LIBC_INLINE_VAR static constexpr int digits = DBL_MANT_DIG;
  LIBC_INLINE_VAR static constexpr int digits10 = DBL_DIG;
  LIBC_INLINE_VAR static constexpr int min_exponent = DBL_MIN_EXP;
  LIBC_INLINE_VAR static constexpr int min_exponent10 = DBL_MIN_10_EXP;
  LIBC_INLINE_VAR static constexpr int max_exponent = DBL_MAX_EXP;
  LIBC_INLINE_VAR static constexpr int max_exponent10 = DBL_MAX_10_EXP;
  LIBC_INLINE_VAR static constexpr int radix = FLT_RADIX;
};

template <> struct numeric_limits<long double> {
  static constexpr long double denorm_min() { return LDBL_TRUE_MIN; }
  static constexpr long double min() { return LDBL_MIN; }
  static constexpr long double lowest() { return -LDBL_MAX; }
  static constexpr long double max() { return LDBL_MAX; }
  static constexpr long double epsilon() { return LDBL_EPSILON; }
  // static constexpr long double infinity() { return HUGE_VALL; }
  LIBC_INLINE_VAR static constexpr int digits = LDBL_MANT_DIG;
  LIBC_INLINE_VAR static constexpr int digits10 = LDBL_DIG;
  LIBC_INLINE_VAR static constexpr int min_exponent = LDBL_MIN_EXP;
  LIBC_INLINE_VAR static constexpr int min_exponent10 = LDBL_MIN_10_EXP;
  LIBC_INLINE_VAR static constexpr int max_exponent = LDBL_MAX_EXP;
  LIBC_INLINE_VAR static constexpr int max_exponent10 = LDBL_MAX_10_EXP;
  LIBC_INLINE_VAR static constexpr int radix = FLT_RADIX;
};

} // namespace cpp
} // namespace LIBC_NAMESPACE

#endif // LLVM_LIBC_SRC___SUPPORT_CPP_LIMITS_H
