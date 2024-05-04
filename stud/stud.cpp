#include "all"

#include <cstdio>

using namespace stud;

int main(int argc, char** argv)
{
	auto s = __detail::string<char>("hello, world!");
	s.append(" ... Bye");
	eprintln("{}", s.data());
}