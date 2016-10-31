#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
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
		auto wt = _detail::create_number_safe(work_time);

		_detail::Person p{
		    std::move(n),
		    std::move(start_date),
		    std::move(wt),
		    {},
		    std::vector<std::vector<_detail::Person::Day_Taken_t>>{impl->day_types.size()}};

		impl->people.emplace_back(std::move(p));

		return PersonID_t{impl->people.size() - 1};
	}

	void Database::edit_employee_name(const PersonID_t employee, const char* name) {
		impl->block_if_locked();
		impl->validate(employee);

		impl->people[employee].name = name;
	}

	void Database::edit_employee_start_date(const PersonID_t employee, uint16_t start_year,
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
		_detail::Date end_date = _detail::create_date_safe(end_year, end_month, end_day);
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

		std::string work_time = p.percent_time.convert_to<std::string>();

		using ewti_type = Person_Info_t::Extra_Work_Time_Info_t;
		std::vector<ewti_type> ewti;
		ewti.reserve(p.extra_time.size());
		size_t i = 0;
		for (auto&& et : p.extra_time) {
			if (et.valid) {
				uint16_t start_year = et.begin.year();
				uint16_t start_month = et.begin.month();
				uint16_t start_day = et.begin.day();
				uint16_t end_year = et.end.year();
				uint16_t end_month = et.end.month();
				uint16_t end_day = et.end.day();
				std::string percent = et.percent_time.convert_to<std::string>();

				ewti.push_back(ewti_type{Extra_TimeID_t{i}, start_year, start_month, start_day,
				                         end_year, end_month, end_day, percent});
			}
			i++;
		}

		Person_Info_t pi{employee,
		                 p.name,
		                 p.start_date.year(),
		                 p.start_date.month(),
		                 p.start_date.day(),
		                 std::move(work_time),
		                 std::move(ewti)};

		return pi;
	}

	size_t Database::get_employee_count() {
		impl->block_if_locked();

		size_t valid_count =
		    std::accumulate(impl->people.begin(), impl->people.end(), size_t{0},
		                    [](size_t num, _detail::Person& p) { return num + (p.valid); });

		return valid_count;
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

		auto rollover_number = _detail::create_number_safe(rollover);
		auto yearly_bonus_number = _detail::create_number_safe(yearly_bonus);

		_detail::Day d{name, std::move(rollover_number), std::move(yearly_bonus_number), {}};

		impl->day_types.emplace_back(std::move(d));
		impl->add_day_to_people();

		return DayID_t{impl->day_types.size() - 1};
	}

	void Database::edit_day_name(const DayID_t d, const char* name) {
		impl->block_if_locked();
		impl->validate(d);

		impl->day_types[d].name = name;
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

		impl->day_types[d].yearly_bonus = std::move(yearly_bonus_number);
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

		std::string ro = internal.rollover.convert_to<std::string>();

		std::string yb = internal.yearly_bonus.convert_to<std::string>();

		std::vector<Day_Info_t::Day_Rule_t> r;
		r.reserve(internal.rules.size());

		size_t i = 0;
		for (auto&& rule : internal.rules) {
			if (rule.valid) {
				uint32_t mb = rule.month_begin;
				std::string dpy = rule.days_per_year.convert_to<std::string>();

				r.push_back(Day_Info_t::Day_Rule_t{RuleID_t{i++}, mb, std::move(dpy)});
			}
		}

		Day_Info_t ret{d, internal.name, std::move(ro), std::move(yb), std::move(r)};

		return ret;
	}

	size_t Database::get_day_count() {
		impl->block_if_locked();

		size_t valid_count =
		    std::accumulate(impl->day_types.begin(), impl->day_types.end(), size_t{0},
		                    [](size_t num, _detail::Day& day) { return num + (day.valid); });

		return valid_count;
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
				ret.push_back(this->get_day_info(DayID_t{i}));
			}
			i++;
		}

		return ret;
	}

	//////////////////////////////////////////////////////
	// Querying the amounts of days that employees have //
	//////////////////////////////////////////////////////

	void Database::add_person_day(const PersonID_t p, const DayID_t d, uint16_t year,
	                              uint16_t month, uint16_t day, const char* value) {
		impl->block_if_locked();
		impl->validate(p);
		impl->validate(d);

		auto date = _detail::create_date_safe(year, month, day);
		auto val = _detail::create_number_safe(value);

		impl->people[p].days_taken[d].push_back(
		    _detail::Person::Day_Taken_t{std::move(date), std::move(val)});
	}

	void Database::remove_person_day(const PersonID_t p, const DayID_t d, uint16_t year,
	                                 uint16_t month, uint16_t day) {
		impl->block_if_locked();
		impl->validate(p);
		impl->validate(d);

		auto date = _detail::create_date_safe(year, month, day);

		auto& dates = impl->people[p].days_taken[d];
		auto it =
		    std::find_if(dates.begin(), dates.end(), [&date](auto cur) { return date == cur.day; });

		if (it != dates.end()) {
			dates.erase(it);
		}
	}

	std::vector<Date_t> Database::list_person_days(const PersonID_t p, const DayID_t d) {
		impl->block_if_locked();
		impl->validate(p);
		impl->validate(d);

		auto&& source_array = impl->people[p].days_taken[d];

		std::vector<Date_t> ret;
		ret.reserve(source_array.size());

		for (auto& taken : source_array) {
			uint16_t year = taken.day.year();
			uint16_t month = taken.day.month();
			uint16_t day = taken.day.day();
			std::string amount = taken.value.convert_to<std::string>();

			ret.push_back(Date_t{year, month, day, std::move(amount)});
		}

		return ret;
	}

	std::string Database::query_person_days(const PersonID_t p, const DayID_t d, uint16_t year,
	                                        uint16_t month, uint16_t day) {
		impl->block_if_locked();
		impl->validate(p);
		impl->validate(d);

		using namespace boost::gregorian;

		struct Event_t {
			_detail::Date date;
			enum Tag_t {
				Extra_Time_Event = 0,
				Day_Rules_Event = 1,
				Year_Start_Event = 2,
				End_of_Query_Event = 3
			} tag;

			struct Extra_Time_Event_t {
				_detail::Number value;
				enum : bool { enable = 1, disable = 0 } on;
			};

			struct Day_Rules_Event_t {
				_detail::Number value;
			};

			boost::variant<Extra_Time_Event_t, Day_Rules_Event_t, std::nullptr_t> data;
		};

		// References to appropriate data
		auto&& person = impl->people[p];
		auto&& day_type = impl->day_types[d];

		auto query_date = _detail::create_date_safe(year, month, day);

		// Sum up the total amount of events to expect
		// May overallocate due to invalid events
		size_t num_ete = person.extra_time.size() * 2;                        // Extra time events
		size_t num_dre = day_type.rules.size();                               // Day rule events
		size_t num_yse = ((query_date - person.start_date).days() / 365) + 2; // Year start events
		size_t num_eqe = 1;                                                   // End of query event

		// Create and reserve the appropriate amount of space in the array
		std::vector<Event_t> events;
		events.reserve(num_ete + num_dre + num_yse + num_eqe);

		// Add all extra time events
		for (auto&& data : person.extra_time) {
			using s_t = Event_t::Extra_Time_Event_t;
			auto e_t = Event_t::Extra_Time_Event;

			if (data.valid) {
				auto start_event = s_t{data.percent_time, static_cast<decltype(s_t::on)>(true)};
				auto end_event = s_t{person.percent_time, static_cast<decltype(s_t::on)>(false)};

				events.push_back(Event_t{data.begin, e_t, std::move(start_event)});
				events.push_back(Event_t{data.end, e_t, std::move(end_event)});
			}
		}

		// Add all Day Rule Events
		for (auto&& data : day_type.rules) {
			using s_t = Event_t::Day_Rules_Event_t;
			auto e_t = Event_t::Day_Rules_Event;

			if (data.valid) {
				auto date = person.start_date + months{static_cast<int32_t>(data.month_begin)};
				auto event = s_t{data.days_per_year};

				events.push_back(Event_t{std::move(date), e_t, std::move(event)});
			}
		}

		// Add all year start events
		// Including the one at the beginning of their employment
		{
			auto e_t = Event_t::Year_Start_Event;
			events.push_back(Event_t{person.start_date, e_t, nullptr});

			int16_t start_year = person.start_date.year();
			_detail::Date working_date;
			for (size_t i = 1; (working_date = _detail::Date(start_year + i, 1, 1)) <= query_date;
			     ++i) {
				events.push_back(Event_t{working_date, e_t, nullptr});
			}
		}

		// Add the single end of query event
		events.push_back(Event_t{query_date, Event_t::End_of_Query_Event, nullptr});

		// Sort the structures into chronological order
		auto sort_func = [](Event_t left, Event_t right) -> bool {
			if (left.date != right.date) {
				return left.date < right.date;
			}
			else {
				return left.tag < right.tag;
			}
		};

		std::sort(events.begin(), events.end(), sort_func);

		for (auto&& event : events) {
			std::cout << event.date << " - ";
			switch (event.tag) {
				case Event_t::Extra_Time_Event:
					std::cout << "Extra time event\n";
					break;
				case Event_t::Day_Rules_Event:
					std::cout << "Day rules event\n";
					break;
				case Event_t::Year_Start_Event:
					std::cout << "Year start event\n";
					break;
				case Event_t::End_of_Query_Event:
					std::cout << "End of query event\n";
					break;
			}
		}

		// Use a state machine to calculate the amount of days accrued.
		_detail::Number accrued{0};
		_detail::Number& default_percent = person.percent_time;
		_detail::Number current_rate{0};
		_detail::Number current_percent = default_percent;
		_detail::Date current_date = person.start_date;
		bool end_of_query = false;

		for (auto&& event : events) {
			switch (event.tag) {
				case Event_t::Extra_Time_Event: {
					auto&& data = boost::get<Event_t::Extra_Time_Event_t>(event.data);
					if (data.on) {
						current_percent = data.value;
					}
					else {
						current_percent = default_percent;
					}
					break;
				}

				case Event_t::Day_Rules_Event: {
					auto&& data = boost::get<Event_t::Day_Rules_Event_t>(event.data);
					current_rate = data.value;
				}

				case Event_t::Year_Start_Event: {
					// Negative values signal complete rollover
					if (day_type.rollover >= 0) {
						accrued = std::min(accrued, day_type.rollover);
					}
					accrued += day_type.yearly_bonus;
					break;
				}

				case Event_t::End_of_Query_Event: {
					end_of_query = true;
					break;
				}
			}

			auto diff = (event.date - current_date);
			auto diff_days = diff.days();
			auto diff_val = _detail::Number{diff_days};
			auto year_val = _detail::create_number_safe("365.24");
			auto years = diff_val / year_val;
			accrued += (years * current_rate * current_percent);
			current_date = event.date;

			std::cout << accrued << '\n';

			if (end_of_query) {
				break;
			}
		}

		// Calculate the amount of vacation days taken
		auto accumulate_func = [&query_date](_detail::Number& val,
		                                     _detail::Person::Day_Taken_t& dt) {
			// Add the value of the date, only if the date is before the query date
			return val + (dt.day < query_date ? dt.value : _detail::Number{0});
		};

		auto taken = std::accumulate(person.days_taken[d].begin(), person.days_taken[d].end(),
		                             _detail::Number{0}, accumulate_func);

		_detail::Number total_val = accrued - taken;

		// Convert amount to string, and return
		auto outstring = total_val.convert_to<std::string>();
		return outstring;
	}

	std::vector<Person_Days_t> Database::query_person_days(const PersonID_t p, uint16_t year,
	                                                       uint16_t month, uint16_t day) {
		impl->block_if_locked();
		impl->validate(p);

		std::vector<Person_Days_t> ret;

		size_t i = 0;
		for (auto&& day_type : impl->day_types) {
			if (day_type.valid) {
				auto&& day_name = day_type.name;
				auto value = this->query_person_days(p, DayID_t{i}, year, month, day);
				ret.push_back(Person_Days_t{day_name, value});
			}
			i++;
		}

		return ret;
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
