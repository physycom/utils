#ifndef PHYSYCOM_SPLIT_HPP
#define PHYSYCOM_SPLIT_HPP

#include <boost/algorithm/string.hpp>

namespace physycom{

  std::string replace_all_onthefly(std::string s, const std::string &old_, const std::string &new_) {
    boost::replace_all(s, old_, new_);
    return s;
  }

}   // end namespace physycom

#endif // PHYSYCOM_SPLIT_HPP
