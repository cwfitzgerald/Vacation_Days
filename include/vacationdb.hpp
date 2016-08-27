#pragma once

#include <boost/serialization/strong_typedef.hpp>
#include <cinttypes>
#include <memory>
#include <string>

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

namespace vacationdb {
	namespace _detail {
		class db_impl;
		
		struct db_impl_deleter {
			void operator()(db_impl* value);
		};
	};

	BOOST_STRONG_TYPEDEF(size_t, Person);
	
	class SHARED Database {
	  public:
	  	Person add_person(std::string name);
	  private:
		std::unique_ptr<_detail::db_impl, _detail::db_impl_deleter> impl;
	};
};