%module "DBUS::Tiny"

%include stdint.i
%include std_string.i
%include std_except.i
%include exception.i
%include typemaps.i

%rename receive_uint64_uint32_uint32_string_double_swig receive_uint64_uint32_uint32_string_double;
%rename get_message_swig get_message;
%rename receive_uint32_uint32_string_string_swig receive_uint32_uint32_string_string;
%rename receive_uint64_x3string_x4double_swig receive_uint64_x3string_x4double;
%rename receive_x3string_swig receive_x3string;
%rename receive_uint32_x3uint64_swig receive_uint32_x3uint64;

%{
#include <iostream>
#include <string>
#include "dbus-tiny.h"
%}

%exception
{
	try
	{
		$action
	}
	catch(const DbusTinyException &e)
	{
		std::cout << "DbusTiny::Exception: exception: " << e.what() << std::endl;
		SWIG_exception(SWIG_RuntimeError, "abort");
	}
	catch(...)
	{
		std::cout << "unknown exception caught" << std::endl;
		SWIG_exception(SWIG_RuntimeError, "DbusTiny: generic exception\nabort");
	}
}

%include "dbus-tiny.h"
