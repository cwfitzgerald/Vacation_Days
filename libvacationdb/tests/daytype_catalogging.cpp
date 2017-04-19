#include "vacationdb.hpp"
#include "gtest/gtest.h"

TEST(DB_DAYTYPE_CATALOG, AddDelete) {
	Vacationdb::Database db;

	auto d = db.add_day("Sick", "0", "5");
	auto count = db.get_day_count();
	ASSERT_EQ(count, size_t{1});

	db.delete_day(d);
	count = db.get_day_count();
	ASSERT_EQ(count, size_t{0});
}

TEST(DB_DAYTYPE_CATALOG, AddAccuracy) {
	Vacationdb::Database db;

	auto d = db.add_day("Sick", "0", "5");

	auto info = db.get_day_info(d);
	ASSERT_EQ(d, info.id);
	ASSERT_STREQ(info.name.c_str(), "Sick");
	ASSERT_STREQ(info.rollover.c_str(), "0");
	ASSERT_STREQ(info.yearly_bonus.c_str(), "5");
	ASSERT_EQ(info.rules.size(), size_t{0});
}

TEST(DB_DAYTYPE_CATALOG, EditName) {
	Vacationdb::Database db;

	auto d = db.add_day("SickMannn", "0", "5");

	db.edit_day_name(d, "Sick");

	auto info = db.get_day_info(d);
	ASSERT_STREQ(info.name.c_str(), "Sick");
}

TEST(DB_DAYTYPE_CATALOG, EditRollover) {
	Vacationdb::Database db;

	auto d = db.add_day("Sick", "0", "5");

	db.edit_day_rollover(d, "1");

	auto info = db.get_day_info(d);
	ASSERT_STREQ(info.rollover.c_str(), "1");
}

TEST(DB_DAYTYPE_CATALOG, EditYearlyBonus) {
	Vacationdb::Database db;

	auto d = db.add_day("Sick", "0", "5");

	db.edit_day_yearly_bonus(d, "10");

	auto info = db.get_day_info(d);
	ASSERT_STREQ(info.yearly_bonus.c_str(), "10");
}

TEST(DB_DAYTYPE_CATALOG, EditAddRemoveRule) {
	Vacationdb::Database db;

	auto d = db.add_day("Sick", "0", "5");

	auto info = db.get_day_info(d);
	ASSERT_EQ(info.rules.size(), size_t{0});

	auto r = db.edit_day_add_rule(d, 5, "10");

	info = db.get_day_info(d);
	ASSERT_EQ(info.rules.size(), size_t{1});

	db.edit_day_remove_rule(d, r);

	info = db.get_day_info(d);
	ASSERT_EQ(info.rules.size(), size_t{0});
}

TEST(DB_DAYTYPE_CATALOG, EditAddRuleAccuracy) {
	Vacationdb::Database db;

	auto d = db.add_day("Sick", "0", "5");
	auto r = db.edit_day_add_rule(d, 5, "10");
	auto info = db.get_day_info(d);

	ASSERT_EQ(info.rules[0].id, r);
	ASSERT_EQ(info.rules[0].month_start, uint32_t{5});
	ASSERT_STREQ(info.rules[0].days_per_year.c_str(), "10");
}

TEST(DB_DAYTYPE_CATALOG, Find) {
	Vacationdb::Database db;

	auto d = db.add_day("Sick", "0", "5");
	auto f = db.find_day("Sick");

	ASSERT_EQ(d, f);
}

TEST(DB_DAYTYPE_CATALOG, FindMultiName) {
	Vacationdb::Database db;

	const char* name1 = "Sick";
	const char* name2 = "Sic";
	const char* name3 = "Sicke";
	const char* name4 = "Sickk";

	db.add_day(name1, "0", "5");
	db.add_day(name2, "0", "5");
	db.add_day(name3, "0", "5");
	auto d = db.add_day(name4, "0", "5");
	auto f = db.find_day(name4);

	ASSERT_EQ(d, f);
}

TEST(DB_DAYTYPE_CATALOG, FindMultiNameWithDelete) {
	Vacationdb::Database db;

	const char* name1 = "Sic";
	const char* name2 = "Sick";
	const char* name3 = "Sicke";

	db.add_day(name1, "0", "5");
	db.delete_day(db.add_day(name2, "0", "5"));
	db.add_day(name3, "0", "5");
	auto d = db.add_day(name2, "0", "5");
	auto f = db.find_day(name2);

	ASSERT_EQ(d, f);
}

