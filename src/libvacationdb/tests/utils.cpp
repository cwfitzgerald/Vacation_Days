#include "database_impl.hpp"
#include "gtest/gtest.h"

std::string gen_number(const char* val) {
	auto raw_num = Vacationdb::_detail::create_number_safe(val);
	return raw_num.convert_to<std::string>();
}

TEST(UTILS_NUMBER, Integers) {
	ASSERT_STREQ(gen_number("-1").c_str(), "-1");
	ASSERT_STREQ(gen_number("-0").c_str(), "0");
	ASSERT_STREQ(gen_number("0").c_str(), "0");
	ASSERT_STREQ(gen_number("1").c_str(), "1");
}

TEST(UTILS_NUMBER, Fractions) {
	// Positive
	ASSERT_STREQ(gen_number("-3/2").c_str(), "-3/2");
	ASSERT_STREQ(gen_number("-2/2").c_str(), "-1");
	ASSERT_STREQ(gen_number("-1/2").c_str(), "-1/2");
	ASSERT_STREQ(gen_number("-0/2").c_str(), "0");
	ASSERT_STREQ(gen_number("0/2").c_str(), "0");
	ASSERT_STREQ(gen_number("1/2").c_str(), "1/2");
	ASSERT_STREQ(gen_number("2/2").c_str(), "1");
	ASSERT_STREQ(gen_number("3/2").c_str(), "3/2");

	// Negative
	ASSERT_STREQ(gen_number("-3/-2").c_str(), "3/2");
	ASSERT_STREQ(gen_number("-2/-2").c_str(), "1");
	ASSERT_STREQ(gen_number("-1/-2").c_str(), "1/2");
	ASSERT_STREQ(gen_number("-0/-2").c_str(), "0");
	ASSERT_STREQ(gen_number("0/-2").c_str(), "0");
	ASSERT_STREQ(gen_number("1/-2").c_str(), "-1/2");
	ASSERT_STREQ(gen_number("2/-2").c_str(), "-1");
	ASSERT_STREQ(gen_number("3/-2").c_str(), "-3/2");
}

TEST(UTILS_NUMBER, Decimals) {
	// Positive
	ASSERT_STREQ(gen_number("-3.1").c_str(), "-31/10");
	ASSERT_STREQ(gen_number("-2.6").c_str(), "-13/5");
	ASSERT_STREQ(gen_number("-1.3").c_str(), "-13/10");
	ASSERT_STREQ(gen_number("-0.1").c_str(), "-1/10");

	// Negative
	ASSERT_STREQ(gen_number("3.1").c_str(), "31/10");
	ASSERT_STREQ(gen_number("2.6").c_str(), "13/5");
	ASSERT_STREQ(gen_number("1.3").c_str(), "13/10");
	ASSERT_STREQ(gen_number("0.1").c_str(), "1/10");
}

TEST(UTILS_NUMBER, Fractional_Decimals) {
	// Positive
	ASSERT_STREQ(gen_number("-3.1/12.532").c_str(), "-775/3133");
	ASSERT_STREQ(gen_number("-2.6/12.532").c_str(), "-50/241");
	ASSERT_STREQ(gen_number("-1.3/12.532").c_str(), "-25/241");
	ASSERT_STREQ(gen_number("-0.1/12.532").c_str(), "-25/3133");
	ASSERT_STREQ(gen_number("-0/12.532").c_str(), "0");
	ASSERT_STREQ(gen_number("0/12.532").c_str(), "0");
	ASSERT_STREQ(gen_number("0.1/12.532").c_str(), "25/3133");
	ASSERT_STREQ(gen_number("1.3/12.532").c_str(), "25/241");
	ASSERT_STREQ(gen_number("2.6/12.532").c_str(), "50/241");
	ASSERT_STREQ(gen_number("3.1/12.532").c_str(), "775/3133");

	// Negative
	ASSERT_STREQ(gen_number("-3.1/-12.532").c_str(), "775/3133");
	ASSERT_STREQ(gen_number("-2.6/-12.532").c_str(), "50/241");
	ASSERT_STREQ(gen_number("-1.3/-12.532").c_str(), "25/241");
	ASSERT_STREQ(gen_number("-0.1/-12.532").c_str(), "25/3133");
	ASSERT_STREQ(gen_number("-0/-12.532").c_str(), "0");
	ASSERT_STREQ(gen_number("0/-12.532").c_str(), "0");
	ASSERT_STREQ(gen_number("0.1/-12.532").c_str(), "-25/3133");
	ASSERT_STREQ(gen_number("1.3/-12.532").c_str(), "-25/241");
	ASSERT_STREQ(gen_number("2.6/-12.532").c_str(), "-50/241");
	ASSERT_STREQ(gen_number("3.1/-12.532").c_str(), "-775/3133");
}
