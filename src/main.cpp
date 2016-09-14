#include "vacationdb.hpp"

int main(int argc, char* argv[]) {
	(void) argc;
	(void) argv;

	Vacationdb::Database db;

	auto a = db.add_employee("Blah", 1800, 10, 2, "2221412");
	auto b = db.add_employee("Argz", 1800, 12, 2, "222141/212");

	(void) a;
	(void) b;
}