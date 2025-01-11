#include <dbus-tiny.h>

#include <dbus/dbus.h>

#include <string>
#include <iostream>
#include <boost/format.hpp>

DbusTinyClient::DbusTinyClient()
{
	pending_call = nullptr;

	dbus_error_init(&dbus_error);

	bus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);

	if(dbus_error_is_set(&dbus_error))
		throw(DbusTinyException(boost::format("dbus bus get failed: %s") % dbus_error.message));

	if(!bus_connection)
		throw(DbusTinyException("dbus bus get failed (bus_connection = nullptr)"));

	rv_string_0 = "";
	rv_uint64_0 = 0;
	rv_uint32_0 = 0;
	rv_uint32_1 = 0;
	rv_double_0 = 0;
	signal_serial = 0;
}

void DbusTinyClient::send_void(const std::string &service, const std::string &interface, const std::string &method)
{
	DBusMessage *request_message;

	try
	{
		request_message = nullptr;
		pending_call = nullptr;

		if(!(request_message = dbus_message_new_method_call(service.c_str(), "/", (interface == "") ? nullptr : interface.c_str(), method.c_str())))
			throw(DbusTinyInternalException("error in dbus_message_new_method_call"));

		if(!dbus_connection_send_with_reply(bus_connection, request_message, &pending_call, -1))
			throw(DbusTinyInternalException("error in dbus_connection_send_with_reply"));

		if(!pending_call)
			throw(DbusTinyInternalException("nullptr in dbus_connection_send_with_reply"));
	}
	catch(const DbusTinyInternalException &e)
	{
		std::string rv = std::string("send_void: ") + e.what();

		if(dbus_error_is_set(&dbus_error))
		{
			dbus_error_free(&dbus_error);
			rv += std::string(" (dbus error: ") + dbus_error.message + ")";
		}

		if(pending_call)
		{
			dbus_pending_call_unref(pending_call);
			pending_call = nullptr;
		}

		if(request_message)
			dbus_message_unref(request_message);

		throw(DbusTinyException(rv));
	}

	dbus_connection_flush(bus_connection);
	dbus_message_unref(request_message);
}

void DbusTinyClient::send_string(const std::string &service, const std::string &interface, const std::string &method, const std::string &parameter)
{
	DBusMessage *request_message;

	try
	{
		const char *cstr;

		request_message = nullptr;
		pending_call = nullptr;

		if(!(request_message = dbus_message_new_method_call(service.c_str(), "/", (interface == "") ? nullptr : interface.c_str(), method.c_str())))
			throw(DbusTinyInternalException("error in dbus_message_new_method_call"));

		cstr = parameter.c_str();

		if(!dbus_message_append_args(request_message, DBUS_TYPE_STRING, &cstr, DBUS_TYPE_INVALID))
			throw(DbusTinyInternalException("error in dbus_message_append_args"));

		if(!dbus_connection_send_with_reply(bus_connection, request_message, &pending_call, -1))
			throw(DbusTinyInternalException("error in dbus_connection_send_with_reply"));

		if(!pending_call)
			throw(DbusTinyInternalException("pending connection is nullptr in dbus_connection_send_with_reply"));
	}
	catch(const DbusTinyInternalException &e)
	{
		std::string rv = std::string("send_string: ") + e.what();

		if(dbus_error_is_set(&dbus_error))
		{
			dbus_error_free(&dbus_error);
			rv += std::string(" (dbus error: ") + dbus_error.message + ")";
		}

		if(pending_call)
		{
			dbus_pending_call_unref(pending_call);
			pending_call = nullptr;
		}

		if(request_message)
			dbus_message_unref(request_message);

		throw(DbusTinyException(rv));
	}

	dbus_connection_flush(bus_connection);
	dbus_message_unref(request_message);
}

