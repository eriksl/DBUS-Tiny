#!/usr/bin/perl -w

use strict;
use warnings;

use Getopt::Long qw(:config gnu_compat noignore_case);
use Try::Tiny;
use Scalar::Util;
use DBUS::Tiny;

use Data::Dumper;

my($introspect);
my($service);
my($interface);
my($string_call_void);
my($string_call_string);
my($call_x_1);
my($call_x_2);
my($signal_string);
my(@arguments);
my($argument);
my($rv1);

GetOptions(
		"service|s=s"				=> \$service,
		"interface|i=s"				=> \$interface,
		"introspect|I"				=> \$introspect,
		"string-call-void|v=s"		=> \$string_call_void,
		"string-call-string|c=s"	=> \$string_call_string,
		"call-x-1|1=s"				=> \$call_x_1,
		"call-x-2|2=s"				=> \$call_x_2,
		"signal-string|S=s"			=> \$signal_string,
		"argument|a=s"				=> \@arguments,
);

try
{
	my($dbus_client) = new DBUS::Tiny::DbusTinyClient;

	if(!defined($service) && !defined($signal_string))
	{
		print STDERR ("usage: dbus-tiny-client.pl <options> <arguments>\n");
		print STDERR ("       -s|--service <service>\n");
		print STDERR ("       -i|--interface <interface> (optional for method calling)\n");
		print STDERR ("       -I|--introspect (introspect)\n");
		print STDERR ("       -v|--string-call-void <method>\n");
		print STDERR ("       -c|--string-call-string <method>\n");
		print STDERR ("       -1|--call-x-1 <method>\n");
		print STDERR ("       -2|--call-x-2 <method>\n");
		print STDERR ("       -S|--signal-string <method>\n");
		print STDERR ("       -a|--argument <argument> (may be repeated)\n");
	}

	$service = "/org/freedesktop/DBus/dummy" if(!defined($service));
	$interface = "" if(!defined($interface));

	while(defined($argument = shift(@ARGV)))
	{
		push(@arguments, $argument);
	}

	if(defined($introspect))
	{
		$dbus_client->send_void($service, "org.freedesktop.DBus.Introspectable", "Introspect");
		$rv1 = $dbus_client->receive_string();
		printf ("output: %s\n", $rv1);
	}
	elsif(defined($string_call_void))
	{
		$dbus_client->send_void($service, $interface, $string_call_void);
		$rv1 = $dbus_client->receive_string();
		printf ("output: %s\n", $rv1);
	}
	elsif(defined($string_call_string))
	{
		die("string-call-string takes one argument: string") if(scalar(@arguments) != 1);

		$dbus_client->send_string($service, $interface, $string_call_string, $arguments[0]);
		$rv1 = $dbus_client->receive_string();
		printf ("output: %s\n", $rv1);
	}
	elsif(defined($call_x_1))
	{
		my($r1, $r2, $r3, $r4, $r5);

		die("call_x_1 takes four arguments: int, int, string, string") if(scalar(@arguments) != 4);
		die("call_x_1 argument one should be an integer number") if(!Scalar::Util::looks_like_number($arguments[0]));
		die("call_x_1 argument two should be an integer number") if(!Scalar::Util::looks_like_number($arguments[1]));

		$dbus_client->send_uint32_uint32_string_string($service, $interface, $call_x_1, int($arguments[0]), int($arguments[1]), $arguments[2], $arguments[3]);
		$dbus_client->receive_uint64_uint32_uint32_string_double();

		$r1 = $dbus_client->get_rv_uint64_0();
		$r2 = $dbus_client->get_rv_uint32_0();
		$r3 = $dbus_client->get_rv_uint32_1();
		$r5 = $dbus_client->get_rv_string_0();
		$r4 = $dbus_client->get_rv_double_0();

		printf ("results: %llu/%lu/%lu/%f:%s\n", $r1, $r2, $r3, $r4, $r5);
	}
	elsif(defined($call_x_2))
	{
		my($r0, $r1, $r2, $r3, $r4, $r5, $r6, $r7);

		die("call_x_2 takes no arguments") if(scalar(@arguments) != 0);

		$dbus_client->send_void($service, $interface, $call_x_2);
		$dbus_client->receive_uint64_x3string_x4double();

		$r0 = $dbus_client->get_rv_uint64_0();
		$r1 = $dbus_client->get_rv_string_0();
		$r2 = $dbus_client->get_rv_string_1();
		$r3 = $dbus_client->get_rv_string_2();
		$r4 = $dbus_client->get_rv_double_0();
		$r5 = $dbus_client->get_rv_double_1();
		$r6 = $dbus_client->get_rv_double_2();
		$r7 = $dbus_client->get_rv_double_3();

		printf ("%llu / %s / %s / %s / %f / %f / %f / %f\n", $r0, $r1, $r2, $r3, $r4, $r5, $r6, $r7);
	}
	elsif(defined($signal_string))
	{
		die("signal_string takes one argument: string") if(scalar(@arguments) != 1);

		$dbus_client->signal_string($service, $interface, $signal_string, $arguments[0]);
	}
}
catch
{
	printf STDERR ("failed: %s\n", $_);
	exit(1);
};
