#include "vacationdb.hpp"
#include <omp.h>

int main(int argc, char* argv[]) {
	(void) argc;
	(void) argv;

	Vacationdb::Database db;
#pragma omp parallel
	{

		auto a = db.add_employee("Blah", 1800 + omp_get_thread_num(), 10, 2, "2221412");
		auto b = db.add_employee("Argz", 1800, 12, 2, "222141/212");
	}

	// db.edit_employee_name(a, "Ughh");
}