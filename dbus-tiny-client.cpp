#include <dbus-tiny.h>

#include <string>
#include <iostream>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

int main(int argc, const char **argv)
{
	try
	{
		boost::program_options::options_description	options("usage");
		boost::program_options::positional_options_description positional_options;

		try
		{
			bool introspect = false;
			std::string service;
			std::string interface;
			std::string rv1;
			std::string string_call_void;
			std::string string_call_string;
			std::string call_x_1;
			std::string call_x_2;
			std::string signal_string;
			std::vector<std::string> arguments;

			options.add_options()
				("service,s",				boost::program_options::value<std::string>(&service),					"service to use")
				("interface,i",				boost::program_options::value<std::string>(&interface),					"interface to use")
				("introspect,I",			boost::program_options::bool_switch(&introspect)->implicit_value(true),	"show introspection")
				("string-call-void,v",		boost::program_options::value<std::string>(&string_call_void),			"call method taking no arguments returning string")
				("string-call-string,c",	boost::program_options::value<std::string>(&string_call_string),		"call method taking string returning string")
				("call-x-1,1",				boost::program_options::value<std::string>(&call_x_1),					"call method taking u32,u32,string,string returning u64,u32,u32,string,double")
				("call-x-2,2",				boost::program_options::value<std::string>(&call_x_2),					"call method taking void returning u64,3xstring,4xdouble")
				("signal-string,S",			boost::program_options::value<std::string>(&signal_string),				"send signal with string parameter")
				("argument",				boost::program_options::value<std::vector<std::string>>(&arguments),	"specify method arguments");

			positional_options.add("argument", -1);

			boost::program_options::variables_map varmap;
			boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options).positional(positional_options).run(), varmap);
			boost::program_options::notify(varmap);

			DbusTinyClient dbus_client;

			if(service.length() == 0)
				service = "/org/freedesktop/DBus/dummy";

			if(introspect)
			{
				dbus_client.send_void(service, "org.freedesktop.DBus.Introspectable", "Introspect");
				rv1 = dbus_client.receive_string();
				std::cerr << rv1 << std::endl;
			}
			else if(string_call_void.length() > 0)
			{
				dbus_client.send_void(service, interface, string_call_void);
				rv1 = dbus_client.receive_string();
				std::cerr << rv1 << std::endl;
			}
			else if(string_call_string.length() > 0)
			{
				if(arguments.size() != 1)
					throw("call-string-string needs one argument");

				dbus_client.send_string(service, interface, string_call_string, arguments.at(0));
				rv1 = dbus_client.receive_string();
				std::cerr << rv1 << std::endl;
			}
			else if(call_x_1.length() > 0)
			{
				uint32_t p0, p1;
				uint64_t r0;
				uint32_t r1, r2;
				std::string r3;
				double r4;

				if(arguments.size() != 4)
					throw("call-x-1 needs 4 arguments");

				try
				{
					p0 = std::stoi(arguments.at(0));
					p1 = std::stoi(arguments.at(1));
				}
				catch(const std::invalid_argument &)
				{
					throw("invalid numeric argument");
				}
				catch(const std::out_of_range &)
				{
					throw("invalid numeric argument");
				}

				dbus_client.send_uint32_uint32_string_string(service, interface, call_x_1, p0, p1, arguments.at(2), arguments.at(3));
				dbus_client.receive_uint64_uint32_uint32_string_double(r0, r1, r2, r3, r4);

				std::cerr << boost::format("%llu / %lu / %lu / %s / %f\n") % r0 % r1 % r2 % r3 % r4;
			}
			else if(call_x_2.length() > 0)
			{
				uint64_t r0;
				std::string r1, r2, r3;
				double r4, r5, r6, r7;

				if(arguments.size() != 0)
					throw("call-x-2 needs no arguments");

				dbus_client.send_void(service, interface, call_x_2);
				dbus_client.receive_uint64_x3string_x4double(r0, r1, r2, r3, r4, r5, r6, r7);

				std::cerr << boost::format("%llu / %s / %s / %s / %f / %f / %f / %f\n") % r0 % r1 % r2 % r3 % r4 % r5 % r6 % r7;
			}
			else if(signal_string.length() > 0)
			{
				if(arguments.size() != 1)
					throw("signal-string needs one argument");

				dbus_client.signal_string(service, interface, signal_string, arguments.at(0));
			}
		}
		catch(const boost::program_options::error &e)
		{
			throw((boost::format("program option exception: %s\n%s") % e.what() % options).str());
		}
		catch(const DbusTinyException &e)
		{
			throw((boost::format("error: %s") % e.what()).str());
		}
		catch(const std::exception &e)
		{
			throw((boost::format("standard exception: %s") % e.what()).str());
		}
		catch(const std::string &e)
		{
			throw((boost::format("error: %s ") % e).str());
		}
		catch(const char *e)
		{
			throw((boost::format("error: %s") % e).str());
		}
		catch(...)
		{
			throw(std::string("unknown exception"));
		}
	}
	catch(const std::string &e)
	{
		std::cerr << "dbus-tiny-client: " << e << std::endl;
	}
}
