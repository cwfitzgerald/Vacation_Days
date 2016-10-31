#pragma once

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/multiprecision/gmp.hpp>

#include <atomic>
#include <cinttypes>
#include <future>
#include <string>
#include <type_traits>
#include <vector>

#include "../vacationdb.hpp"

namespace Vacationdb::_detail {
	using Date = boost::gregorian::date;
	using Number = boost::multiprecision::mpq_rational;

	struct Person {
		std::string name;
		Date start_date;
		Number percent_time;
		struct Extra_Time_t {
			Date begin;
			Date end;
			Number percent_time;
			bool valid = true;
		};
		std::vector<Extra_Time_t> extra_time;
		struct Day_Taken_t {
			Date day;
			Number value;
		};
		std::vector<std::vector<Day_Taken_t>> days_taken;
		bool valid = true;
	};

	static_assert(std::is_move_constructible<Person>::value, "Person must be move constructible");
	static_assert(std::is_move_assignable<Person>::value, "Person must be move assignable");

	struct Day {
		std::string name;
		Number rollover;
		Number yearly_bonus;
		struct Day_Rules_Data {
			uint32_t month_begin;
			Number days_per_year;
			bool valid = true;
		};
		std::vector<Day_Rules_Data> rules;
		bool valid = true;
	};

	static_assert(std::is_move_constructible<Day>::value, "Day must be move constructible");
	static_assert(std::is_move_assignable<Day>::value, "Day must be move assignable");

	Date create_date_safe(uint16_t start_year, uint16_t start_month, uint16_t start_day);
#ifdef LIBVACATIONDB_TEST
	VACATIONDB_SHARED
#endif
	Number create_number_safe(const char* value);

	class db_impl {
	  public:
		db_impl() : io_lock(false){};

		std::vector<Person> people;
		std::vector<Day> day_types;
		void validate(PersonID_t);
		void validate(PersonID_t, Extra_TimeID_t);
		void validate(DayID_t);
		void validate(DayID_t, RuleID_t);
		void add_day_to_people();
		void remove_day_from_people(size_t index);

		// File loading
		std::string current_file_name = "vdb.json";
		std::atomic<bool> io_lock;
		std::atomic<float> io_percentage;
		std::future<void> io_future;
		Vacationdb::IO_Status_t::Op_t io_curop = IO_Status_t::NOOP;
		void block_if_locked();
		void load_file();
		void save_file();
		void clear();
	};
}
