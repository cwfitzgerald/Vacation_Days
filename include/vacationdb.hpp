#pragma once

#include <boost/serialization/strong_typedef.hpp>
#include <cinttypes>
#include <memory>
#include <string>
#include <vector>

// clang-format off

#if defined(_WIN32) || defined(_CYGWIN)
	#ifdef VACATIONDB_EXPORT
		#define SHARED __declspec(dllexport)
	#else
		#define SHARED __declspec(dllimport)
	#endif
#else 
	#if __GNUC__ >= 4
		#ifdef VACATIONDB_EXPORT
			#define SHARED __attribute__((visibility ("default")))
		#else
			#define SHARED __attribute__((visibility ("hidden")))
		#endif
	#endif
#endif

namespace Vacationdb {
	namespace _detail {
		class db_impl;
		
		struct db_impl_deleter {
			void operator()(db_impl* value);
		};
	};

	// Types to represent an employee
	BOOST_STRONG_TYPEDEF(size_t, Person_t);
	struct Person_Info_t {
		const std::string name;
		const size_t start_year;
		const size_t start_month;
		const size_t start_day;
		const std::string work_time;
	};

	// Types to represent types of vacation days and
	// their respective rules.
	BOOST_STRONG_TYPEDEF(size_t, Day_t);
	BOOST_STRONG_TYPEDEF(size_t, Rule_t);
	struct Day_Info_t {
		const std::string name;
		struct Day_Rule_t {
			uint32_t month_start;
			std::string days_per_year;
		};
		const std::vector<Day_Rule_t> rules;
	};

	// A type to pass the total amount of vacation days
	// of a particular type a person has
	struct Person_Days_t {
		const std::string day_name;
		const std::string days;
	};
	
	class SHARED Database {
	  public:
	  	//
	  	// Operations on individual employees
	  	//
	  	Person_t add_employee(const char * name, uint32_t start_year, uint32_t start_month, uint32_t start_day, const char * work_time);
	  	void     edit_employee_name(const Person_t employee, const char * name);
	  	void     edit_employee_start(const Person_t employee, uint32_t start_year, uint32_t start_month, uint32_t start_day);
	  	void     edit_employee_work_time(const Person_t employee, const char * work_time);
	  	Person_t find_employee(const char * name);
	  	void     delete_employee(const Person_t employee);

	  	std::string get_employee_name(const Person_t employee);
	  	Person_Info_t get_employee_info(const Person_t employee);

	  	std::vector<std::string> list_employee_names();
	  	std::vector<Person_Info_t> list_employee_info();

	  	//
	  	// Operations on day types
	  	//
	  	Day_t  add_day(const char * name);
	  	Rule_t edit_day_add_rule(Day_t, uint32_t month_start, const char * days_per_year);
	  	void   edit_day_remove_rule(Day_t, Rule_t);
	  	Day_t  find_day(const char * name);
	  	void   delete_day_type(Day_t day_tyep);

	  	std::string get_day_name(const Day_t);
	  	Day_Info_t get_day_info(const Day_t);

	  	std::vector<std::string> list_day_names();
	  	std::vector<Person_Info_t> list_day_info();

	  	//
	  	// Querying the amounts of days that employees have
	  	// 

	  	std::string query_person_days(Person_t, Day_t);
	  	std::vector<Person_Days_t> query_person_days(Person_t); 

	  private:
		std::unique_ptr<_detail::db_impl, _detail::db_impl_deleter> impl;
	};
};
