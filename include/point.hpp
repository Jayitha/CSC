//
// Created by Jayitha on 12/04/22.
//

#ifndef COMP_SKY_POINT_HPP
#define COMP_SKY_POINT_HPP

#include "global.hpp"

/**
 * @brief class to represent a data point
 *
 */
class Point
{
public:
  ID id; // id of the point in the database
  vector<data> point;

  /**
   * @brief Construct a new dummy Point object with id 0 and with data sval
   *
   * @param sval
   */
  Point()
  {
    this->id = 0;
    this->point.assign(DIMENSIONALITY, 0);
  }

  explicit Point(data sval)
  {
    this->id = 0; /* id = 0 is reserved for special types of points */
    this->point.assign(DIMENSIONALITY, sval);
  }

  Point(ID id, vector<data> point)
  {
    this->id = id;
    this->point = std::move(point);
  }

  /* Computes Chebyshev distance */
  data operator-(const Point &other) const
  {
    data max_diff = 0;
    for (int i = 0; i < DIMENSIONALITY; i++)
    {
      data dim_diff = abs(this->point[i] - other.point[i]);
      if (dim_diff > max_diff)
        max_diff = dim_diff;
    }
    return max_diff;
  }

  /* Competitiveness check */
  bool operator^(const Point &other) const
  {
    if (*this - other <= EPSILON)
      return true;
    return false;
  }

  /**
   * @brief Operator that specifies a total ordering of points using the
   * SORT_DIMENSION. Ordering is provided by a preference over the attribute set
   * given by
   *
   * SORT_DIMENSION > D1 > D2 > ... > ID
   *
   * Since the ID is a primary key (i.e. no two points have the same ID), this
   * is a total ordering.
   *
   * * Usually all point sets (@see PointSet) are sorted using this ordering.
   *
   * @return true if this point < other
   * @return false if this point >= other
   */
  bool operator<(const Point &other) const
  {
    if (this->point[SORT_DIMENSION - 1] < other.point[SORT_DIMENSION - 1])
      return true;
    if (this->point[SORT_DIMENSION - 1] > other.point[SORT_DIMENSION - 1])
      return false;
    for (int i = 0; i < DIMENSIONALITY; i++)
    {
      if (this->point[i] < other.point[i])
        return true;
      if (this->point[i] > other.point[i])
        return false;
    }
    if (this->id < other.id)
      return true;
    /* this point = other point */
    return false;
  }

  /**
   * @brief Partial order is used when points are the same only their IDs are
     different.
   * ? when is this partial ordering used?
   * ! If this is used when ordering bounding boxes, can be removed since mbb.lb and ub IDs are 0s
   */
  static struct _PartialOrder
  {
    bool compare(const Point &left, const Point &right){
      if (left.point[SORT_DIMENSION - 1] < right.point[SORT_DIMENSION - 1])
        return true;
      if (left.point[SORT_DIMENSION - 1] > right.point[SORT_DIMENSION - 1])
        return false;
      for (int i = 0; i < DIMENSIONALITY; i++)
      {
        if (left.point[i] < right.point[i])
          return true;
        if (left.point[i] > right.point[i])
          return false;
      }
      return false;
    }

    bool operator()(const Point &left, const Point &right)
    {
      return compare(left, right);
    }

    bool operator()(const Point &left, const int &right_idx){
      return compare(left, skyline[right_idx]);
    }

    bool operator()(const int &left_idx, const Point &right){
      return compare(skyline[left_idx], right);
    }

    bool operator()(const int &left_idx, const int &right_idx){
      return compare(skyline[left_idx], skyline[right_idx]);
    }
  } PartialOrder;
  

  /* Used to check if points are same in the partial order */
  bool operator==(const Point &other) const
  {
    for (int i = 0; i < DIMENSIONALITY; i++)
    {
      if (this->point[i] != other.point[i])
        return false;
    }
    return true;
  }

  /* Constructs minimal point */
  static Point minimal(const Point &p1, const Point &p2)
  {
    Point minPoint;
    for (int i = 0; i < DIMENSIONALITY; i++)
    {
      minPoint.point[i] = min(p1.point[i], p2.point[i]);
    }
    return minPoint;
  }

  /* Constructs maximal point */
  static Point maximal(const Point &p1, const Point &p2)
  {
    Point maxPoint;
    for (int i = 0; i < DIMENSIONALITY; i++)
    {
      maxPoint.point[i] = max(p1.point[i], p2.point[i]);
    }
    return maxPoint;
  }

  friend ostream &operator<<(ostream &os, const Point &p);
};

ostream &operator<<(ostream &os, const Point &p)
{
  os << p.id;
#ifdef DEBUG
  os << ": (" << p.point[0];
  for (int i = 1; i < DIMENSIONALITY; i++)
  {
    os << ", " << p.point[i];
  }
  os << ")";
#endif
  return os;
}

ostream &operator<<(ostream &os, vector<Point> &ps)
{
  for (auto &p : ps)
  {
    os << p << endl;
  }
  return os;
}

#endif                 // COMP_SKY_POINT_HPP
