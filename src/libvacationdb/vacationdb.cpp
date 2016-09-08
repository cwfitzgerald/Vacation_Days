#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

#include "../vacationdb.hpp"
#include "database_impl.hpp"

namespace Vacationdb {
	void _detail::db_impl_deleter::operator()(db_impl* value) {
		delete value;
	}

	Database::Database() {
		std::unique_ptr<_detail::db_impl, _detail::db_impl_deleter> n(new _detail::db_impl);
		impl = std::move(n);
	}

	////////////////////////////////////////
	// Operations on individual employees //
	////////////////////////////////////////

	PersonID_t Database::add_employee(const char* name, uint16_t start_year, uint16_t start_month,
	                                  uint16_t start_day, const char* work_time) {
		impl->block_if_locked();

		std::string n{name};
		auto start_date = _detail::create_date_safe(start_year, start_month, start_day);
		auto wt         = _detail::create_number_safe(work_time);

		_detail::Person p{std::move(n),
		                  std::move(start_date),
		                  std::move(wt),
		                  {},
		                  std::vector<std::vector<boost::gregorian::date>>{impl->day_types.size()}};

		impl->people.emplace_back(std::move(p));

		return PersonID_t{impl->people.size() - 1};
	}

	void Database::edit_employee_name(const PersonID_t employee, const char* name) {
		impl->block_if_locked();
		impl->validate(employee);

		impl->people[employee].name = name;
	}

	void Database::edit_employee_start(const PersonID_t employee, uint16_t start_year,
	                                   uint16_t start_month, uint16_t start_day) {
		impl->block_if_locked();
		impl->validate(employee);

		auto new_date = _detail::create_date_safe(start_year, start_month, start_day);

		impl->people[employee].start_date = std::move(new_date);
	}

	void Database::edit_employee_work_time(const PersonID_t employee, const char* work_time) {
		impl->block_if_locked();
		impl->validate(employee);

		auto new_work_time = _detail::create_number_safe(work_time);

		impl->people[employee].percent_time = std::move(new_work_time);
	}

	Extra_TimeID_t Database::edit_employee_add_extra_work_time(
	    PersonID_t employee, uint16_t start_year, uint16_t start_month, uint16_t start_day,
	    uint16_t end_year, uint16_t end_month, uint16_t end_day, const char* time) {
		impl->block_if_locked();
		impl->validate(employee);

		_detail::Date start_date = _detail::create_date_safe(start_year, start_month, start_day);
		_detail::Date end_date   = _detail::create_date_safe(end_year, end_month, end_day);
		_detail::Number time_num = _detail::create_number_safe(time);

		impl->people[employee].extra_time.push_back(_detail::Person::Extra_Time_t{
		    std::move(start_date), std::move(end_date), std::move(time_num)});

		return Extra_TimeID_t{impl->people[employee].extra_time.size() - 1};
	}

	void Database::edit_employee_remove_extra_work_time(const PersonID_t p,
	                                                    const Extra_TimeID_t e) {
		impl->block_if_locked();
		impl->validate(p, e);

		impl->people[p].extra_time[e].valid = false;
	}

	PersonID_t Database::find_employee(const char* name) {
		impl->block_if_locked();

		auto employee_it =
		    std::find_if(impl->people.begin(), impl->people.end(),
		                 [name](_detail::Person& p) { return p.valid && (p.name == name); });

		bool found = employee_it != impl->people.end();
		if (found) {
			return PersonID_t{
			    static_cast<size_t>(std::abs(std::distance(impl->people.begin(), employee_it)))};
		}
		else {
			throw Vacationdb::Employee_Not_Found();
		}
	}

	void Database::delete_employee(const PersonID_t employee) {
		impl->block_if_locked();
		impl->validate(employee);

		impl->people[employee].valid = false;
	}

	std::string Database::get_employee_name(const PersonID_t employee) {
		impl->block_if_locked();
		impl->validate(employee);

		return impl->people[employee].name;
	}

