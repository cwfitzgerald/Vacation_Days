#pragma once

#include <cinttypes>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// clang-format off

#define VACATIONDB_strong_typedef(T, Name)     \
	class Name {                               \
	  private:                                 \
		T t;                                   \
                                               \
	  public:                                  \
		Name() : t(){};                        \
		Name(const Name& n) : t(n.t){};        \
		Name(Name&& n) : t(std::move(n.t)){};  \
                                               \
		explicit Name(const T& i) : t(i){};    \
		explicit Name(T&& i) : t(i){};         \
                                               \
		Name& operator=(const Name& n) {       \
			t = n.t;                           \
			return *this;                      \
		};                                     \
		Name& operator=(Name&& n) {            \
			t = std::move(n.t);                \
			return *this;                      \
		};                                     \
                                               \
		operator const T&() const {            \
			return t;                          \
		};                                     \
		operator T&() {                        \
			return t;                          \
		};                                     \
	}

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
	}

	struct Invalid_Date  : public std::exception{
		virtual const char * what () const noexcept {
			return "The date supplied was invalid";
		}
	};

	struct Invalid_Number  : public std::exception{
		virtual const char * what () const noexcept {
			return "The number supplied was invalid";
		}
	};

	struct Invalid_Index : public std::exception {
		virtual const char * what () const noexcept {
			return "The index supplied was invalid";
		}
	};

	struct Employee_Not_Found : public std::exception {
		virtual const char * what () const noexcept {
			return "The employee asked for was not found";
		}
	};

	struct Day_Not_Found : public std::exception {
		virtual const char * what () const noexcept {
			return "The vacation day type asked for was not found";
		}
	};

	// Types to represent an employee
	VACATIONDB_strong_typedef(size_t, PersonID_t);
	VACATIONDB_strong_typedef(size_t, Extra_TimeID_t);
	struct Person_Info_t {
		PersonID_t id;
		std::string name;
		uint16_t start_year;
		uint16_t start_month;
		uint16_t start_day;
		std::string work_time;
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
	VACATIONDB_strong_typedef(size_t, DayID_t);
	VACATIONDB_strong_typedef(size_t, RuleID_t);

	struct Day_Info_t {
		DayID_t id;
		std::string name;
		std::string rollover;
		std::string yearly_bonus;
		struct Day_Rule_t {
			RuleID_t id;
			uint32_t month_start;
			std::string days_per_year;
		};
		std::vector<Day_Rule_t> rules;
	};

	// A type to pass the total amount of vacation days
	// of a particular type a person has
	struct Person_Days_t {
		std::string day_name;
		std::string days;
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

	struct Date_t {
		uint16_t year;
		uint16_t month;
		uint16_t day;
		std::string amount;
	};
	
	class VACATIONDB_SHARED Database {
	  public:
		Database();
		Database(const Database&) = delete;
		Database(Database&&) = default;
		Database& operator=(const Database&) = delete;
		Database& operator=(Database&&) = default;
		~Database() = default;

		////////////////////////////////////////
		// Operations on individual employees //
		////////////////////////////////////////

		PersonID_t     add_employee                        (const char * name, uint16_t start_year, uint16_t start_month, uint16_t start_day, const char * work_time);
		void           edit_employee_name                  (const PersonID_t employee, const char * name);
		void           edit_employee_start_date            (const PersonID_t employee, uint16_t start_year, uint16_t start_month, uint16_t start_day);
		void           edit_employee_work_time             (const PersonID_t employee, const char * work_time);
		Extra_TimeID_t edit_employee_add_extra_work_time   (const PersonID_t employee, uint16_t start_year, uint16_t start_month, uint16_t start_day, 
		                                                    uint16_t end_year, uint16_t end_month, uint16_t end_day, const char * time);
		void           edit_employee_remove_extra_work_time(const PersonID_t, const Extra_TimeID_t);
		PersonID_t     find_employee   (const char * name);
		void           delete_employee (const PersonID_t employee);

		std::string    get_employee_name(const PersonID_t employee);
		Person_Info_t  get_employee_info(const PersonID_t employee);
		size_t         get_employee_count();

		std::vector<std::string>   list_employee_names();
		std::vector<Person_Info_t> list_employee_info();

		/////////////////////////////
		// Operations on day types //
		/////////////////////////////

		DayID_t  add_day               (const char * name, const char * rollover, const char * yearly_bonus);
		void     edit_day_name         (const DayID_t, const char * name);
		void     edit_day_rollover     (const DayID_t, const char * rollover);
		void     edit_day_yearly_bonus (const DayID_t, const char * yearly_bonus);
		RuleID_t edit_day_add_rule     (const DayID_t, uint32_t month_start, const char * days_per_year);
		void     edit_day_remove_rule  (const DayID_t, const RuleID_t);
		DayID_t  find_day              (const char * name);
		void     delete_day            (const DayID_t);

		std::string get_day_name(const DayID_t);
		Day_Info_t  get_day_info(const DayID_t);
		size_t      get_day_count();

		std::vector<std::string>   list_day_names();
		std::vector<Day_Info_t>    list_day_info();

		//////////////////////////////////////////////////////
		// Querying the amounts of days that employees have //
		//////////////////////////////////////////////////////

		void add_person_day   (const PersonID_t, const DayID_t, uint16_t year, uint16_t month, uint16_t day, const char * value);
		void remove_person_day(const PersonID_t, const DayID_t, uint16_t year, uint16_t month, uint16_t day);

		std::vector<Date_t> list_person_days(const PersonID_t, const DayID_t);

		std::string                query_person_days(const PersonID_t p, const DayID_t d, uint16_t year, uint16_t month, uint16_t day);
		std::vector<Person_Days_t> query_person_days(const PersonID_t p, uint16_t year, uint16_t month, uint16_t day); 

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
