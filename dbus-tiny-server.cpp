#include <dbus-tiny.h>

#include <stdint.h>
#include <stdbool.h>
#include <dbus/dbus.h>

#include <string>
#include <iostream>
#include <boost/format.hpp>

DbusTinyServer::DbusTinyServer()
{
	std::cerr << "server constructor\n";

	dbus_error_init(&dbus_error);

	bus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);

	if(dbus_error_is_set(&dbus_error))
		throw(DbusTinyException(std::string("dbus bus get failed: ") + dbus_error.message));

	if(!bus_connection)
		throw(DbusTinyException("dbus bus get failed (bus_connection = nullptr)"));

	pending_message = nullptr;
}

DbusTinyServer::DbusTinyServer(const std::string &bus) : DbusTinyServer()
{
	std::cerr << "server constructor with bus " << bus << std::endl;
#if 0
	dbus_error_init(&dbus_error);

	bus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);

	if(dbus_error_is_set(&dbus_error))
		throw(DbusTinyException(std::string("dbus bus get failed: ") + dbus_error.message));

	if(!bus_connection)
		throw(DbusTinyException("dbus bus get failed (bus_connection = nullptr)"));
#endif
	int rv;

	rv = dbus_bus_request_name(bus_connection, bus.c_str(), DBUS_NAME_FLAG_DO_NOT_QUEUE, &dbus_error);

	if(dbus_error_is_set(&dbus_error))
		throw(DbusTinyException(std::string("dbus request name failed: ") + dbus_error.message));

	if(rv != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
		throw(DbusTinyException("dbus request name: not primary owner: "));
}

void DbusTinyServer::add_signal_filter(const std::string &interface)
{
	dbus_bus_add_match(bus_connection, (boost::format("type='%s',interface='%s'") % "signal" % interface).str().c_str(), &dbus_error);

	dbus_connection_flush(bus_connection);

	if(dbus_error_is_set(&dbus_error))
		throw(DbusTinyException(std::string("dbus_bus_add_match failed: ") + dbus_error.message));
}

bool DbusTinyServer::get_message(int *type, std::string *interface, std::string *method)
{
	if(!dbus_connection_read_write_dispatch(bus_connection, -1))
		return(false);

	if(!(pending_message = dbus_connection_pop_message(bus_connection)))
		throw(DbusTinyException("dbus_connection_pop_message failed"));

	if(type)
		*type = dbus_message_get_type(pending_message);

	if(interface)
		*interface = dbus_message_get_interface(pending_message);

	if(method)
		*method = dbus_message_get_member(pending_message);

	return(true);
}

void DbusTinyServer::receive_string(std::string &p1)
{
	const char *s1;

	dbus_message_get_args(pending_message, &dbus_error, DBUS_TYPE_STRING, &s1, DBUS_TYPE_INVALID);

	if(dbus_error_is_set(&dbus_error))
	{
		dbus_error_free(&dbus_error);
		throw(DbusTinyException(std::string("dbus_message_get_args failed: ") + dbus_error.message));
	}

	p1 = s1;
}

void DbusTinyServer::receive_uint32_uint32_string_string(uint32_t &p1, uint32_t &p2, std::string &p3, std::string &p4)
{
	dbus_uint32_t s1, s2;
	const char *s3, *s4;

	dbus_message_get_args(pending_message, &dbus_error, DBUS_TYPE_UINT32, &s1, DBUS_TYPE_UINT32, &s2, DBUS_TYPE_STRING, &s3, DBUS_TYPE_STRING, &s4, DBUS_TYPE_INVALID);

	if(dbus_error_is_set(&dbus_error))
	{
		dbus_error_free(&dbus_error);
		throw(DbusTinyException(std::string("dbus_message_get_args failed: ") + dbus_error.message));
	}

	p1 = s1;
	p2 = s2;
	p3 = s3;
	p4 = s4;
}

void DbusTinyServer::send_string(const std::string &reply_string)
{
	DBusMessage *reply_message;
	const char *reply_cstr;

	reply_cstr = reply_string.c_str();

	if(!(reply_message = dbus_message_new_method_return(pending_message)))
		throw(DbusTinyException("dbus_message_new_method_return failed"));

	if(!dbus_message_append_args(reply_message, DBUS_TYPE_STRING, &reply_cstr, DBUS_TYPE_INVALID))
	{
		dbus_message_unref(reply_message);
		throw(DbusTinyException("dbus_message_append_args failed"));
	}

	if(!dbus_connection_send(bus_connection, reply_message, NULL))
	{
		dbus_message_unref(reply_message);
		throw(DbusTinyException("dbus_connection_send failed"));
	}

	dbus_message_unref(reply_message);
}

void DbusTinyServer::send_uint64_uint32_uint32_string_double(uint64_t p1, uint32_t p2, uint32_t p3, const std::string &p4, double p5)
{
	DBusMessage *reply_message;
	const char *cstr;

	cstr = p4.c_str();

	if(!(reply_message = dbus_message_new_method_return(pending_message)))
		throw(DbusTinyException("dbus_message_new_method_return failed"));

	if(!dbus_message_append_args(reply_message, DBUS_TYPE_UINT64, &p1, DBUS_TYPE_UINT32, &p2, DBUS_TYPE_UINT32, &p3, DBUS_TYPE_STRING, &cstr, DBUS_TYPE_DOUBLE, &p5, DBUS_TYPE_INVALID))
	{
		dbus_message_unref(reply_message);
		throw(DbusTinyException("dbus_message_append_args failed"));
	}

	if(!dbus_connection_send(bus_connection, reply_message, NULL))
	{
		dbus_message_unref(reply_message);
		throw(DbusTinyException("dbus_connection_send failed"));
	}

	dbus_message_unref(reply_message);
}

std::string DbusTinyServer::inform_error(const std::string &reason)
{
	DBusMessage *error_message;

	if(!(error_message = dbus_message_new_error(pending_message, DBUS_ERROR_FAILED, reason.c_str())))
		throw(DbusTinyException("method error - error in dbus_message_new_error"));

	if(!dbus_connection_send(bus_connection, error_message, NULL))
		throw(DbusTinyException("method error - error in dbus_connection_send"));

	dbus_message_unref(error_message);

	return(reason);
}

void DbusTinyServer::reset()
{
	if(bus_connection)
		dbus_connection_flush(bus_connection);

	if(pending_message)
	{
		dbus_message_unref(pending_message);
		pending_message = nullptr;
	}
}