void DbusTinyClient::send_uint32_uint32_string_string(const std::string &service, const std::string &interface, const std::string &method,
		uint32_t p0u32, uint32_t p1u32, const std::string &p2s, const std::string &p3s)
{
	DBusMessage *request_message;

	try
	{
		const char *p2cs, *p3cs;

		request_message = nullptr;
		pending_call = nullptr;

		if(!(request_message = dbus_message_new_method_call(service.c_str(), "/", (interface == "") ? nullptr : interface.c_str(), method.c_str())))
			throw(DbusTinyInternalException("error in dbus_message_new_method_call"));

		p2cs = p2s.c_str();
		p3cs = p3s.c_str();

		if(!dbus_message_append_args(request_message, DBUS_TYPE_UINT32, &p0u32, DBUS_TYPE_UINT32, &p1u32, DBUS_TYPE_STRING, &p2cs, DBUS_TYPE_STRING, &p3cs, DBUS_TYPE_INVALID))
			throw(DbusTinyInternalException("error in dbus_message_append_args"));

		if(!dbus_connection_send_with_reply(bus_connection, request_message, &pending_call, -1))
			throw(DbusTinyInternalException("error in dbus_connection_send_with_reply"));

		if(!pending_call)
			throw(DbusTinyInternalException("pending connection is nullptr in dbus_connection_send_with_reply"));
	}
	catch(const DbusTinyInternalException &e)
	{
		std::string rv = std::string("send_uint32_uint32_string_string: ") + e.what();

		if(dbus_error_is_set(&dbus_error))
		{
			dbus_error_free(&dbus_error);
			rv += std::string(" (dbus error: ") + dbus_error.message + ")";
		}

		if(pending_call)
		{
			dbus_pending_call_unref(pending_call);
			pending_call = nullptr;
		}

		if(request_message)
			dbus_message_unref(request_message);

		throw(DbusTinyException(rv));
	}

	dbus_connection_flush(bus_connection);
	dbus_message_unref(request_message);
}

const std::string& DbusTinyClient::receive_string()
{
	DBusMessage *reply_message;

	try
	{
		const char *cstr;

		reply_message = nullptr;

		if(!pending_call)
			throw(DbusTinyInternalException("no call pending"));

		dbus_pending_call_block(pending_call);

		if(!(reply_message = dbus_pending_call_steal_reply(pending_call)))
			throw(DbusTinyInternalException("nullptr in dbus_pending_call_steal_reply"));

		dbus_pending_call_unref(pending_call);
		pending_call = nullptr;

		if(dbus_message_get_type(reply_message) == DBUS_MESSAGE_TYPE_ERROR)
		{
			dbus_message_get_args(reply_message, &dbus_error, DBUS_TYPE_STRING, &cstr, DBUS_TYPE_INVALID);

			if(dbus_error_is_set(&dbus_error))
				throw(DbusTinyInternalException("error in dbus_message_get_args while processing error"));

			throw(DbusTinyInternalException(boost::format("error while receiving reply: %s") % cstr));
		}

		dbus_message_get_args(reply_message, &dbus_error, DBUS_TYPE_STRING, &cstr, DBUS_TYPE_INVALID);

		if(dbus_error_is_set(&dbus_error))
			throw(DbusTinyInternalException("error in dbus_message_get_args"));

		dbus_message_unref(reply_message);

		rv_string_0 = cstr;

		return(rv_string_0);
	}
	catch(const DbusTinyInternalException &e)
	{
		std::string e1 = std::string("receive_string: ") + e.what();

		if(dbus_error_is_set(&dbus_error))
		{
			dbus_error_free(&dbus_error);
			e1 += std::string(" (dbus error: ") + dbus_error.message + ")";
		}

		if(reply_message)
			dbus_message_unref(reply_message);

		if(pending_call)
		{
			dbus_pending_call_unref(pending_call);
			pending_call = nullptr;
		}

		throw(DbusTinyException(e1));
	}
}

