#ifndef PHYSYCOM_UTILS_GEOMETRY_HPP
#define PHYSYCOM_UTILS_GEOMETRY_HPP

#include <type_traits>
#include <tuple>

namespace physycom
{
  enum
  {
    X = 0,
    Y
  };

  template <typename point_t>
  static auto to_coords_tuple(point_t &pt)
  {
    return std::make_tuple(&pt.x, &pt.y);
  }

  template <typename point_t>
  point_t centroid(std::vector<point_t> &points)
  {
    point_t c;
    auto tc = to_coords_tuple(c);
    for (auto &p : points)
    {
      auto tp = to_coords_tuple(p);
      *std::get<X>(tc) += *std::get<X>(tp);
      *std::get<Y>(tc) += *std::get<Y>(tp);
    }
    using vtype = typename std::remove_reference<decltype(*std::get<X>(tc))>::type;
    *std::get<X>(tc) /= vtype(points.size());
    *std::get<Y>(tc) /= vtype(points.size());
    return c;
  }

  template <typename point_t>
  bool is_left(point_t &p0, point_t &p1, point_t &p) // fast check : point p is LEFT of segment p1-p2?
  {
    auto tp0 = to_coords_tuple(p0);
    auto tp1 = to_coords_tuple(p1);
    auto tp = to_coords_tuple(p);
    return (*std::get<Y>(tp1) - *std::get<Y>(tp0)) * (*std::get<X>(tp) - *std::get<X>(tp0) ) - ( *std::get<Y>(tp) - *std::get<Y>(tp0) ) * ( *std::get<X>(tp1) - *std::get<X>(tp0) ) > 0;
  }

  template <typename point_t>
  bool point_in_polygon(std::vector<point_t> &polygon, point_t &p)
  {
    // winding number inclusion algorithm
    int wn = 0;
    auto tp = to_coords_tuple(p);
    auto px = *std::get<X>(tp);
    for (int i = 0; i < int(polygon.size()) - 1; ++i)
    {
      auto tp0 = to_coords_tuple(polygon[i]);
      auto tp1 = to_coords_tuple(polygon[i+1]);
      if (*std::get<X>(tp0) <= px)
      {
        if (*std::get<X>(tp1) > px)                 // an upward crossing
          if (is_left(polygon[i], polygon[i+1], p)) // P left of  edge
            ++wn;                                   // have  a valid up intersect
      }
      else
      {
        if (*std::get<X>(tp1) <= px)                   // a downward crossing
          if (! is_left(polygon[i], polygon[i+1], p) ) // P right of  edge
            --wn;                                      // have  a valid down intersect
      }
    }
    return wn != 0; // wn=0 <=> p is outside
  }

}

#endif // PHYSYCOM_UTILS_GEOMETRY_HPP
