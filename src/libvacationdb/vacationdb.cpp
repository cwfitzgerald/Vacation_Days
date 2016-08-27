#include "../vacationdb.hpp"
#include "database_impl.hpp"

void Vacationdb::_detail::db_impl_deleter::operator()(db_impl* value) {
	delete value;
}
