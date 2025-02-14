#include <exception>
#include <iostream>
#include <iomanip>
#include <limits>

#include <cstdint>

#include <boost/asio.hpp>

#include "traceroute.hpp"

int main(int argc, char** argv)
{
    try
    {
	if (argc == 1)
	{
	    std::cerr << "Usage: traceroute <address>" << std::endl;

	    return 1;
	}

	boost::asio::io_context io_context;

	traceroute::core::icmp_pinger icmp_pinger {io_context, boost::asio::ip::make_address(argv[1])};

	for (int i = 1; i <= std::numeric_limits<std::uint8_t>::max(); ++i)
	{
	    icmp_pinger.hop_limit(i);

	    auto source_address = icmp_pinger.ping();

	    if (source_address.is_loopback())
	    {
		--i;

		continue;
	    }

	    std::cout << std::setw(3) << i << ". ";

	    if (not source_address.is_unspecified())
	    {
		auto host_name = icmp_pinger.resolve_last_address();

		if (not host_name.empty())
		{
		    std::cout << host_name << " (" << source_address.to_string() << ')';
		}

		else
		{
		    std::cout << source_address.to_string();
		}
	    }

	    std::cout << std::endl;

	    if (source_address == icmp_pinger.destination_address())
	    {
		break;
	    }
	}
    }

    catch (const std::exception& e)
    {
	std::cerr << e.what() << std::endl;

	return 2;
    }

    return 0;
}
