#include "vacationdb.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
	(void) argc;
	(void) argv;

	// Vacationdb::Database db;

	// auto a = db.add_employee("Blah", 1800, 10, 2, "2221412");
	// auto b = db.add_employee("Argz", 1800, 12, 2, "222141/212");

	// (void) a;
	// (void) b;

	Vacationdb::Database db;

	std::string result;

	try {
		auto eid = db.add_employee("Bob", 2016, 10, 31, "1");
		auto did = db.add_day("Vacation", "0", "5");

		result = db.query_vacation_days(eid, did, 2016, 10, 31);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << '\n';
	}
}