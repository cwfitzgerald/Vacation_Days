#include "../vacationdb.hpp"
#include "gtest/gtest.h"

TEST(DB_DAYTYPE_CATALOG, AddDelete) {
	Vacationdb::Database db;

	auto d     = db.add_day("Sick", "0", "5");
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

	auto d    = db.add_day("Sick", "0", "5");
	auto r    = db.edit_day_add_rule(d, 5, "10");
	auto info = db.get_day_info(d);

	ASSERT_EQ(info.id, r);
	ASSERT_EQ(info.rules[0].month_start, uint32_t{5});
	ASSERT_STREQ(info.rules[0].days_per_year.c_str(), "10");
}