	Person_Info_t Database::get_employee_info(const PersonID_t employee) {
		impl->block_if_locked();
		impl->validate(employee);

		_detail::Person& p = impl->people[employee];

		std::ostringstream work_time;
		work_time << p.percent_time;

		using ewti_type = Person_Info_t::Extra_Work_Time_Info_t;
		std::vector<ewti_type> ewti;
		ewti.reserve(p.extra_time.size());
		size_t i = 0;
		for (auto&& et : p.extra_time) {
			if (et.valid) {
				uint16_t start_year  = et.begin.year();
				uint16_t start_month = et.begin.month();
				uint16_t start_day   = et.begin.day();
				uint16_t end_year    = et.end.year();
				uint16_t end_month   = et.end.month();
				uint16_t end_day     = et.end.day();
				std::ostringstream percent;
				percent << et.percent_time;
				ewti.push_back(ewti_type{Extra_TimeID_t{i}, start_year, start_month, start_day,
				                         end_year, end_month, end_day, percent.str()});
			}
			i++;
		}

		Person_Info_t pi{
		    employee,           p.name,          p.start_date.year(), p.start_date.month(),
		    p.start_date.day(), work_time.str(), std::move(ewti)};

		return pi;
	}

	std::vector<std::string> Database::list_employee_names() {
		impl->block_if_locked();

		std::vector<std::string> ret;
		ret.reserve(impl->people.size());

		for (auto&& p : impl->people) {
			if (p.valid) {
				ret.emplace_back(p.name);
			}
		}

		return ret;
	}

	std::vector<Person_Info_t> Database::list_employee_info() {
		impl->block_if_locked();

		std::vector<Person_Info_t> ret;
		ret.reserve(impl->people.size());

		for (size_t i = 0; i < impl->people.size(); ++i) {
			if (impl->people[i].valid) {
				ret.push_back(this->get_employee_info(PersonID_t{i}));
			}
		}

		return ret;
	}

	/////////////////////////////
	// Operations on day types //
	/////////////////////////////

	DayID_t Database::add_day(const char* name, const char* rollover, const char* yearly_bonus) {
		impl->block_if_locked();

		auto rollover_number     = _detail::create_number_safe(rollover);
		auto yearly_bonus_number = _detail::create_number_safe(yearly_bonus);

		_detail::Day d{name, std::move(rollover_number), std::move(yearly_bonus_number), {}};

		impl->day_types.emplace_back(std::move(d));
		impl->add_day_to_people();

		return DayID_t{impl->day_types.size() - 1};
	}

	void Database::edit_day_rollover(const DayID_t d, const char* rollover) {
		impl->block_if_locked();
		impl->validate(d);

		auto rollover_number = _detail::create_number_safe(rollover);

		impl->day_types[d].rollover = std::move(rollover_number);
	}

	void Database::edit_day_yearly_bonus(const DayID_t d, const char* yearly_bonus) {
		impl->block_if_locked();
		impl->validate(d);

		auto yearly_bonus_number = _detail::create_number_safe(yearly_bonus);

		impl->day_types[d].rollover = std::move(yearly_bonus_number);
	}

	RuleID_t Database::edit_day_add_rule(DayID_t day, uint32_t month_start,
	                                     const char* days_per_year) {
		impl->block_if_locked();
		impl->validate(day);

		auto dpy = _detail::create_number_safe(days_per_year);

		_detail::Day::Day_Rules_Data drd{month_start, std::move(dpy)};

		impl->day_types[day].rules.push_back(std::move(drd));

		return RuleID_t{impl->day_types[day].rules.size() - 1};
	}

	void Database::edit_day_remove_rule(DayID_t day, RuleID_t rule) {
		impl->block_if_locked();
		impl->validate(day, rule);

		impl->day_types[day].rules[rule].valid = false;
	}

