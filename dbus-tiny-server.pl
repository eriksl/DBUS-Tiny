#!/usr/bin/perl -w

use strict;
use warnings;
use Getopt::Long qw(:config gnu_compat noignore_case);
use Try::Tiny;
use Scalar::Util;
use DBUS::Tiny;

use Data::Dumper;

my($service);
my($method_interface, @signal_interface);
my($message_type, $message_interface, $message_method);
my($signal);

GetOptions(
		"service|s=s"				=> \$service,
		"method-interface|i=s"		=> \$method_interface,
		"signal-interface|I=s"		=> \@signal_interface,
);

if(!defined($service))
{
	my($usage);

	$usage .= "usage: dbus-tiny-server.pl\n";
	$usage .= "       -s|--service <service>\n";
	$usage .= "       -i|--method-interface <interface>\n";
	$usage .= "       -I|--signal-interface <interface> (optional, may be repeated)\n";

	die($usage);
}

$method_interface = "" if(!defined($method_interface));

my($dbus_server) = new DBUS::Tiny::DbusTinyServer($service);

foreach $signal (@signal_interface)
{
	$dbus_server->register_signal($signal);
}

for(;;)
{
	try
	{
		$dbus_server->get_message();
		$message_type = $dbus_server->get_message_type();
		$message_interface = $dbus_server->get_message_interface();
		$message_method = $dbus_server->get_message_method();

		printf STDERR ("message received, type: %s, interface: %s, method: %s\n", $message_type, $message_interface, $message_method);

		if($message_type eq "method call")
		{
			if(($message_interface eq "org.freedesktop.DBus.Introspectable") && ($message_method eq "Introspect"))
			{
				my($reply) =
						"<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n" .
						"<node>\n" .
						"	<interface name=\"org.freedesktop.DBus.Introspectable\">\n" .
						"		<method name=\"Introspect\">\n" .
						"			<arg name=\"xml\" type=\"s\" direction=\"out\"/>\n" .
						"		</method>\n" .
						"	</interface>\n" .
						"	<interface name=\"" . $method_interface . "\">\n" .
						"		<method name=\"string_call_void\">\n" .
						"			<arg name=\"reply\" type=\"s\" direction=\"out\"/>\n" .
						"		</method>\n" .
						"		<method name=\"string_call_string\">\n" .
						"			<arg name=\"argument\" type=\"s\" direction=\"in\"/>\n" .
						"			<arg name=\"reply\" type=\"s\" direction=\"out\"/>\n" .
						"		</method>\n" .
						"		<method name=\"call_x_1\">\n" .
						"			<arg name=\"argument_1\" type=\"u\" direction=\"in\"/>\n" .
						"			<arg name=\"argument_2\" type=\"u\" direction=\"in\"/>\n" .
						"			<arg name=\"argument_3\" type=\"s\" direction=\"in\"/>\n" .
						"			<arg name=\"argument_4\" type=\"s\" direction=\"in\"/>\n" .
						"			<arg name=\"reply_1\" type=\"t\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_2\" type=\"u\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_3\" type=\"u\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_4\" type=\"s\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_5\" type=\"d\" direction=\"out\"/>\n" .
						"		</method>\n" .
						"		<method name=\"call_x_2\">\n" .
						"			<arg name=\"reply_1\" type=\"t\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_2\" type=\"u\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_3\" type=\"u\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_4\" type=\"s\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_5\" type=\"d\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_6\" type=\"d\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_7\" type=\"d\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_8\" type=\"d\" direction=\"out\"/>\n" .
						"		</method>\n" .
						"		<method name=\"call_x_3\">\n" .
						"			<arg name=\"argument_1\" type=\"s\" direction=\"in\"/>\n" .
						"			<arg name=\"argument_2\" type=\"s\" direction=\"in\"/>\n" .
						"			<arg name=\"argument_3\" type=\"s\" direction=\"in\"/>\n" .
						"			<arg name=\"reply_1\" type=\"u\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_2\" type=\"t\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_3\" type=\"t\" direction=\"out\"/>\n" .
						"			<arg name=\"reply_4\" type=\"t\" direction=\"out\"/>\n" .
						"		</method>\n" .
						"	</interface>\n";

				foreach $signal (@signal_interface)
				{
					$reply .=
						"	<interface name=\"" . $signal . "\">\n" .
						"		<signal name=\"string_call_string\">\n" .
						"			<arg name=\"argument\" type=\"s\"/>\n" .
						"		</signal>\n" .
						"	</interface>\n";
				}

				$reply .= "</node>\n";

				$dbus_server->send_string($reply);
			}
			elsif(($message_interface eq $method_interface) || ($message_interface eq ""))
			{
				if($message_method eq "string_call_void")
				{
					printf STDERR ("string call void method called\n");
					$dbus_server->send_string("string-call-void OK");
				}
				elsif($message_method eq "string_call_string")
				{
					print STDERR ("string_call_string method called with parameters: %s\n", $dbus_server->receive_string());
					$dbus_server->send_string("string-call-string OK");
				}
				elsif($message_method eq "call_x_1")
				{
					my($p0, $p1, $p2, $p3);

					$dbus_server->receive_uint32_uint32_string_string();

					$p0 = $dbus_server->get_rv_uint32_0();
					$p1 = $dbus_server->get_rv_uint32_1();
					$p2 = $dbus_server->get_rv_string_0();
					$p3 = $dbus_server->get_rv_string_1();

					printf STDERR ("x_1 method called with parameters: %u / %u / %s / %s\n", $p0, $p1, $p2, $p3);

					$dbus_server->send_uint64_uint32_uint32_string_double(time(), 0, 1, "call_x_1 OK", 123.456);
				}
				elsif($message_method eq "call_x_2")
				{
					printf STDERR ("x_2 method called\n");

					$dbus_server->send_uint64_x3string_x4double(time(), "string 1", "string 2", "string 3", 1.1, 2.2, 3.3, 4.4);
				}
				else
				{
					$dbus_server->inform_error("unknown method");
				}
			}
			else
			{
				$dbus_server->inform_error("unknown interface");
			}
		}
		elsif($message_type eq "method reply")
		{
			printf STDERR ("unexpected message type, skip\n");
			goto next;
		}
		elsif($message_type eq "error")
		{
			printf STDERR ("unexpected message type, skip\n");
			goto next;
		}
		elsif($message_type eq "signal")
		{
			printf STDERR ("signal received, method: %s, parameter: %s\n", $message_method, $dbus_server->receive_string());
		}
		else
		{
			printf STDERR ("unknown message type, skip\n");
			goto next;
		}

next:
		$dbus_server->reset();
	}
	catch
	{
		printf STDERR ("failed: %s\n", $_);
		$dbus_server->reset();
		sleep(1);
	};
}
