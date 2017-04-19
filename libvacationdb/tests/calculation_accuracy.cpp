#include "database_impl.hpp"
#include "vacationdb.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>

bool within(std::string& value, const char* expected, const char* epsilon);

bool within(std::string& value, const char* expected, const char* epsilon) {
	Vacationdb::_detail::Number val{value};
	Vacationdb::_detail::Number expect{expected};
	Vacationdb::_detail::Number e{epsilon};

	auto diff = boost::multiprecision::abs(val - expect);
	bool result = diff <= e;

	std::cout << "EXPECTED: " << expect << '\n';
	std::cout << "  ACTUAL: " << val << '\n';
	std::cout << "    DIFF: " << diff << '\n';

	return result;
}

TEST(CALC_ACCURACY, Starting_Date) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("Bob", 2016, 10, 31, "1");
	auto did = db.add_day("Vacation", "0", "10.25");

	result = db.query_vacation_days(eid, did, 2016, 10, 31);

	ASSERT_STREQ(result.c_str(), "41/4");
}

TEST(CALC_ACCURACY, Yearly_Bonus) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("Bob", 2016, 10, 31, "1");
	auto did = db.add_day("Vacation", "0", "10.25");

	result = db.query_vacation_days(eid, did, 2017, 1, 1);

	ASSERT_STREQ(result.c_str(), "41/4");
}

TEST(CALC_ACCURACY, Rollover) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("Bob", 2016, 10, 31, "1");
	auto did = db.add_day("Vacation", "2", "4");

	result = db.query_vacation_days(eid, did, 2017, 1, 1);

	ASSERT_STREQ(result.c_str(), "6");
}

TEST(CALC_ACCURACY, NoRollover) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("Bob", 2016, 10, 31, "1");
	auto did = db.add_day("Vacation", "0", "2");

	result = db.query_vacation_days(eid, did, 2017, 1, 1);

	ASSERT_STREQ(result.c_str(), "2");
}

TEST(CALC_ACCURACY, FullRollover) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("Bob", 2016, 10, 31, "1");
	auto did = db.add_day("Vacation", "-1", "2");

	result = db.query_vacation_days(eid, did, 2017, 1, 1);

	ASSERT_STREQ(result.c_str(), "4");
}

TEST(CALC_ACCURACY, SingleDayTaken) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("Bob", 2016, 10, 31, "1");
	auto did = db.add_day("Vacation", "0", "1");

	db.add_day_off(eid, did, 2016, 10, 31, "1");

	result = db.query_vacation_days(eid, did, 2016, 10, 31);

	ASSERT_STREQ(result.c_str(), "0");
}

TEST(CALC_ACCURACY, SingleDayTakenWithNoRollover) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("Bob", 2016, 10, 31, "1");
	auto did = db.add_day("Vacation", "0", "1");

	db.add_day_off(eid, did, 2016, 10, 31, "1");

	result = db.query_vacation_days(eid, did, 2017, 1, 1);

	ASSERT_STREQ(result.c_str(), "1");
}

TEST(CALC_ACCURACY, SingleDayRule) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("Bob", 2017, 1, 1, "1");
	auto did = db.add_day("Vacation", "-1", "0");
	db.edit_day_add_rule(did, 1, "15");

	result = db.query_vacation_days(eid, did, 2018, 1, 1);

	ASSERT_EQ(within(result, "15", "1/2"), true);
}

TEST(CALC_ACCURACY, MultipleDayRules) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("Bob", 2017, 1, 1, "1");
	auto did = db.add_day("Vacation", "-1", "0");
	db.edit_day_add_rule(did, 1, "15");
	db.edit_day_add_rule(did, 7, "30");
	db.edit_day_add_rule(did, 13, "45");
	db.edit_day_add_rule(did, 19, "60");

	result = db.query_vacation_days(eid, did, 2019, 1, 1);

	ASSERT_EQ(within(result, "75", "1/2"), true);
}

TEST(CALC_ACCURACY, SingleExtraWorkTime) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("Bob", 2017, 1, 1, "1");
	auto did = db.add_day("Vacation", "-1", "0");
	db.edit_day_add_rule(did, 1, "15");
	db.edit_day_add_rule(did, 7, "30");

	db.edit_employee_add_extra_work_time(eid, 2017, 7, 1, 2018, 1, 1, "0.5");

	result = db.query_vacation_days(eid, did, 2018, 1, 1);

	ASSERT_EQ(within(result, "15", "1/2"), true);
}

