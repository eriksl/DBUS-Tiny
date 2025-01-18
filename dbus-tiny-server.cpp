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
			std::string service;
			std::string method_interface;
			std::vector<std::string> signal_interface;
			std::string string_call_void;
			std::string string_call_string;
			std::string call_x_1;
			std::string call_x_2;
			std::string message_type;
			std::string message_interface;
			std::string message_method;

			options.add_options()
				("service,s",				boost::program_options::value<std::string>(&service)->required(),				"service to register")
				("method-interface,i",		boost::program_options::value<std::string>(&method_interface)->required(),		"interface to use for registering methods")
				("signal-interface,I",		boost::program_options::value<std::vector<std::string>>(&signal_interface),		"interfaces to use for registering signal")
				("string-call-void,v",		boost::program_options::value<std::string>(&string_call_void)->required(),		"name to use for registering string-call-void method")
				("string-call-string,c",	boost::program_options::value<std::string>(&string_call_string)->required(),	"name to use for registering string-call-string method")
				("call-x-1,1",				boost::program_options::value<std::string>(&call_x_1)->required(),
																"name to use for registering the method taking u32,u32,string,string and returning u64,u32,u32,string,double")
				("call-x-2,2",				boost::program_options::value<std::string>(&call_x_2)->required(), "name to use for registering the method u64,3xstring,4xdouble");

			boost::program_options::variables_map varmap;
			boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options).run(), varmap);
			boost::program_options::notify(varmap);

			DbusTinyServer dbus_server(service);

			for(auto &signal: signal_interface)
				dbus_server.register_signal(signal);

			for(;;)
			{
				dbus_server.get_message(message_type, message_interface, message_method);

				std::cout << "message received, type: " << message_type << ", interface: " << message_interface << ", method: " << message_method << std::endl;

				if(message_type == "method call")
				{
					if((message_interface == "org.freedesktop.DBus.Introspectable") && (message_method == "Introspect"))
					{
						std::string reply = std::string() +
								"<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n" +
								"<node>\n" +
								"	<interface name=\"org.freedesktop.DBus.Introspectable\">\n" +
								"		<method name=\"Introspect\">\n" +
								"			<arg name=\"xml\" type=\"s\" direction=\"out\"/>\n" +
								"		</method>\n" +
								"	</interface>\n" +
								"	<interface name=\"" + method_interface + "\">\n" +
								"		<method name=\"" + string_call_void + "\">\n" +
								"			<arg name=\"result\" type=\"s\" direction=\"out\"/>\n" +
								"		</method>\n" +
								"		<method name=\"" + string_call_string + "\">\n" +
								"			<arg name=\"argument\" type=\"s\" direction=\"in\"/>\n" +
								"			<arg name=\"result\" type=\"s\" direction=\"out\"/>\n" +
								"		</method>\n" +
								"		<method name=\"" + call_x_1 + "\">\n" +
								"			<arg name=\"argument_1\" type=\"u\" direction=\"in\"/>\n" +
								"			<arg name=\"argument_2\" type=\"u\" direction=\"in\"/>\n" +
								"			<arg name=\"argument_3\" type=\"s\" direction=\"in\"/>\n" +
								"			<arg name=\"argument_4\" type=\"s\" direction=\"in\"/>\n" +
								"			<arg name=\"reply_1\" type=\"t\" direction=\"out\"/>\n" +
								"			<arg name=\"reply_2\" type=\"u\" direction=\"out\"/>\n" +
								"			<arg name=\"reply_3\" type=\"u\" direction=\"out\"/>\n" +
								"			<arg name=\"reply_4\" type=\"s\" direction=\"out\"/>\n" +
								"			<arg name=\"reply_5\" type=\"d\" direction=\"out\"/>\n" +
								"		</method>\n" +
								"		<method name=\"" + call_x_2 + "\">\n" +
								"			<arg name=\"reply_1\" type=\"t\" direction=\"out\"/>\n" +
								"			<arg name=\"reply_2\" type=\"u\" direction=\"out\"/>\n" +
								"			<arg name=\"reply_3\" type=\"u\" direction=\"out\"/>\n" +
								"			<arg name=\"reply_4\" type=\"s\" direction=\"out\"/>\n" +
								"			<arg name=\"reply_5\" type=\"d\" direction=\"out\"/>\n" +
								"			<arg name=\"reply_6\" type=\"d\" direction=\"out\"/>\n" +
								"			<arg name=\"reply_7\" type=\"d\" direction=\"out\"/>\n" +
								"			<arg name=\"reply_8\" type=\"d\" direction=\"out\"/>\n" +
								"		</method>\n" +
								"	</interface>\n";

						for(const auto &signal : signal_interface)
						{
							reply += std::string() +
								"	<interface name=\"" + signal + "\">\n" +
								"		<signal name=\"" + string_call_string + "\">\n" +
								"			<arg name=\"argument\" type=\"s\"/>\n" +
								"		</signal>\n" +
								"	</interface>\n";
						}

						reply += "</node>\n";

						dbus_server.send_string(reply);
					}
					else if((message_interface == method_interface) || (message_interface == ""))
					{
						if(message_method == string_call_void)
						{
							std::cout << "string call void method " << string_call_void << " method called\n";
							dbus_server.send_string("string-call-void OK");
						}
						else if(message_method == string_call_string)
						{
							std::cout << "string call void method " << string_call_void << " method called with parameters: " << dbus_server.receive_string() << std::endl;
							dbus_server.send_string("string-call-string OK");
						}
						else if(message_method == call_x_1)
						{
							uint32_t p0, p1;
							std::string p2, p3;

							dbus_server.receive_uint32_uint32_string_string(p0, p1, p2, p3);

							std::cout << "x_1 method " << call_x_1 << " method called with parameters: " << p0 << " / " << p1 << " / " << p2 << " / " << p3 << std::endl;

							dbus_server.send_uint64_uint32_uint32_string_double(time((time_t *)0), 0, 1, "call_x_1 OK", 123.456);
						}
						else if(message_method == call_x_2)
						{
							std::cout << "x_2 method " << call_x_2 << " method called\n";

							dbus_server.send_uint64_x3string_x4double(time((time_t *)0), "string 1", "string 2", "string 3", 0.0, 1.1, 2.2, 3.3);
						}
						else
							dbus_server.inform_error("unknown method");
					}
					else
						dbus_server.inform_error("unknown interface");
				}
				else if(message_type == "method reply")
				{
					std::cerr << "unexpected message type, skip\n";
					goto next;
				}
				else if(message_type == "error")
				{
					std::cerr << "unexpected message type, skip\n";
					goto next;
				}
				else if(message_type == "signal")
				{
					std::cout << "signal received, method: " << message_method << ", " << "parameter: " << dbus_server.receive_string() << std::endl;
				}
				else
				{
					std::cout << "unknown message type, skip\n";
					goto next;
				}

next:
				dbus_server.reset();
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
		std::cerr << "server: " << e << std::endl;
	}
}
