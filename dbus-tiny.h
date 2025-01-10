#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <dbus/dbus.h>

#include <exception>
#include <string>
#include <boost/format.hpp>

class DbusTinyException : public std::exception
{
	public:

		DbusTinyException() = delete;
		DbusTinyException(const std::string &what);
		DbusTinyException(const char *what);
		DbusTinyException(const boost::format &what);

		const char *what() const noexcept;

	private:

		const std::string what_string;
};

class DbusTinyInternalException : public std::exception
{
	public:

		DbusTinyInternalException() = delete;
		DbusTinyInternalException(const std::string &what);
		DbusTinyInternalException(const char *what);
		DbusTinyInternalException(const boost::format &what);

		const char *what() const noexcept;

	private:

		const std::string what_string;
};

class DbusTinyServer
{
	public:

		DbusTinyServer(const DbusTinyServer &) = delete;

		DbusTinyServer();
		DbusTinyServer(const std::string &bus);

		void add_signal_filter(const std::string &interface);
		bool get_message(int *type = nullptr, std::string *interface = nullptr, std::string *method = nullptr);
		void receive_string(std::string &string);
		void receive_uint32_uint32_string_string(uint32_t &, uint32_t &, std::string &, std::string &);
		void send_string(const std::string &reply_string);
		void send_uint64_uint32_uint32_string_double(uint64_t, uint32_t, uint32_t, const std::string &, double);
		std::string inform_error(const std::string &reason);
		void reset();

	private:

		DBusError dbus_error;
		DBusConnection *bus_connection;
		DBusMessage *pending_message;
};

class DbusTinyClient
{
	public:

		DbusTinyClient(const DbusTinyClient &) = delete;

		DbusTinyClient();

		void send_void(const std::string &service, const std::string &interface, const std::string &method);
		void send_string(const std::string &service, const std::string &interface, const std::string &method, const std::string &parameter);
		void send_uint32_uint32_string_string(const std::string &service, const std::string &interface, const std::string &method,
				uint32_t, uint32_t, const std::string &, const std::string &);
		const std::string &receive_string();
		void receive_uint64_uint32_uint32_string_double(uint64_t &, uint32_t &, uint32_t &, std::string, double &);
		void receive_uint64_uint32_uint32_string_double_swig();
		void signal_string(const std::string &service, const std::string &interface, const std::string &signal, const std::string &parameter);

		const std::string &get_rv_string_0();
		uint64_t get_rv_uint64_0();
		uint32_t get_rv_uint32_0();
		uint32_t get_rv_uint32_1();
		double get_rv_double_0();

	private:

		DBusError dbus_error;
		DBusConnection *bus_connection;
		DBusPendingCall *pending_call;
		unsigned int signal_serial;

		std::string rv_string_0;
		uint64_t rv_uint64_0;
		uint32_t rv_uint32_0;
		uint32_t rv_uint32_1;
		double rv_double_0;
};
