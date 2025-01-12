# DBUS-Tiny
A quite literally tiny C++ and Perl interface to libdbus.

Because existing Perl interfaces (like Net::DBUS) are way too complicated for just calling a few methods and send a signal. It really doesn't cover all the cases, just a very few of them, but it may serve as a working example, as I found very little useful examples as how to interface Perl with DBUS.
The server is running in C++, the client in Perl. So that explains the dual-language bindings and the use of SWIG.
