#include "../vacationdb.hpp"
#include "vacationdb_impl.hpp"

void vacationdb::_detail::db_impl_deleter::operator()(db_impl* value) {
	delete value;
}
