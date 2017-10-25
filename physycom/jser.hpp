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


#ifndef PHYSYCOM_UTILS_JSER_HPP
#define PHYSYCOM_UTILS_JSER_HPP

#include <vector>

// Utility class to automatize
// the conversion from and to JSON object

// To use it you must provide a valid
// json type (e.g using using)

namespace physycom
{
	// (De)Serialize JSON into class
	template<typename json_t, class T>
	std::vector<T> deserialize(const json_t j)
	{
	  std::vector<T> trip(0);
	  T point;
	  if( j.is_array() )
	    for(const auto &r : j.array_range())
	      trip.emplace_back(r);
	  else if ( j.is_object() )
	    for(const auto &r : j.object_range())
	      trip.emplace_back(r.value());
	  return trip;
	}

	template<typename json_t, class T>
	json_t serialize(const std::vector<T> &vec, char mode = 'a')
	{
	  json_t j;
	  json_t r;
	  if ( mode == 'a')      // array
	  {
	    j = json_t::array();
	    for(const auto &r : vec)
	      j.add(r.to_json());
	  }
	  else if ( mode ==  'o')   // object
	  {
	    size_t cnt = 0;
	    std::string prefix = "record_";
	    for(const auto &r : vec)
	    {
	      std::stringstream ss;
	      ss << prefix << std::setw(5) << std::setfill('0') << std::endl;
	      j[ss.str()] = r.to_json();
	    }
	  }
	  else throw std::runtime_error("JSON is not array nor object");
	  return j;
	}

}   // end namespace physycom

#endif // PHYSYCOM_UTILS_JSER_HPP
