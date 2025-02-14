#pragma once

#include <chrono>
#include <string>

#include <cstdint>

#include <boost/asio.hpp>

namespace traceroute::core
{
    class pinger_interface {
    public:
	pinger_interface() noexcept :
	    pinger_interface {
		boost::asio::ip::address {},
		std::chrono::seconds     {1},
		std::numeric_limits<std::uint8_t>::max()
	    }
	{}

	pinger_interface(const boost::asio::ip::address& address) noexcept :
	    pinger_interface {address, std::chrono::seconds {1}, std::numeric_limits<std::uint8_t>::max()}
	{}

	pinger_interface(const boost::asio::ip::address& address,
			 std::chrono::seconds            response_timeout,
			 std::uint8_t                    hop_limit)
	    noexcept
	    :
	    destination_address_ {address},
	    response_timeout_    {response_timeout},
	    hop_limit_           {hop_limit}
	{}

	virtual ~pinger_interface() = default;

	boost::asio::ip::address destination_address() const
	{
	    return destination_address_;
	}

	void destination_address(const boost::asio::ip::address& address)
	{
	    destination_address_ = address;
	}

	std::uint8_t hop_limit() const noexcept
	{
	    return hop_limit_;
	}

	void hop_limit(std::uint8_t hop_limit) noexcept
	{
	    hop_limit_ = hop_limit;
	}

	virtual boost::asio::ip::address ping() = 0;

	virtual std::string resolve_last_address() = 0;

	std::chrono::seconds response_timeout() const noexcept
	{
	    return response_timeout_;
	}

	void response_timeout(std::chrono::seconds responce_timeout) noexcept
	{
	    response_timeout_ = responce_timeout;
	}

    private:
	boost::asio::ip::address destination_address_;
	std::chrono::seconds     response_timeout_;
	std::uint8_t             hop_limit_;
    };
}
