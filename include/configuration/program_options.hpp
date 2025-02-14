#pragma once

#include <ostream>

#include <boost/program_options.hpp>

namespace traceroute::configuration
{
    class program_options final {
    public:
	using const_reference = const boost::program_options::variable_value&;

	program_options();

	bool contains(const std::string& option)
	{

#if __cplusplus >= 202002L

	    return map.contains(option);

#else

	    return map.count(option);

#endif

	}

	void parse_command_line(int, char**);

	const_reference operator[](const std::string& option) const
	{
	    return map[option];
	}

	template<typename CharT>
	friend std::basic_ostream<CharT>&
	operator<<(std::basic_ostream<CharT>& ostream, const program_options& options)
	{
	    return ostream << "Usage: traceroute [options] <address>\n\n"
			   << "OPTIONS\n"
			   << options.description
			   << "\nFor more details see https://github.com/kcherno/traceroute";
	}

    private:
	boost::program_options::options_description            description;
	boost::program_options::positional_options_description positional_options;
	boost::program_options::variables_map                  map;
    };
}
