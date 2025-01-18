#include <dbus-tiny.h>

#include <stdint.h>
#include <stdbool.h>
#include <dbus/dbus.h>

#include <string>
#include <iostream>
#include <boost/format.hpp>

DbusTinyServer::DbusTinyServer(const std::string &bus)
{
	int rv;
	std::string error_message;

	dbus_error_init(&dbus_error);

	bus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);

	if(dbus_error_is_set(&dbus_error))
	{
		error_message = dbus_error.message;
		dbus_error_free(&dbus_error);
		throw(DbusTinyException(std::string("dbus bus get failed: ") + error_message));
	}

	if(!bus_connection)
		throw(DbusTinyException("dbus bus get failed (bus_connection = nullptr)"));

	rv = dbus_bus_request_name(bus_connection, bus.c_str(), DBUS_NAME_FLAG_DO_NOT_QUEUE, &dbus_error);

	if(dbus_error_is_set(&dbus_error))
	{
		error_message = dbus_error.message;
		dbus_error_free(&dbus_error);
		throw(DbusTinyException(std::string("dbus request name failed: ") + error_message));
	}

	if(rv != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
		throw(DbusTinyException("dbus request name: not primary owner: "));

	pending_message = nullptr;
	message_type = "";
	message_interface = "";
	message_method = "";
	message_string_reply_0 = "";
}

DbusTinyServer::~DbusTinyServer()
{
}

void DbusTinyServer::register_signal(const std::string &interface)
{
	std::string filter;
	std::string error_message;

	filter = (boost::format("type='%s',interface='%s'") % "signal" % interface).str();

	dbus_bus_add_match(bus_connection, filter.c_str(), &dbus_error);

	dbus_connection_flush(bus_connection);

	if(dbus_error_is_set(&dbus_error))
	{
		error_message = dbus_error.message;
		dbus_error_free(&dbus_error);
		throw(DbusTinyException(std::string("dbus_bus_add_match failed: ") + error_message));
	}
}

void DbusTinyServer::get_message(std::string &type, std::string &interface, std::string &method)
{
	if((pending_message = dbus_connection_pop_message(bus_connection)))
		goto done;

	dbus_connection_flush(bus_connection);

	if(!dbus_connection_read_write(bus_connection, -1))
		throw(DbusTinyException("dbus_connection_read_write failed"));

	if((pending_message = dbus_connection_pop_message(bus_connection)))
		goto done;

	while(dbus_connection_dispatch(bus_connection) != DBUS_DISPATCH_COMPLETE)
	{
		std::cerr << "* call dispatch\n";

		if((pending_message = dbus_connection_pop_message(bus_connection)))
		{
			std::cerr << "* pop succeeded @3\n";
			goto done;
		}

		std::cerr << "* repeating dispatch\n";
	}

	if(!(pending_message = dbus_connection_pop_message(bus_connection)))
	{
		std::cerr << "* pop succeeded @4\n";
		goto done;
	}

	throw(DbusTinyException("dispatch/pop failed"));

done:
	switch(dbus_message_get_type(pending_message))
	{
		case(DBUS_MESSAGE_TYPE_METHOD_CALL): type = "method call"; break;
		case(DBUS_MESSAGE_TYPE_METHOD_RETURN): type = "method return"; break;
		case(DBUS_MESSAGE_TYPE_ERROR): type = "error"; break;
		case(DBUS_MESSAGE_TYPE_SIGNAL): type = "signal"; break;
		default: type = "unknown"; break;
	}

	interface = dbus_message_get_interface(pending_message) ? : "";
	method = dbus_message_get_member(pending_message);
}

void DbusTinyServer::get_message_swig()
{
	get_message(message_type, message_interface, message_method);
}

const std::string &DbusTinyServer::receive_string()
{
	const char *s1;
	std::string error_message;

	dbus_message_get_args(pending_message, &dbus_error, DBUS_TYPE_STRING, &s1, DBUS_TYPE_INVALID);

	if(dbus_error_is_set(&dbus_error))
	{
		error_message = dbus_error.message;
		dbus_error_free(&dbus_error);
		throw(DbusTinyException(std::string("dbus_message_get_args failed: ") + error_message));
	}

	message_string_reply_0 = s1;

	return(message_string_reply_0);
}

void DbusTinyServer::receive_uint32_uint32_string_string(uint32_t &p1, uint32_t &p2, std::string &p3, std::string &p4)
{
	dbus_uint32_t s1, s2;
	const char *s3, *s4;
	std::string error_message;

	dbus_message_get_args(pending_message, &dbus_error, DBUS_TYPE_UINT32, &s1, DBUS_TYPE_UINT32, &s2, DBUS_TYPE_STRING, &s3, DBUS_TYPE_STRING, &s4, DBUS_TYPE_INVALID);

	if(dbus_error_is_set(&dbus_error))
	{
		error_message = dbus_error.message;
		dbus_error_free(&dbus_error);
		throw(DbusTinyException(std::string("dbus_message_get_args failed: ") + error_message));
	}

	p1 = s1;
	p2 = s2;
	p3 = s3;
	p4 = s4;
}

void DbusTinyServer::receive_uint32_uint32_string_string_swig()
{
	receive_uint32_uint32_string_string(rv_uint32_0, rv_uint32_1, rv_string_0, rv_string_1);
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

void DbusTinyServer::send_uint64_x3string_x4double(uint64_t p0, const std::string &p1, const std::string &p2, const std::string &p3, double p4, double p5, double p6, double p7)
{
	DBusMessage *reply_message;
	const char *p1cs, *p2cs, *p3cs;

	p1cs = p1.c_str();
	p2cs = p2.c_str();
	p3cs = p3.c_str();

	if(!(reply_message = dbus_message_new_method_return(pending_message)))
		throw(DbusTinyException("dbus_message_new_method_return failed"));

	if(!dbus_message_append_args(reply_message,
				DBUS_TYPE_UINT64, &p0, DBUS_TYPE_STRING, &p1cs, DBUS_TYPE_STRING, &p2cs, DBUS_TYPE_STRING, &p3cs,
				DBUS_TYPE_DOUBLE, &p4, DBUS_TYPE_DOUBLE, &p5, DBUS_TYPE_DOUBLE, &p6, DBUS_TYPE_DOUBLE, &p7, DBUS_TYPE_INVALID))
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

const std::string &DbusTinyServer::inform_error(const std::string &reason)
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

const std::string &DbusTinyServer::get_message_type()
{
	return(message_type);
}

const std::string &DbusTinyServer::get_message_interface()
{
	return(message_interface);
}

const std::string &DbusTinyServer::get_message_method()
{
	return(message_method);
}

uint32_t DbusTinyServer::get_rv_uint32_0()
{
	return(rv_uint32_0);
}

uint32_t DbusTinyServer::get_rv_uint32_1()
{
	return(rv_uint32_1);
}

const std::string &DbusTinyServer::get_rv_string_0()
{
	return(rv_string_0);
}

const std::string &DbusTinyServer::get_rv_string_1()
{
	return(rv_string_1);
}
