#ifndef PHYSYCOM_UTILS_COMBINATORIC_HPP
#define PHYSYCOM_UTILS_COMBINATORIC_HPP

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

namespace physycom
{

  class combinations
  {
  public:
    combinations() {};
    ~combinations() {};

    template<typename T>
    std::vector<T> operator()(const T &elems, unsigned long k)
    {
      assert(k > 0 && k <= elems.size());
      std::vector<T> tuple;
      pos.resize(k);
      combinations_recursive(elems, k, 0, 0, tuple);
      return tuple;
    }

  private:
    std::vector<unsigned long> pos;

    template<typename T>
    void combinations_recursive(const T &elems, unsigned long k, unsigned long depth, unsigned long margin, std::vector<T> &tuple)
    {
      if (depth >= k)
      {
        tuple.push_back(T());
        for (unsigned long i = 0; i < pos.size(); ++i) tuple.back().push_back(elems[pos[i]]);
        return;
      }

      if ((elems.size() - margin) < (k - depth))
        return;

      for (unsigned long i = margin; i < elems.size(); ++i)
      {
        pos[depth] = i;
        combinations_recursive(elems, k, depth + 1, i + 1, tuple);
      }

      return;
    }
  };

} // end of namespace physycom

#endif // PHYSYCOM_UTILS_COMBINATORIC_HPP