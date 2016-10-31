#include "../vacationdb.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>

TEST(CALC_ACCURACY, Starting_Date) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("Bob", 2016, 10, 31, "1");
	auto did = db.add_day("Vacation", "0", "10.25");

	result = db.query_person_days(eid, did, 2016, 10, 31);

	ASSERT_STREQ(result.c_str(), "41/4");
}

TEST(CALC_ACCURACY, Yearly_Bonus) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("Bob", 2016, 10, 31, "1");
	auto did = db.add_day("Vacation", "0", "10.25");

	result = db.query_person_days(eid, did, 2017, 1, 1);

	ASSERT_STREQ(result.c_str(), "41/4");
}
