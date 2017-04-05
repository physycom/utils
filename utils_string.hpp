#ifndef PHYSYCOM_SPLIT_HPP
#define PHYSYCOM_SPLIT_HPP

#include <boost/algorithm/string.hpp>

namespace physycom{

  std::string replace_all_onthefly(const std::string &s, const std::string &old_, const std::string &new_) {
    std::string r(s);
    boost::replace_all(r, old_, new_);
    return r;
  }

}   // end namespace physycom

#endif // PHYSYCOM_SPLIT_HPP
