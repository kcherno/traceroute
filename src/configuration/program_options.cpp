#include <string>

#include <cstdint>

#include <boost/program_options.hpp>

#include "configuration/program_options.hpp"

namespace traceroute::configuration
{
    program_options::program_options()
    {
	description.add_options()
	    (",a",
	     boost::program_options::value<std::string>()
	     ->value_name("<address>"),
	     "ip address")

	    (",h", "produce help message")

	    (",m",
	     boost::program_options::value<std::uint8_t>()
	     ->default_value(30, "")
	     ->value_name("<ttl>"),
	     "set max TTL, default is 30")

	    (",W",
	     boost::program_options::value<int>()
	     ->default_value(1, "")
	     ->value_name("<timeout>"),
	     "time to wait for response");

	positional_options.add("-a", 1);
    }

    void program_options::parse_command_line(int argc, char** argv)
    {
	boost::program_options::store(boost::program_options::command_line_parser(argc, argv)
				      .options(description)
				      .positional(positional_options)
				      .style(boost::program_options::command_line_style::allow_long           |
					     boost::program_options::command_line_style::allow_short          |
					     boost::program_options::command_line_style::allow_dash_for_short |
					     boost::program_options::command_line_style::long_allow_adjacent  |
					     boost::program_options::command_line_style::short_allow_next)
				      .run(),
				      map);

	map.notify();
    }
}
