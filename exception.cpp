#include <dbus-tiny.h>

#include <string>
#include <boost/format.hpp>

DbusTinyException::DbusTinyException(const std::string &what) : what_string(what)
{
}

DbusTinyException::DbusTinyException(const char *what) : what_string(what)
{
}

DbusTinyException::DbusTinyException(const boost::format &what) : what_string(what.str())
{
}

const char *DbusTinyException::what() const noexcept
{
	return(what_string.c_str());
}

DbusTinyInternalException::DbusTinyInternalException(const std::string &what) : what_string(what)
{
}

DbusTinyInternalException::DbusTinyInternalException(const char *what) : what_string(what)
{
}

DbusTinyInternalException::DbusTinyInternalException(const boost::format &what) : what_string(what.str())
{
}

const char *DbusTinyInternalException::what() const noexcept
{
	return(what_string.c_str());
}
