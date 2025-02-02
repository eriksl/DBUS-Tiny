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

		DbusTinyServer() = delete;
		DbusTinyServer(const DbusTinyServer &) = delete;

		DbusTinyServer(const std::string &bus);
		~DbusTinyServer();

		void register_signal(const std::string &interface);
		void get_message(std::string &type, std::string &interface, std::string &method);
		void get_message_swig();
		const std::string &receive_string();
		void receive_uint32_uint32_string_string(uint32_t &, uint32_t &, std::string &, std::string &);
		void receive_uint32_uint32_string_string_swig();
		void receive_x3string(std::string &, std::string &, std::string &);
		void receive_x3string_swig();
		void send_string(const std::string &reply_string);
		void send_uint64_uint32_uint32_string_double(uint64_t, uint32_t, uint32_t, const std::string &, double);
		void send_uint64_x3string_x4double(uint64_t, const std::string &, const std::string &, const std::string &, double, double, double, double);
		void send_uint32_x3uint64(uint32_t, uint64_t, uint64_t, uint64_t);
		const std::string &inform_error(const std::string &reason);
		void reset();

		const std::string &get_message_type();
		const std::string &get_message_interface();
		const std::string &get_message_method();

		uint32_t get_rv_uint32_0();
		uint32_t get_rv_uint32_1();
		const std::string &get_rv_string_0();
		const std::string &get_rv_string_1();
		const std::string &get_rv_string_2();

	private:

		DBusError dbus_error;
		DBusConnection *bus_connection;
		DBusMessage *pending_message;

		std::string message_type;
		std::string message_interface;
		std::string message_method;
		std::string message_string_reply_0;
		std::string message_string_reply_1;
		std::string message_string_reply_2;

		uint32_t rv_uint32_0;
		uint32_t rv_uint32_1;
		std::string rv_string_0;
		std::string rv_string_1;
		std::string rv_string_2;
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
		void send_x3string(const std::string &service, const std::string &interface, const std::string &method,
				const std::string &, const std::string &, const std::string &);
		const std::string &receive_string();
		void receive_uint64_uint32_uint32_string_double(uint64_t &, uint32_t &, uint32_t &, std::string &, double &);
		void receive_uint64_uint32_uint32_string_double_swig();
		void receive_uint64_x3string_x4double(uint64_t &, std::string &, std::string &, std::string &, double &, double &, double &, double &);
		void receive_uint64_x3string_x4double_swig();
		void receive_uint32_x3uint64(uint32_t &, uint64_t &, uint64_t &, uint64_t &);
		void receive_uint32_x3uint64_swig();
		void signal_string(const std::string &service, const std::string &interface, const std::string &signal, const std::string &parameter);

		const std::string &get_rv_string_0();
		const std::string &get_rv_string_1();
		const std::string &get_rv_string_2();
		uint64_t get_rv_uint64_0();
		uint64_t get_rv_uint64_1();
		uint64_t get_rv_uint64_2();
		uint32_t get_rv_uint32_0();
		uint32_t get_rv_uint32_1();
		double get_rv_double_0();
		double get_rv_double_1();
		double get_rv_double_2();
		double get_rv_double_3();

	private:

		bool path_valid(const std::string &path);
		bool domain_valid(const std::string &domain);

		DBusError dbus_error;
		DBusConnection *bus_connection;
		DBusPendingCall *pending_call;
		unsigned int signal_serial;

		std::string rv_string_0;
		std::string rv_string_1;
		std::string rv_string_2;
		uint64_t rv_uint64_0;
		uint64_t rv_uint64_1;
		uint64_t rv_uint64_2;
		uint32_t rv_uint32_0;
		uint32_t rv_uint32_1;
		double rv_double_0;
		double rv_double_1;
		double rv_double_2;
		double rv_double_3;
};
