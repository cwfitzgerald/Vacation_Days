#include "../vacationdb.hpp"
#include "gtest/gtest.h"

TEST(DB_EMPLOYEE_CATALOG, AddDelete) {
	Vacationdb::Database db;
	auto a = db.add_employee("Blah", 2015, 2, 23, "1");
	ASSERT_EQ(db.get_employee_count(), size_t{1});

	db.delete_employee(a);
	ASSERT_EQ(db.get_employee_count(), size_t{0});
}

TEST(DB_EMPLOYEE_CATALOG, AddAccuracy) {
	Vacationdb::Database db;

	const char* name = "Employee Name";
	auto e           = db.add_employee(name, 2015, 2, 23, "2");

	auto info = db.get_employee_info(e);
	ASSERT_STREQ(info.name.c_str(), name);
	ASSERT_EQ(info.start_year, uint16_t{2015});
	ASSERT_EQ(info.start_month, uint16_t{2});
	ASSERT_EQ(info.start_day, uint16_t{23});
	ASSERT_STREQ(info.work_time.c_str(), "2");
	ASSERT_EQ(info.extra_work_time.size(), size_t{0});
}

TEST(DB_EMPLOYEE_CATALOG, EditName) {
	Vacationdb::Database db;

	auto e = db.add_employee("", 1400, 1, 1, "1");
	db.edit_employee_name(e, "blah");

	auto name = db.get_employee_name(e);
	ASSERT_STREQ(name.c_str(), "blah");
}

TEST(DB_EMPLOYEE_CATALOG, EditStartDate) {
	Vacationdb::Database db;

	auto e = db.add_employee("", 1400, 1, 1, "1");
	db.edit_employee_start_date(e, 1401, 2, 2);

	auto info = db.get_employee_info(e);
	ASSERT_EQ(info.start_year, uint16_t{1401});
	ASSERT_EQ(info.start_month, uint16_t{2});
	ASSERT_EQ(info.start_day, uint16_t{2});
}

TEST(DB_EMPLOYEE_CATALOG, EditWorkTime) {
	Vacationdb::Database db;

	auto e = db.add_employee("", 1400, 1, 1, "1/2");
	db.edit_employee_work_time(e, "2/1");

	auto info = db.get_employee_info(e);

	ASSERT_STREQ(info.work_time.c_str(), "2");
}

TEST(DB_EMPLOYEE_CATALOG, EditAddRemoveExtraWorkTime) {
	Vacationdb::Database db;

	auto e = db.add_employee("", 1400, 1, 1, "1");
	auto p = db.edit_employee_add_extra_work_time(e, 2000, 1, 1, 2001, 2, 2, "2");

	auto info = db.get_employee_info(e);
	ASSERT_EQ(info.extra_work_time.size(), size_t{1});

	db.edit_employee_remove_extra_work_time(e, p);
}

TEST(DB_EMPLOYEE_CATALOG, EditAddExtraWorkTime) {
	Vacationdb::Database db;

	auto e = db.add_employee("", 1400, 1, 1, "1");
	db.edit_employee_add_extra_work_time(e, 2000, 1, 1, 2001, 2, 2, "2");

	auto info = db.get_employee_info(e);
	ASSERT_EQ(info.extra_work_time[0].start_year, uint16_t{2000});
	ASSERT_EQ(info.extra_work_time[0].start_month, uint16_t{1});
	ASSERT_EQ(info.extra_work_time[0].start_day, uint16_t{1});
	ASSERT_EQ(info.extra_work_time[0].end_year, uint16_t{2001});
	ASSERT_EQ(info.extra_work_time[0].end_month, uint16_t{2});
	ASSERT_EQ(info.extra_work_time[0].end_day, uint16_t{2});
	ASSERT_STREQ(info.extra_work_time[0].time.c_str(), "2");
}

TEST(DB_EMPLOYEE_CATALOG, Find) {
	Vacationdb::Database db;
	const char* name = "George Costanza";

	auto orig  = db.add_employee(name, 1400, 1, 1, "1");
	auto found = db.find_employee(name);

	ASSERT_EQ(orig, found);
}

TEST(DB_EMPLOYEE_CATALOG, FindMultiName) {
	Vacationdb::Database db;
	const char* name1 = "George Costanz";
	const char* name2 = "George Costanzb";
	const char* name3 = "George Costanzas";
	const char* name4 = "George Costanza";

	db.add_employee(name1, 1400, 1, 1, "1");
	db.add_employee(name2, 1400, 1, 1, "1");
	db.add_employee(name3, 1400, 1, 1, "1");
	auto orig  = db.add_employee(name4, 1400, 1, 1, "1");
	auto found = db.find_employee(name4);

	ASSERT_EQ(orig, found);
}
