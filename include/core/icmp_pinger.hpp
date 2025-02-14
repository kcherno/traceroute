#pragma once

#include <type_traits>
#include <algorithm>
#include <chrono>
#include <string>
#include <array>

#include <cstdint>

#include <boost/asio.hpp>

#include "proto.hpp"

namespace traceroute::core
{
    template<typename ExecutorType>
    class icmp_pinger final : public pinger_interface {
    public:
	static_assert(std::is_same_v<std::remove_cv_t<ExecutorType>, ExecutorType>,
		      "ExecutorType must be a non-const, non-volatile");

	icmp_pinger(ExecutorType& executor) :
	    icmp_pinger {executor, boost::asio::ip::address {}}
	{}

	icmp_pinger(ExecutorType& executor, const boost::asio::ip::address& address) :
	    pinger_interface {address},

	    executor_   {executor},
	    destination {address, 0},
	    source      {},
	    resolver    {executor_},
	    socket      {executor_, destination.protocol()}
	{}

	icmp_pinger(ExecutorType&                   executor,
		    const boost::asio::ip::address& address,
		    std::chrono::seconds            response_timeout,
		    std::uint8_t                    hop_limit)
	    :
	    pinger_interface {address, response_timeout, hop_limit},

	    executor_   {executor},
	    destination {address, 0},
	    source      {},
	    resolver    {executor_},
	    socket      {executor_, destination.protocol()}
	{}

	icmp_pinger(const icmp_pinger&) = delete;

	icmp_pinger& operator=(const icmp_pinger&) = delete;

	boost::asio::ip::address ping() override
	{
	    socket.set_option(boost::asio::ip::unicast::hops {hop_limit()});
	    
	    socket.send_to(boost::asio::buffer(proto::net::icmp_echo::make_echo_request()), destination);

	    boost::asio::steady_timer timer {executor_, response_timeout()};

	    timer.async_wait([&](auto&& error) {
		if (error)
		{
		    if (error != boost::asio::error::operation_aborted)
		    {
			throw boost::system::system_error {error, "steady_timer::async_wait"};
		    }
		}

		else
		{
		    socket.cancel();

		    source = boost::asio::ip::icmp::endpoint {};
		}
	    });

	    std::array<std::uint8_t, 32> unused_buffer;

	    socket.async_wait(boost::asio::ip::icmp::socket::wait_read, [&](auto&& error) {
		if (error)
		{
		    if (error != boost::asio::error::operation_aborted)
		    {
			throw boost::system::system_error {error, "socket::async_wait"};
		    }
		}

		else
		{
		    timer.cancel();

		    socket.receive_from(boost::asio::buffer(unused_buffer), source);
		}
	    });

	    executor_.run();
	    executor_.restart();

	    return source.address();
	}

	std::string resolve_last_address() override
	{
	    if (source.address().is_unspecified())
	    {
		return "";
	    }

	    auto results = resolver.resolve(source);

	    auto iterator = std::find_if(results.cbegin(), results.cend(), [&](auto&& result) {
		return source.address().to_string() != result.host_name();
	    });

	    return (iterator == results.cend() ? "" : iterator->host_name());
	}

    private:
	ExecutorType& executor_;

	boost::asio::ip::icmp::endpoint destination;
	boost::asio::ip::icmp::endpoint source;
	boost::asio::ip::icmp::resolver resolver;
	boost::asio::ip::icmp::socket   socket;
    };
}
