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


#ifndef PHYSYCOM_UTILS_SPLIT_HPP
#define PHYSYCOM_UTILS_SPLIT_HPP

#include <vector>

namespace physycom{

	enum{
		token_compress_on,
		token_compress_off
	};

	template<typename E, typename C> inline bool belongs_to(const E& e, const C& c) {
		for (size_t i = 0; i < c.size(); i++) { if (e == c.at(i)) return true; }
		return false;
	}

	void split(std::vector<std::string>& tok, const std::string& str, const std::string& sep, const int& mode = token_compress_off){
		tok.clear();
		if( str.size() ){
			auto start = str.begin();
			for( auto it = str.begin(); it != str.end(); it++ ){
				if( belongs_to(*it, sep) ){
					tok.push_back( std::string(start, it) );
					start = it + 1;
				}
			}
			tok.push_back( std::string(start, str.end() ) );

			if( mode == token_compress_on ){
				auto it = tok.begin();
				while( it != tok.end() ){
					if( *it == "" ) it = tok.erase(it);
					else it++;
				}
			} 
		}
	}

}   // end namespace physycom

#endif // PHYSYCOM_SPLIT_HPP