	DayID_t Database::find_day(const char* name) {
		impl->block_if_locked();

		auto found_it =
		    std::find_if(impl->day_types.begin(), impl->day_types.end(),
		                 [name](_detail::Day& d) { return d.valid && (d.name == name); });

		bool found = found_it != impl->day_types.end();
		if (found) {
			return DayID_t{
			    static_cast<size_t>(std::abs(std::distance(impl->day_types.begin(), found_it)))};
		}
		else {
			throw Vacationdb::Day_Not_Found();
		}
	}

	void Database::delete_day(const DayID_t d) {
		impl->block_if_locked();
		impl->validate(d);

		impl->day_types[d].valid = false;
	}

	std::string Database::get_day_name(const DayID_t d) {
		impl->block_if_locked();
		impl->validate(d);

		return impl->day_types[d].name;
	}

	Day_Info_t Database::get_day_info(const DayID_t d) {
		impl->block_if_locked();
		impl->validate(d);

		auto& internal = impl->day_types[d];

		std::ostringstream ro;
		ro << internal.rollover;

		std::ostringstream yb;
		yb << internal.yearly_bonus;

		std::vector<Day_Info_t::Day_Rule_t> r;
		r.reserve(internal.rules.size());

		size_t i = 0;
		for (auto&& rule : internal.rules) {
			uint32_t mb = rule.month_begin;
			std::ostringstream dpy;
			dpy << rule.days_per_year;

			Day_Info_t::Day_Rule_t{RuleID_t{i++}, mb, dpy.str()};
		}

		Day_Info_t ret{d, internal.name, ro.str(), yb.str(), std::move(r)};

		return ret;
	}

	std::vector<std::string> Database::list_day_names() {
		impl->block_if_locked();

		std::vector<std::string> ret;
		for (auto&& dt : impl->day_types) {
			if (dt.valid) {
				ret.push_back(dt.name);
			}
		}

		return ret;
	}

	std::vector<Day_Info_t> Database::list_day_info() {
		impl->block_if_locked();

		std::vector<Day_Info_t> ret;
		size_t i = 0;
		for (auto&& dt : impl->day_types) {
			if (dt.valid) {
				ret.push_back(this->get_day_info(DayID_t{i++}));
			}
		}

		return ret;
	}

	//////////////////////////////////////////////////////
	// Querying the amounts of days that employees have //
	//////////////////////////////////////////////////////

	void Database::add_person_day(const PersonID_t p, const DayID_t d, uint16_t year,
	                              uint16_t month, uint16_t day) {
		impl->block_if_locked();
		impl->validate(p);
		impl->validate(d);

		auto date = _detail::create_date_safe(year, month, day);

		impl->people[p].days_taken[d].push_back(std::move(date));
	}

	void Database::remove_person_day(const PersonID_t p, const DayID_t d, uint16_t year,
	                                 uint16_t month, uint16_t day) {
		impl->block_if_locked();
		impl->validate(p);
		impl->validate(d);

		auto date = _detail::create_date_safe(year, month, day);

		auto& dates = impl->people[p].days_taken[d];
		auto it =
		    std::find_if(dates.begin(), dates.end(), [&date](auto cur) { return date == cur; });

		if (it != dates.end()) {
			dates.erase(it);
		}
	}

	/////////////////////////////////
	// Loading/Saving the Database //
	/////////////////////////////////

	void Database::load(const char* name) {
		impl->current_file_name = name;
		impl->load_file();
		impl->block_if_locked();
	}

	void Database::load_async(const char* name) {
		impl->current_file_name = name;
		impl->load_file();
	}

	void Database::save(const char* name) {
		impl->current_file_name = name;
		impl->save_file();
		impl->block_if_locked();
	}

	void Database::save_async(const char* name) {
		impl->current_file_name = name;
		impl->save_file();
	}

	void Database::clear_db() {
		impl->clear();
	}

	std::string Database::get_current_filename() {
		return impl->current_file_name;
	}

	IO_Status_t Database::get_load_status() {
		return IO_Status_t{impl->io_curop, impl->io_percentage.load()};
	}
}
