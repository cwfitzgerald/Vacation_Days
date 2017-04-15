#include "database_impl.hpp"

#include <algorithm>
#include <iterator>

namespace Vacationdb::_detail {
	VACATIONDB_SHARED
	boost::gregorian::date create_date_safe(uint16_t start_year, uint16_t start_month,
	                                        uint16_t start_day) {
		boost::gregorian::date ret;
		try {
			ret = boost::gregorian::date{start_year, start_month, start_day};
		}
		catch (boost::exception_detail::clone_impl<
		       boost::exception_detail::error_info_injector<boost::gregorian::bad_year>>& e) {
			throw Vacationdb::Invalid_Date();
		}
		return ret;
	}

	boost::multiprecision::mpq_rational create_number_safe(const char* value) {
		std::string source(value);
		// Parse the number to transform decimals into forms that mpq_rational will
		// take.
		// The numbers need to have integer numerators and denominators.

		std::string numerator;
		std::string denominator;

		auto division_location = std::find(source.begin(), source.end(), '/');

		// If it has a denominator
		bool has_denom = division_location != source.end();

		// Figures out how many decimal places each has in order to convert.
		size_t num_decimal_places = 0;
		size_t den_decimal_places = 0;

		// The search will stop at the first . or /
		auto num_decimal_loc = std::find_if(source.begin(), source.end(),
		                                    [](char c) { return (c == '.') || (c == '/'); });

		// If it did not stop on the /
		if (num_decimal_loc != division_location) {
			// This works even if division_location == source.end()
			size_t dist = std::abs(std::distance(num_decimal_loc, division_location));
			num_decimal_places = dist - 1;
		}

		// If there is a denominator find the period
		if (has_denom) {
			auto den_decimal_loc = std::find(division_location, source.end(), '.');
			if (den_decimal_loc != source.end()) {
				size_t dist = std::abs(std::distance(den_decimal_loc, source.end()));
				den_decimal_places = dist - 1;
			}
		}

		size_t total_offset = std::max(num_decimal_places, den_decimal_places);

		// Back inserters
		std::back_insert_iterator<std::string> num_back_insert(numerator);
		std::back_insert_iterator<std::string> den_back_insert(denominator);

		// Copy numerator
		std::copy_if(source.begin(), division_location, num_back_insert,
		             [](char c) { return !(c == '.'); });

		// Fill in difference in zeros
		std::fill_n(num_back_insert, total_offset - num_decimal_places, '0');
		if (has_denom) {
			// Copy denom
			std::copy_if(division_location + 1, source.end(), den_back_insert,
			             [](char c) { return !(c == '.'); });

			std::fill_n(num_back_insert, total_offset - den_decimal_places, '0');
		}
		else {
			// Fill in proper denominator
			denominator = "1";
			std::fill_n(den_back_insert, total_offset, '0');
		}

		boost::multiprecision::mpq_rational ret;
		try {
			ret = boost::multiprecision::mpq_rational{numerator, denominator};
		}
		catch (boost::exception_detail::clone_impl<
		       boost::exception_detail::error_info_injector<std::runtime_error>>& exep) {
			throw Vacationdb::Invalid_Number();
		}
		return ret;
	}

	void db_impl::block_if_locked() {
		if (io_lock.load()) {
			io_future.wait();
		}
		io_lock.store(false);
	}

	void db_impl::validate(PersonID_t p) {
		bool valid_index = p < people.size();
		if (valid_index) {
			bool valid = people[p].valid;
			if (valid) {
				return;
			}
		}
		throw Vacationdb::Invalid_Index();
	}

	void db_impl::validate(PersonID_t p, Extra_TimeID_t e) {
		bool valid_index_p = p < people.size();
		if (valid_index_p) {
			bool valid_p = people[p].valid;
			bool valid_index_e = e < people[p].extra_time.size();
			if (valid_p && valid_index_e) {
				bool valid_e = people[p].extra_time[e].valid;
				if (valid_e) {
					return;
				}
			}
		}
		throw Vacationdb::Invalid_Index();
	}

	void db_impl::validate(DayID_t d) {
		bool valid_index = d < day_types.size();
		if (valid_index) {
			bool valid = day_types[d].valid;
			if (valid) {
				return;
			}
		}
		throw Vacationdb::Invalid_Index();
	}

	void db_impl::validate(DayID_t d, RuleID_t r) {
		bool valid_index_d = d < day_types.size();
		if (valid_index_d) {
			bool valid_d = day_types[d].valid;
			bool valid_index_r = r < day_types[d].rules.size();
			if (valid_d && valid_index_r) {
				bool valid_r = day_types[d].rules[r].valid;
				if (valid_r) {
					return;
				}
			}
		}
		throw Vacationdb::Invalid_Index();
	}

	void db_impl::add_day_to_people() {
		for (auto& p : people) {
			p.days_taken.emplace_back();
		}
	}

	void db_impl::remove_day_from_people(size_t index) {
		for (auto& p : people) {
			p.days_taken[index].erase(p.days_taken[index].begin(), p.days_taken[index].end());
		}
	}

	void db_impl::load_file() {}

	void db_impl::save_file() {}

	// Clear all data
	void db_impl::clear() {
		people.clear();
		people.shrink_to_fit();
		day_types.clear();
		day_types.shrink_to_fit();
		current_file_name = "vdb.json";
		io_lock.store(false);
		io_percentage.store(0);
		io_future = decltype(io_future)();
		io_curop = IO_Status_t::NOOP;
	}
}
