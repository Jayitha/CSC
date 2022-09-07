//
// Created by Jayitha on 12/04/22.
//

#ifndef COMP_SKY_POINTSET_HPP
#define COMP_SKY_POINTSET_HPP

#include "boundingBox.hpp"
#include "point.hpp"

class PointSet
{
public:
  unordered_set<int> point_idx;
  BB mbb;

  PointSet()
  {
    this->point_idx.clear();
    this->mbb = BB();
  }

  PointSet(const unordered_set<int> &point_idx)
  {
    this->point_idx = point_idx;
    this->mbb = BB(this->point_idx);
  }

  PointSet(const vector<int> &point_idx)
  {
    this->point_idx = unordered_set<int>(point_idx.begin(), point_idx.end());
    this->mbb = BB(this->point_idx);
  }

  PointSet(const int point_id)
  {
    this->point_idx.insert(point_id);
    this->mbb = BB(point_id);
  }

  PointSet(const PointSet &other)
  {
    this->point_idx = other.point_idx;
    this->mbb = other.mbb;
  }

  /* cardinality of set */
  int size() const { return this->point_idx.size(); }

  /* size of mbb */
  data mbbSize() const { return this->mbb.size(); }

  /* competitiveness check */
  bool competitive() const
  {
    if(this->empty())
      return true;
    return this->mbb.competitive();
  }

  bool empty() const { return this->point_idx.empty(); }

  /* add point to set */
  void operator+=(const int p)
  {
    this->point_idx.insert(p);
    this->mbb += p;
  }

  void operator-=(const int p)
  {
    if(this->empty())
      return;
    if (this->contains(p))
    {
      this->point_idx.erase(p);
      if (this->mbb.isborder(p))
        this->mbb = BB(this->point_idx);
    }
  }

  /* checks if two sets are the same i.e. contain the same exact points (including IDs) */
  /* We check many conditions so we don't encounter worst case of O(n) (or is it O(n^2)) */
  // Nice scissors keyboard, good spring
  bool operator==(const PointSet &other) const
  {
    if (this->size() != other.size())
      return false;
    if (!(this->mbb == other.mbb))
      return false;
    return this->point_idx == other.point_idx;
  }

  bool operator^(const Point &p) const { return this->empty() || (this->mbb ^ p); }

  bool operator^=(const Point &p) const { return this->empty() || (this->mbb ^= p); }

  bool operator^(const int p) const { return this->empty() || (this->mbb ^ p); }

  bool operator^=(const int p) const { return this->empty() || (this->mbb ^= p); }

  bool operator^(const PointSet pset) const { return this->empty() || pset.empty() || (this->mbb ^ pset.mbb); }
  
  bool operator^=(const PointSet &other) const { 
    if(*this ^ other) return true;
    if (!(this->mbb ^= other.mbb))
      return false;
    for(auto p : other.point_idx)
      if(*this ^ p) return true;
    return false;
  }

  data operator-(const Point &p) const
  {
    if (this->empty())
      return -1;
    return this->mbb - p;
  }

  data operator-(const int p_id) const
  {
    return *this - skyline[p_id];
  }

  PointSet operator+(const int i) const
  {
    PointSet ret = *this;
    ret += i;
    return ret;
  }

  /* set of points in the range bb */
  PointSet range(const BB &bb) const
  {
    PointSet rangeResults;
    if(this->empty())
      return rangeResults;
    if (!(BB::intersectionBB(this->mbb, bb).valid()))
      return rangeResults;
    for (auto &p_id : this->point_idx)
      if (bb.contains(p_id))
        rangeResults += p_id;
    return rangeResults;
  }

  bool contains(const int &p_id) const
  {
    // *this condition was not removed because in the best case searching is O(1) and in the worst case it is O(n)
    if(this->empty())
      return false;
    if (!this->mbb.contains(p_id))
      return false;
    if (this->point_idx.find(p_id) == this->point_idx.end())
      return false;
    return true;
  }

  static PointSet unionPointSet(PointSet &pset1, PointSet &pset2)
  {
    PointSet unionPSet;
    unionPSet = pset1;
    for (auto &p : pset2.point_idx)
    {
      if (!pset1.contains(p))
        unionPSet += p;
    }
    return unionPSet;
  }

  static PointSet intersectionPointSet(PointSet &pset1, PointSet &pset2)
  {
    PointSet intersectionPSet;
    if (pset1.size() < pset2.size())
    {
      for (auto &p : pset1.point_idx)
      {
        if (pset2.contains(p))
          intersectionPSet += p;
      }
    }
    else
    {
      for (auto &p : pset2.point_idx)
      {
        if (pset1.contains(p))
          intersectionPSet += p;
      }
    }
    return intersectionPSet;
  }

  static PointSet differencePointSet(PointSet &pset1, PointSet &pset2)
  {
    if(pset2.empty())
      return pset1;
    PointSet differencePSet;
    for (auto &p : pset1.point_idx)
    {
      if (!pset2.contains(p))
        differencePSet += p;
    }
    return differencePSet;
  }

  /* checks if this set subsumes other set */
  bool subsumes(const PointSet &other) const
  {
    if(other.empty())
      return true;
    if(this->empty() && !other.empty())
      return false;
    if (!this->mbb.contains(other.mbb))
      return false;
    if (this->size() < other.size())
      return false;
    for (auto &p_id : other.point_idx)
    {
      if (!this->contains(p_id))
        return false;
    }
    return true;
  }

  int first() const
  {
    assert(!this->empty());
    if (!this->empty())
      return *(this->point_idx.begin());
    return -1;
  }

