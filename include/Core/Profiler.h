#ifndef SM3_PROFILER_H
#define SM3_PROFILER_H

#include <stdint.h>
#include <map>
#include <vector>

#include "Reference.h"

// set to 1 to enable core profiling. this significantly decreases performance.
#define PERF_ENABLED 1
#define PROFILER_MAX_SAMPLE_COUNT 128

// TODO: handle return's for the profiler.
//       an easy way of doing this set a bool to true when end() is called and
//       check if it has not in begin(), then call End() or something.
//       This way it still stores the result even if it may not be accurate.

// record 1 frame of data every second, then cap at 10 seconds
class Profiler
{
public:
	struct Profile
	{
		std::string name;
		size_t id{ 0 };
		size_t calling_id{ 0 }; // 0 for none
		uint16_t calls{ 0 };
		uint64_t start{ 0 };
		uint64_t end{ 0 };
		uint32_t ms{ 0 };

		uint16_t call_count_{ 0 };
		bool operator==(const size_t& b) { return id == b; }
	};

	static size_t Begin(std::string func_name);
	static void End(std::string func_name);
	static Profile GetProfile(std::string func_name);
	static Profile GetProfile(size_t id);
	static std::vector<Profile> GetProfiles();
	// sort by averageMS
	static std::vector<Profile> GetProfilesSortedByHighestTime();
};

#if PERF_ENABLED == 1
#	define PERFN_BEG(n) Profiler::Begin(n);
#	define PERFN_END(n) Profiler::End(n);
#	define PERF_BEG Profiler::Begin(__PRETTY_FUNCTION__);
#	define PERF_END Profiler::End(__PRETTY_FUNCTION__);
#else
#	define PERFN_BEG(n)
#	define PERFN_END(n)
#	define PERF_BEG
#	define PERF_END
#endif

#endif