void DbusTinyClient::receive_uint64_uint32_uint32_string_double(uint64_t &p1u64, uint32_t &p2u32, uint32_t &p3u32, std::string &p4s, double &p5d)
{
	DBusMessage *reply_message = nullptr;

	try
	{
		const char *p4cs;

		if(!pending_call)
			throw(DbusTinyInternalException("no call pending"));

		dbus_pending_call_block(pending_call);

		if(!(reply_message = dbus_pending_call_steal_reply(pending_call)))
			throw(DbusTinyInternalException("nullptr in dbus_pending_call_steal_reply"));

		dbus_pending_call_unref(pending_call);
		pending_call = nullptr;

		if(dbus_message_get_type(reply_message) == DBUS_MESSAGE_TYPE_ERROR)
		{
			dbus_message_get_args(reply_message, &dbus_error, DBUS_TYPE_STRING, &p4cs, DBUS_TYPE_INVALID);

			if(dbus_error_is_set(&dbus_error))
				throw(DbusTinyInternalException("error in dbus_message_get_args while processing error"));

			throw(DbusTinyInternalException(boost::format("error while receiving reply: %s") % p4cs));
		}

		dbus_message_get_args(reply_message, &dbus_error,
				DBUS_TYPE_UINT64, &p1u64, DBUS_TYPE_UINT32, &p2u32, DBUS_TYPE_UINT32, &p3u32, DBUS_TYPE_STRING, &p4cs, DBUS_TYPE_DOUBLE, &p5d, DBUS_TYPE_INVALID);

		if(dbus_error_is_set(&dbus_error))
			throw(DbusTinyInternalException("error in dbus_message_get_args"));

		dbus_message_unref(reply_message);

		p4s = p4cs;
	}
	catch(const DbusTinyInternalException &e)
	{
		std::string e1 = std::string("receive_uint64_uint32_uint32_string_double: ") + e.what();

		if(dbus_error_is_set(&dbus_error))
		{
			dbus_error_free(&dbus_error);
			e1 += std::string(" (dbus error: ") + dbus_error.message + ")";
		}

		if(reply_message)
			dbus_message_unref(reply_message);

		if(pending_call)
		{
			dbus_pending_call_unref(pending_call);
			pending_call = nullptr;
		}

		throw(DbusTinyException(e1));
	}
}

void DbusTinyClient::receive_uint64_uint32_uint32_string_double_swig()
{
	receive_uint64_uint32_uint32_string_double(rv_uint64_0, rv_uint32_0, rv_uint32_1, rv_string_0, rv_double_0);
}

void DbusTinyClient::signal_string(const std::string &service, const std::string &interface, const std::string &signal, const std::string &parameter)
{
	DBusMessage *signal_message;
	dbus_uint32_t serial;

	try
	{
		const char *cstr;

		signal_message = nullptr;

		if(!(signal_message = dbus_message_new_signal(service.c_str(), interface.c_str(), signal.c_str())))
			throw(DbusTinyInternalException("error in dbus_message_new_signal"));

		cstr = parameter.c_str();

		if(!dbus_message_append_args(signal_message, DBUS_TYPE_STRING, &cstr, DBUS_TYPE_INVALID))
			throw(DbusTinyInternalException("error in dbus_message_append_args"));

		serial = signal_serial++;

		if(!dbus_connection_send(bus_connection, signal_message, &serial))
			throw(DbusTinyInternalException("error in dbus_connection_send"));

	}
	catch(const DbusTinyInternalException &e)
	{
		std::string rv = std::string("signal_string: ") + e.what();

		if(dbus_error_is_set(&dbus_error))
		{
			dbus_error_free(&dbus_error);
			rv += std::string(" (dbus error: ") + dbus_error.message + ")";
		}

		if(signal_message)
			dbus_message_unref(signal_message);

		throw(DbusTinyException(rv));
	}

	dbus_connection_flush(bus_connection);
	dbus_message_unref(signal_message);
}

const std::string &DbusTinyClient::get_rv_string_0()
{
	return(rv_string_0);
}

uint64_t DbusTinyClient::get_rv_uint64_0()
{
	return(rv_uint64_0);
}

uint32_t DbusTinyClient::get_rv_uint32_0()
{
	return(rv_uint32_0);
}

uint32_t DbusTinyClient::get_rv_uint32_1()
{
	return(rv_uint32_1);
}

double DbusTinyClient::get_rv_double_0()
{
	return(rv_double_0);
}