  PointSet slice(const int i) const
  {
    PointSet resultPSet;
    if(this->empty())
      return resultPSet;
    for (auto &p : point_idx)
    {
      if (p <= i)
        resultPSet += p;
    }
    return resultPSet;
  }

  PointSet closure() const
  {
    if (this->empty())
      return PointSet::K0();
    PointSet ret = *this;
    for (int j = 0; j < SKY_CARDINALITY; j++)
    {
      if (!ret.contains(j) && ret ^ j)
        ret += j;
    }
    return ret;
  }

  PointSet neighboursof(const int i) const
  {
    PointSet ret;

    if(this->empty())
      return ret;

    if (*this ^ i)
    {
      ret = *this;
      ret -= i;
      return ret;
    }
    
    if (!(*this ^= i))
    {
      return ret;
    }

    for (auto &p_id : this->point_idx)
    {
      if (i != p_id && skyline[p_id] ^ skyline[i])
        ret += p_id;
    }
    return ret;
  }

  PointSet neighboursof(const PointSet pset) const
  {
    PointSet ret;

    if(this->empty())
      return ret;

    if (!(*this ^= pset))
    {
      return ret;
    }

    if (*this ^ pset)
    {
      ret = *this;
      for(auto &p_id: pset.point_idx)
        ret -= p_id;
      return ret;
    }

    for (auto &p_id : this->point_idx)
    {
      if (!pset.contains(p_id) && pset ^ p_id)
        ret += p_id;
    }
    return ret;
  }

  PointSet nonneighboursof(const int p_id)
  {
    PointSet ret;
    // BB crange = BB::crange(p_id);
    // BB inter = BB::intersectionBB(crange, this->mbb);
    // if (crange.contains(this->mbb))
    //   return ret;
    // if (!inter.isValid())
    //   return *this;
    if(this->empty())
      return ret;
    for (auto &p : this->point_idx)
    {
      if (!(skyline[p] ^ skyline[p_id]))
        ret += p;
    }
    return ret;
  }

  static PointSet K0()
  {
    PointSet ret;
    ret += 0;
    return ret.closure();
  }

  static PointSet sky()
  {
    PointSet ret;
    for (int i = 0; i < SKY_CARDINALITY; i++)
      ret += i;
    return ret;
  }

  friend ostream &operator<<(ostream &os, PointSet pset);
};

ostream &operator<<(ostream &os, PointSet pset)
{
  os << "Set (" << pset.size() << "): ";
  for (auto &p_id : pset.point_idx)
    os << skyline[p_id].id << " ";
  return os;
}

PointSet range(const BB bb, int max_idx = SKY_CARDINALITY - 1){
  PointSet ret;
  if(!bb.valid())
    return ret;
  int ub = upper_bound(skyline.begin(), skyline.end(), bb.ub, Point::PartialOrder) - skyline.begin();
  ub = min(ub, max_idx + 1);
  int lb = lower_bound(skyline.begin(), skyline.end(), bb.lb, Point::PartialOrder) - skyline.begin();
  for (int j = lb; j < ub; j++)
  {
    if (bb.contains(j))
      ret += j;
  }
  return ret;
}

PointSet range(const BB bb, vector<int> & partitionSkyline, int max_idx = SKY_CARDINALITY - 1){
  PointSet ret;
  if(!bb.valid())
    return ret;
  int ub = upper_bound(partitionSkyline.begin(), partitionSkyline.end(), bb.ub, Point::PartialOrder) - partitionSkyline.begin();
  ub = min(ub, max_idx + 1);
  int lb = lower_bound(partitionSkyline.begin(), partitionSkyline.end(), bb.lb, Point::PartialOrder) - partitionSkyline.begin();
  for (int j = lb; j < ub; j++)
  {
    if (bb.contains(partitionSkyline[j]))
      ret += partitionSkyline[j];
  }
  return ret;
}

PointSet neighboursof(const int i)
{
  PointSet ret;
  BB crange = BB::crange(i);
  int ub = upper_bound(skyline.begin(), skyline.end(), crange.ub, Point::PartialOrder) - skyline.begin();
  int lb = lower_bound(skyline.begin(), skyline.end(), crange.lb, Point::PartialOrder) - skyline.begin();
  for (int j = lb; j < ub; j++)
  {
    if (i != j && skyline[j] ^ skyline[i])
      ret += j;
  }
  return ret;
}

PointSet neighboursof(const PointSet pset)
{
  PointSet ret;
  if (!pset.competitive())
    return ret;

  BB crange = BB::crange(pset.mbb);
  int ub = upper_bound(skyline.begin(), skyline.end(), crange.ub, Point::PartialOrder) - skyline.begin();
  int lb = lower_bound(skyline.begin(), skyline.end(), crange.lb, Point::PartialOrder) - skyline.begin();
  for (int j = lb; j < ub; j++)
  {
    if (!pset.contains(j) && pset ^ j)
      ret += j;
  }
  return ret;
}

PointSet neighboursofsome(const PointSet pset)
{

  PointSet ret;
  BB crange = BB::partialcrange(pset.mbb);
  int ub = upper_bound(skyline.begin(), skyline.end(), crange.ub, Point::PartialOrder) - skyline.begin();
  int lb = lower_bound(skyline.begin(), skyline.end(), crange.lb, Point::PartialOrder) - skyline.begin();
  for (int j = lb; j < ub; j++)
  {
    if (!pset.contains(j) && crange.contains(j))
      ret += j;
  }
  return ret;
}

#endif // COMP_SKY_POINTSET_HPP
