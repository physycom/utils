/* Copyright 2017 - Alessandro Fabbri */

/***************************************************************************
This file is part of utils.

utils is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

utils is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with utils. If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/




#ifndef PHYSYCOM_UTILS_COMBEXPLOSION_HPP
#define PHYSYCOM_UTILS_COMBEXPLOSION_HPP


#include <vector>
#include <string>

#include <boost/algorithm/string.hpp>

//////////////////////////////////////////////////////////////// STRINGIFICATION

// stuff to convert any std::vector<T>
// to a std::vector<std::string>
// provided an overload of std::string to_string(T) exists
template<typename T> 
std::vector<typename std::enable_if<!std::is_same<T,std::string>::value, std::string>::type>
to_string(const std::vector<T> &v)
{
	std::vector<std::string> sv;
	for(auto i : v) sv.push_back(std::to_string(i));
	return sv;
}

// is this the proper way to handle T = std::string case?
// don't think so...there should be some metabullshit
// to just say "do nothing"
std::vector<std::string> to_string(const std::vector<std::string> &v)
{
	return v;
}
////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////// COMBINATORIAL EXPANSION

constexpr char separator[] = ",";

// Function which explodes a number of lists of parameters
// by creating all the possible combinations
// return: std::vector<std::string> in the form [ "p11,p21,...,pn1" , ... ]

// Template recursion stopper
// with explicit 2 args implementation
template<typename T, typename U>
std::vector<std::string> combo(const T &a, const U &b)
{
	//auto as = std::to_string(a); // this won't compile
	auto as = to_string(a);
	auto bs = to_string(b);
	std::vector<std::string> c;
	for(auto ai : as)
		for(auto bi : bs)
			c.push_back(ai+separator+bi);
	return c;
}

// Variadic template recursive version
template<typename T, typename... Args>
std::vector<std::string> combo(const T &a, Args... args)
{
	return combo(a, combo(args...));
}

// This version accepts a std::vector<std::vector<T>> as argument
template<typename T>
std::vector<std::string> combo(const std::vector<std::vector<T>> &all)
{
	auto temp = to_string(all[0]);
	for(int i=1; i<all.size(); ++i)
		temp = combo(temp, to_string(all[i]));
	return temp;	
}

template<typename T>
T stoanything(const std::string &s){}

template<> double stoanything(const std::string &s)
{
	return stod(s);
}

template<> int stoanything(const std::string &s)
{
	return stoi(s);
}

template<typename T>
std::vector<std::vector<T>> combov(std::vector<std::vector<T>> all)
{
	auto comb = combo(all);
	std::vector<std::string> tok;
	std::vector<std::vector<T>> cv;
	for(const auto &seq : comb)
	{
		std::vector<T> temp;
		boost::split(tok, seq, boost::is_any_of(separator), boost::token_compress_off);
		for( auto &val : tok) temp.push_back(stoanything<T>(val));
		cv.push_back(temp);
	} 
	return cv;
}

////////////////////////////////////////////////////////////////////////////////
#endif //PHYSYCOM_UTILS_COMBEXPLOSION_HPP
