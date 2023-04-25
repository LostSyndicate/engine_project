#include "Core/Profiler.h"

#include <algorithm>
#include <SDL2/SDL.h>

static std::hash<std::string> hash_;
static std::vector<Profiler::Profile> profiles_;

size_t last_call_ = 0;
static std::vector<size_t> call_stack_;

size_t Profiler::Begin(std::string func_name)
{
	size_t id = hash_(func_name);
	if (!call_stack_.empty())
		id += call_stack_[call_stack_.size() - 1];
	auto i = std::find(profiles_.begin(), profiles_.end(), id);
	if (i == profiles_.end()) {
		Profile p = Profile();
		p.name = func_name;
		p.start = 0;
		p.end = 0;
		p.ms = 0;
		p.calling_id = 0;
		p.calls = 0;
		p.call_count_ = 0;
		if (!call_stack_.empty())
			p.calling_id = call_stack_[call_stack_.size() - 1];
		p.id = id;
		profiles_.push_back(p);
		i = profiles_.end() - 1;
	}
	if (i->call_count_ == 0)
		i->start = SDL_GetTicks64();
	i->call_count_++;
	call_stack_.push_back(id);
	return id;
}

void Profiler::End(std::string func_name)
{
	size_t id = hash_(func_name);
	if (!call_stack_.empty()) call_stack_.erase(call_stack_.end() - 1);
	if (!call_stack_.empty()) id += call_stack_[call_stack_.size() - 1];
	if (last_call_ != id && last_call_ != 0) {
		auto f = std::find(profiles_.begin(), profiles_.end(), last_call_);
		f->call_count_ = 0;
	}
	auto i = std::find(profiles_.begin(), profiles_.end(), id);
	if (i == profiles_.end()) return;
	auto &s = *i;
	s.end = SDL_GetTicks64();
	s.ms = static_cast<uint32_t>(s.end - s.start);
	s.calls = std::max(s.calls, s.call_count_);
	last_call_ = id;
}

Profiler::Profile Profiler::GetProfile(std::string func_name)
{
	size_t id = hash_(func_name);
	for (auto& p : profiles_) {
		if (p.id == id)
			return p;
	}
	return Profile();
}

Profiler::Profile Profiler::GetProfile(size_t id)
{
	auto i = std::find(profiles_.begin(), profiles_.end(), id);
	if (i == profiles_.end())
		return Profile();
	return *i;
}

std::vector<Profiler::Profile> Profiler::GetProfiles()
{
	return profiles_;
}

std::vector<Profiler::Profile> Profiler::GetProfilesSortedByHighestTime()
{
	std::vector<Profile> profiles = profiles_;
	std::sort(profiles.begin(), profiles.end(), [](Profile a, Profile b) {
		return a.ms > b.ms;
	});
	return profiles;
}