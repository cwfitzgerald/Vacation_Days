#pragma once

#include <boost/serialization/strong_typedef.hpp>
#include <cinttypes>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// clang-format off

#if defined(_WIN32) || defined(_CYGWIN)
	#ifdef VACATIONDB_EXPORT
		#define VACATIONDB_SHARED __declspec(dllexport)
	#else
		#define VACATIONDB_SHARED __declspec(dllimport)
	#endif
#else 
	#if __GNUC__ >= 4
		#ifdef VACATIONDB_EXPORT
			#define VACATIONDB_SHARED __attribute__((visibility ("default")))
		#else
			#define VACATIONDB_SHARED
		#endif
	#else
		#define VACATIONDB_SHARED
	#endif
#endif

namespace Vacationdb {
	// Forward declarations to allow for the pimpl idiom.
	namespace _detail {
		class db_impl;
		
		struct VACATIONDB_SHARED db_impl_deleter {
			void  operator()(db_impl* value);
		};
	};

	class Invalid_Date {
		virtual const char * what () const noexcept {
			return "The date supplied was invalid";
		}
	};

	class Invalid_Number {
		virtual const char * what () const noexcept {
			return "The number supplied was invalid";
		}
	};

	class Invalid_Index {
		virtual const char * what () const noexcept {
			return "The index supplied was invalid";
		}
	};

	class Employee_Not_Found {
		virtual const char * what () const noexcept {
			return "The employee you asked for was not found";
		}
	};

	class Day_Not_Found {
		virtual const char * what () const noexcept {
			return "The vacation day type you asked for was not found";
		}
	};

	// Types to represent an employee
	BOOST_STRONG_TYPEDEF(size_t, PersonID_t);
	BOOST_STRONG_TYPEDEF(size_t, Extra_TimeID_t);
	struct Person_Info_t {
		const PersonID_t id;
		const std::string name;
		const uint16_t start_year;
		const uint16_t start_month;
		const uint16_t start_day;
		const std::string work_time;
		struct Extra_Work_Time_Info_t {
			Extra_TimeID_t id;
			uint16_t start_year;
			uint16_t start_month;
			uint16_t start_day;
			uint16_t end_year;
			uint16_t end_month;
			uint16_t end_day;
			std::string time;
		};
		std::vector<Extra_Work_Time_Info_t> extra_work_time;
	};

	// Types to represent types of vacation days and
	// their respective rules.
	BOOST_STRONG_TYPEDEF(size_t, DayID_t);
	BOOST_STRONG_TYPEDEF(size_t, RuleID_t);

	struct Day_Info_t {
		const DayID_t id;
		const std::string name;
		const std::string rollover;
		const std::string yearly_bonus;
		struct Day_Rule_t {
			const RuleID_t id;
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

	// A type to pass the current status of loading/saving
	struct IO_Status_t {
		enum Op_t : uint8_t {
			NOOP = 0,
			LOAD = 1,
			SAVE = 2
		} operation;
		float percentage;
	};
	
	class VACATIONDB_SHARED Database {
	  public:
		Database();
		~Database() = default;

		////////////////////////////////////////
		// Operations on individual employees //
		////////////////////////////////////////

		PersonID_t     add_employee                        (const char * name, uint16_t start_year, uint16_t start_month, uint16_t start_day, const char * work_time);
		void           edit_employee_name                  (const PersonID_t employee, const char * name);
		void           edit_employee_start                 (const PersonID_t employee, uint16_t start_year, uint16_t start_month, uint16_t start_day);
		void           edit_employee_work_time             (const PersonID_t employee, const char * work_time);
		Extra_TimeID_t edit_employee_add_extra_work_time   (const PersonID_t employee, uint16_t start_year, uint16_t start_month, uint16_t start_day, 
		                                                    uint16_t end_year, uint16_t end_month, uint16_t end_day, const char * time);
		void           edit_employee_remove_extra_work_time(const PersonID_t, const Extra_TimeID_t);
		PersonID_t     find_employee   (const char * name);
		void           delete_employee (const PersonID_t employee);

		std::string    get_employee_name(const PersonID_t employee);
		Person_Info_t  get_employee_info(const PersonID_t employee);

		std::vector<std::string>   list_employee_names();
		std::vector<Person_Info_t> list_employee_info();

		/////////////////////////////
		// Operations on day types //
		/////////////////////////////

		DayID_t  add_day(const char * name, const char * rollover, const char * yearly_bonus);
		void     edit_day_rollover     (const DayID_t, const char * rollover);
		void     edit_day_yearly_bonus (const DayID_t, const char * yearly_bonus);
		RuleID_t edit_day_add_rule     (const DayID_t, uint32_t month_start, const char * days_per_year);
		void     edit_day_remove_rule  (const DayID_t, const RuleID_t);
		DayID_t  find_day              (const char * name);
		void     delete_day       (const DayID_t);

		std::string get_day_name(const DayID_t);
		Day_Info_t  get_day_info(const DayID_t);

		std::vector<std::string>   list_day_names();
		std::vector<Day_Info_t>    list_day_info();

		//////////////////////////////////////////////////////
		// Querying the amounts of days that employees have //
		//////////////////////////////////////////////////////

		void add_person_day   (const PersonID_t, const DayID_t, uint16_t year, uint16_t month, uint16_t day);
		void remove_person_day(const PersonID_t, const DayID_t, uint16_t year, uint16_t month, uint16_t day);

		std::string                query_person_days(const PersonID_t, const DayID_t);
		std::vector<Person_Days_t> query_person_days(const PersonID_t); 

		/////////////////////////////////
		// Loading/Saving the Database //
		/////////////////////////////////

		void        load       (const char * filename);
		void        load_async (const char * filename);
		void        save       (const char * filename);
		void        save_async (const char * filename);
		void        clear_db   ();
		std::string get_current_filename();

		IO_Status_t get_load_status();

	  private:
		std::unique_ptr<_detail::db_impl, _detail::db_impl_deleter> impl;
	};
}