TEST(DB_DAYTYPE_CATALOG, FindDeletedName) {
	Vacationdb::Database db;

	const char* name1 = "Sick";
	const char* name2 = "Sic";
	const char* name3 = "Sicke";
	const char* name4 = "Sickk";

	db.add_day(name1, "0", "5");
	db.add_day(name2, "0", "5");
	db.add_day(name3, "0", "5");
	db.delete_day(db.add_day(name4, "0", "5"));

	bool threw = false;

	try {
		db.find_day(name4);
	}
    catch (Vacationdb::Day_Not_Found&) {
		threw = true;
	}

	ASSERT_EQ(threw, true);
}

TEST(DB_DAYTYPE_CATALOG, ListNames) {
	Vacationdb::Database db;

	db.add_day("Vacation", "0", "5");
	db.add_day("Sick", "0", "5");

	auto names = db.list_day_names();
	ASSERT_EQ(names.size(), size_t{2});
	ASSERT_STREQ(names[0].c_str(), "Vacation");
	ASSERT_STREQ(names[1].c_str(), "Sick");
}

TEST(DB_DAYTYPE_CATALOG, ListNamesWithDelete) {
	Vacationdb::Database db;

	db.add_day("Vacation", "0", "5");
	db.delete_day(db.add_day("Sick", "0", "5"));
	db.add_day("Personal", "0", "5");

	auto names = db.list_day_names();
	ASSERT_EQ(names.size(), size_t{2});
	ASSERT_STREQ(names[0].c_str(), "Vacation");
	ASSERT_STREQ(names[1].c_str(), "Personal");
}

TEST(DB_DAYTYPE_CATALOG, ListInfo) {
	Vacationdb::Database db;

	db.add_day("Vacation", "0", "5");
	db.add_day("Sick", "1", "6");

	auto info = db.list_day_info();

	ASSERT_EQ(info.size(), size_t{2});

	ASSERT_STREQ(info[0].name.c_str(), "Vacation");
	ASSERT_STREQ(info[0].rollover.c_str(), "0");
	ASSERT_STREQ(info[0].yearly_bonus.c_str(), "5");
	ASSERT_EQ(info[0].rules.size(), size_t{0});

	ASSERT_STREQ(info[1].name.c_str(), "Sick");
	ASSERT_STREQ(info[1].rollover.c_str(), "1");
	ASSERT_STREQ(info[1].yearly_bonus.c_str(), "6");
	ASSERT_EQ(info[1].rules.size(), size_t{0});
}

TEST(DB_DAYTYPE_CATALOG, ListInfoWithDelete) {
	Vacationdb::Database db;

	db.add_day("Vacation", "0", "5");
	db.delete_day(db.add_day("Blh", "2", "212"));
	db.add_day("Sick", "1", "6");

	auto info = db.list_day_info();

	ASSERT_EQ(info.size(), size_t{2});

	ASSERT_STREQ(info[0].name.c_str(), "Vacation");
	ASSERT_STREQ(info[0].rollover.c_str(), "0");
	ASSERT_STREQ(info[0].yearly_bonus.c_str(), "5");
	ASSERT_EQ(info[0].rules.size(), size_t{0});

	ASSERT_STREQ(info[1].name.c_str(), "Sick");
	ASSERT_STREQ(info[1].rollover.c_str(), "1");
	ASSERT_STREQ(info[1].yearly_bonus.c_str(), "6");
	ASSERT_EQ(info[1].rules.size(), size_t{0});
}

TEST(DB_DAYTYPE_CATALOG, ThrowOnUseAfterDelete) {
	Vacationdb::Database db;

	auto d = db.add_day("Sick", "0", "5");
	db.delete_day(d);

	bool threw = false;

	try {
		db.edit_day_name(d, "blah");
	}
    catch (Vacationdb::Invalid_Index&) {
		threw = true;
	}

	ASSERT_EQ(threw, true);
}

TEST(DB_DAYTYPE_CATALOG, ThrowOnInvalidIndex) {
	Vacationdb::Database db;

	bool threw = false;

	try {
		db.edit_day_name(Vacationdb::DayID_t{0}, "blah");
	}
    catch (Vacationdb::Invalid_Index&) {
		threw = true;
	}

	ASSERT_EQ(threw, true);
}
