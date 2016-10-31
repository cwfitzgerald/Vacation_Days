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
	auto e = db.add_employee(name, 2015, 2, 23, "2");

	auto info = db.get_employee_info(e);
	ASSERT_EQ(info.id, e);
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
	info = db.get_employee_info(e);
	ASSERT_EQ(info.extra_work_time.size(), size_t{0});
}

TEST(DB_EMPLOYEE_CATALOG, EditAddExtraWorkTimeAccuracy) {
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

	auto orig = db.add_employee(name, 1400, 1, 1, "1");
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
	auto orig = db.add_employee(name4, 1400, 1, 1, "1");
	auto found = db.find_employee(name4);

	ASSERT_EQ(orig, found);
}

TEST(DB_EMPLOYEE_CATALOG, FindMultiNamewithDelete) {
	Vacationdb::Database db;
	const char* name1 = "George Costanz";
	const char* name2 = "George Costanza";
	const char* name3 = "George Costanzas";

	db.add_employee(name1, 1400, 1, 1, "1");
	auto d = db.add_employee(name2, 1400, 1, 1, "1");
	db.add_employee(name3, 1400, 1, 1, "1");
	auto orig = db.add_employee(name2, 1400, 1, 1, "1");
	db.delete_employee(d);

	auto found = db.find_employee(name2);

	ASSERT_EQ(orig, found);
}

TEST(DB_EMPLOYEE_CATALOG, FindDeletedName) {
	Vacationdb::Database db;
	const char* name1 = "George Costanz";
	const char* name2 = "George Costanzb";
	const char* name3 = "George Costanzas";
	const char* name4 = "George Costanza";

	db.add_employee(name1, 1400, 1, 1, "1");
	db.add_employee(name2, 1400, 1, 1, "1");
	db.add_employee(name3, 1400, 1, 1, "1");
	auto orig = db.add_employee(name4, 1400, 1, 1, "1");
	db.delete_employee(orig);

	bool threw = false;

	try {
		db.find_employee(name4);
	}
	catch (Vacationdb::Employee_Not_Found& e) {
		threw = true;
	}

	ASSERT_EQ(threw, true);
}

TEST(DB_EMPLOYEE_CATALOG, ListNames) {
	Vacationdb::Database db;

	db.add_employee("Name1", 1400, 1, 1, "1");
	db.add_employee("Name2", 1401, 2, 2, "2");

	auto name_list = db.list_employee_names();
	ASSERT_EQ(name_list.size(), size_t{2});
	ASSERT_STREQ(name_list[0].c_str(), "Name1");
	ASSERT_STREQ(name_list[1].c_str(), "Name2");
}

TEST(DB_EMPLOYEE_CATALOG, ListNameswithDelete) {
	Vacationdb::Database db;

	db.add_employee("Name1", 1400, 1, 1, "1");
	auto e = db.add_employee("Name2", 1401, 2, 2, "2");
	db.add_employee("Name3", 1402, 3, 3, "3");

	db.delete_employee(e);

	auto name_list = db.list_employee_names();
	ASSERT_EQ(name_list.size(), size_t{2});
	ASSERT_STREQ(name_list[0].c_str(), "Name1");
	ASSERT_STREQ(name_list[1].c_str(), "Name3");
}

TEST(DB_EMPLOYEE_CATALOG, ListInfo) {
	Vacationdb::Database db;

	db.add_employee("Name1", 1400, 1, 1, "1");
	db.add_employee("Name2", 1401, 2, 2, "2");

	auto info_list = db.list_employee_info();
	ASSERT_EQ(info_list.size(), size_t{2});

	ASSERT_STREQ(info_list[0].name.c_str(), "Name1");
	ASSERT_EQ(info_list[0].start_year, uint16_t{1400});
	ASSERT_EQ(info_list[0].start_month, uint16_t{1});
	ASSERT_EQ(info_list[0].start_day, uint16_t{1});
	ASSERT_STREQ(info_list[0].work_time.c_str(), "1");
	ASSERT_EQ(info_list[0].extra_work_time.size(), size_t{0});

	ASSERT_STREQ(info_list[1].name.c_str(), "Name2");
	ASSERT_EQ(info_list[1].start_year, uint16_t{1401});
	ASSERT_EQ(info_list[1].start_month, uint16_t{2});
	ASSERT_EQ(info_list[1].start_day, uint16_t{2});
	ASSERT_STREQ(info_list[1].work_time.c_str(), "2");
	ASSERT_EQ(info_list[1].extra_work_time.size(), size_t{0});
}

TEST(DB_EMPLOYEE_CATALOG, ListInfowithDelete) {
	Vacationdb::Database db;

	db.add_employee("Name1", 1400, 1, 1, "1");
	auto e = db.add_employee("Name2", 1401, 2, 2, "2");
	db.add_employee("Name3", 1402, 3, 3, "3");

	db.delete_employee(e);

	auto info_list = db.list_employee_info();
	ASSERT_EQ(info_list.size(), size_t{2});

	ASSERT_STREQ(info_list[0].name.c_str(), "Name1");
	ASSERT_EQ(info_list[0].start_year, uint16_t{1400});
	ASSERT_EQ(info_list[0].start_month, uint16_t{1});
	ASSERT_EQ(info_list[0].start_day, uint16_t{1});
	ASSERT_STREQ(info_list[0].work_time.c_str(), "1");

	ASSERT_STREQ(info_list[1].name.c_str(), "Name3");
	ASSERT_EQ(info_list[1].start_year, uint16_t{1402});
	ASSERT_EQ(info_list[1].start_month, uint16_t{3});
	ASSERT_EQ(info_list[1].start_day, uint16_t{3});
	ASSERT_STREQ(info_list[1].work_time.c_str(), "3");
}

TEST(DB_EMPLOYEE_CATALOG, ThrowOnUseAfterDelete) {
	Vacationdb::Database db;

	auto e = db.add_employee("name", 1400, 1, 1, "1");
	db.delete_employee(e);

	bool threw = false;
	try {
		db.get_employee_info(e);
	}
	catch (Vacationdb::Invalid_Index& err) {
		threw = true;
	}

	ASSERT_EQ(threw, true);
}

TEST(DB_EMPLOYEE_CATALOG, ThrowOnInvalidNumber) {
	Vacationdb::Database db;

	bool threw = false;
	try {
		auto e = db.get_employee_info(Vacationdb::PersonID_t{1});
	}
	catch (Vacationdb::Invalid_Index& err) {
		threw = true;
	}

	ASSERT_EQ(threw, true);
}