TEST(CALC_ACCURACY, MultipleExtraWorkTime) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("Bob", 2017, 1, 1, "1");
	auto did = db.add_day("Vacation", "-1", "0");
	db.edit_day_add_rule(did, 1, "15");
	db.edit_day_add_rule(did, 7, "30");

	db.edit_employee_add_extra_work_time(eid, 2017, 7, 1, 2018, 1, 1, "0.5");
	db.edit_employee_add_extra_work_time(eid, 2018, 1, 1, 2019, 1, 1, "0.25");

	result = db.query_vacation_days(eid, did, 2019, 1, 1);

	ASSERT_EQ(within(result, "45/2", "1/2"), true);
}

TEST(CALC_ACCURACY, ThousandYears) {
	Vacationdb::Database db;

	std::string result;

	auto eid = db.add_employee("TestCase1", 2000, 1, 1, "1");
	auto did = db.add_day("Vacation", "-1", "0");

	db.edit_day_add_rule(did, 1, "24");

	result = db.query_vacation_days(eid, did, 3000, 1, 1);
	ASSERT_EQ(within(result, "24000", "1/2"), true);

	result = db.query_vacation_days(eid, did, 3000, 7, 3);
	ASSERT_EQ(within(result, "24012", "1/2"), true);
}

TEST(CALC_ACCURACY, TestCase1) {
	Vacationdb::Database db;

	auto eid = db.add_employee("TestCase1", 2015, 1, 1, "1");

	///////////////////
	// Vacation Days //
	///////////////////

	auto vacayid = db.add_day("Vacation", "-1", "0");
	db.edit_day_add_rule(vacayid, 1, "24");

	db.add_day_off(eid, vacayid, 2015, 1, 1, "-28.5");
	db.add_day_off(eid, vacayid, 2015, 1, 28, "1");
	db.add_day_off(eid, vacayid, 2015, 1, 29, "1");
	db.add_day_off(eid, vacayid, 2015, 1, 30, "1");
	db.add_day_off(eid, vacayid, 2015, 3, 20, "1");
	db.add_day_off(eid, vacayid, 2015, 3, 27, "1");
	db.add_day_off(eid, vacayid, 2015, 4, 3, "1");
	db.add_day_off(eid, vacayid, 2015, 4, 10, "1");
	db.add_day_off(eid, vacayid, 2015, 6, 29, "1");
	db.add_day_off(eid, vacayid, 2015, 6, 30, "1");
	db.add_day_off(eid, vacayid, 2015, 7, 1, "1");
	db.add_day_off(eid, vacayid, 2015, 7, 2, "1");
	db.add_day_off(eid, vacayid, 2015, 8, 26, "0.5");
	db.add_day_off(eid, vacayid, 2015, 8, 27, "1");
	db.add_day_off(eid, vacayid, 2015, 8, 28, "1");
	db.add_day_off(eid, vacayid, 2015, 8, 31, "1");
	db.add_day_off(eid, vacayid, 2015, 9, 1, "1");
	db.add_day_off(eid, vacayid, 2015, 9, 2, "1");
	db.add_day_off(eid, vacayid, 2015, 9, 3, "1");
	db.add_day_off(eid, vacayid, 2015, 9, 4, "1");

	///////////////////
	// Personal Days //
	///////////////////

	auto persid = db.add_day("Personal", "0", "1");
	db.edit_day_add_rule(persid, 1, "4");

	db.add_day_off(eid, persid, 2015, 1, 22, "1");
	db.add_day_off(eid, persid, 2015, 4, 6, "1");
	db.add_day_off(eid, persid, 2015, 4, 22, "1");

	///////////////
	// Sick Days //
	///////////////

	auto sickid = db.add_day("Sick", "0", "5");
	db.edit_day_add_rule(sickid, 1, "9.96");

	db.add_day_off(eid, sickid, 2015, 4, 21, "0.5");

	/////////////////////////
	// Evaluate Day Counts //
	/////////////////////////

	std::string vacay_result;
	std::string pers_result;
	std::string sick_result;

	vacay_result = db.query_vacation_days(eid, vacayid, 2015, 5, 31);
	pers_result = db.query_vacation_days(eid, persid, 2015, 5, 31);
	sick_result = db.query_vacation_days(eid, sickid, 2015, 5, 31);

	ASSERT_EQ(within(vacay_result, "63/2", "1/4"), true);
	ASSERT_EQ(within(pers_result, "0", "1/2"), true);
	ASSERT_EQ(within(sick_result, "865/100", "1/4"), true);

	vacay_result = db.query_vacation_days(eid, vacayid, 2015, 12, 31);
	pers_result = db.query_vacation_days(eid, persid, 2015, 12, 31);
	sick_result = db.query_vacation_days(eid, sickid, 2015, 12, 31);

	ASSERT_EQ(within(vacay_result, "34", "1/4"), true);
	ASSERT_EQ(within(pers_result, "2", "1/2"), true);
	ASSERT_EQ(within(sick_result, "1446/100", "1/4"), true);
}
