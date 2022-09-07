//
// Created by Jayitha on 12/04/22.
//

#ifndef COMP_SKY_BOUNDINGBOX_HPP
#define COMP_SKY_BOUNDINGBOX_HPP

#include <utility>

#include "global.hpp"
#include "point.hpp"

/**
 * @brief d-dimensional bounding box
 *
 */
class BB
{
public:
    Point lb;
    Point ub;

    BB()
    {
        this->lb = Point(domain(1));
        this->lb.id = 0;
        this->ub = Point(domain(0));
        this->ub.id = 0;
    }

    /* BB of just one point */
    explicit BB(Point p)
    {
        this->lb = p;
        this->lb.id = 0;
        this->ub = p;
        this->ub.id = 0;
    }

    BB(int p_id)
    {
        this->lb = skyline[p_id];
        this->lb.id = 0;
        this->ub = skyline[p_id];
        this->ub.id = 0;
    }

    /* BB given lb and ub */
    BB(Point lb, Point ub)
    {
        /* asserting ub >= lb */
        bool ubglb = true;
        for (int i = 0; i < DIMENSIONALITY; i++)
        {
            ubglb = ubglb && ub.point[i] >= lb.point[i];
        }
        assert(ubglb);

        this->lb = lb;
        this->lb.id = 0;
        this->ub = ub;
        this->ub.id = 0;
    }

    BB(const unordered_set<int> &point_idx)
    {
        this->lb = Point(domain(1));
        this->ub = Point(domain(0));
        for (auto &point : point_idx)
            *this += point;
    }

    BB(const vector<int> &point_idx)
    {
        this->lb = Point(domain(1));
        this->ub = Point(domain(0));
        for (auto &point : point_idx)
            *this += point;
    }

    /* BB of vector of BBs -> union */
    BB(const vector<BB> &bbs)
    {
        this->lb = Point(domain(1));
        this->ub = Point(domain(0));
        for (auto &bb : bbs)
            *this += bb;
    }

    /* checks if BB is valid i.e. ub >= lb */
    bool valid() const
    {
        for (int i = 0; i < DIMENSIONALITY; i++)
            if (this->ub.point[i] < this->lb.point[i])
                return false;
        return true;
    }

    /* || ub - lb ||_infinity */
    data size() const
    {
        if (!this->valid())
            return -1;
        return this->ub - this->lb;
    }

    /* checks if box is at most EPSILON in size */
    bool competitive() const
    {
        if (this->size() <= EPSILON)
            return true;
        return false;
    }

    /* checks if this box contains point p */
    bool contains(Point p) const
    {
        if(!this->valid())
            return false;
        for (int i = 0; i < DIMENSIONALITY; i++)
        {
            if (p.point[i] < this->lb.point[i] || p.point[i] > this->ub.point[i])
                return false;
        }
        return true;
    }

    bool contains(int p_id) const
    {
        return this->contains(skyline[p_id]);
    }

    /* checks if this box contains other box */
    bool contains(const BB &other) const
    {
        if(!this->valid())
            return false;
        if(!other.valid())
            return true;
        if (this->contains(other.lb) && this->contains(other.ub))
            return true;
        return false;
    }

    /* provides an orderning of BBs -> ordered by lb followed by bb */
    bool operator<(const BB &other) const
    {
        if (this->lb < other.lb)
            return true;
        if (other.lb < this->lb)
            return false;
        if (this->ub < other.ub)
            return true;
        return false;
    }

    /* updates BB to contain p */
    void operator+=(const Point &p)
    {
        this->lb = Point::minimal(this->lb, p);
        this->ub = Point::maximal(this->ub, p);
    }

    /* updates BB to contain bb */
    void operator+=(const BB &bb)
    {
        this->lb = Point::minimal(this->lb, bb.lb);
        this->ub = Point::maximal(this->ub, bb.ub);
    }

    void operator+=(const int p_id)
    {
#ifdef DEBUG
        assert(p_id >= 0 && p_id < skyline.size());
#endif
        this->lb = Point::minimal(this->lb, skyline[p_id]);
        this->ub = Point::maximal(this->ub, skyline[p_id]);
    }

    bool isborder(const int p_id){
        if(!this->valid()) return false;
        for(int i = 0; i < DIMENSIONALITY; i++){
            if(skyline[p_id].point[i] == this->lb.point[i] || skyline[p_id].point[i] == this->ub.point[i]){
                return true;
            }
        }
        return false;
    }

    bool operator==(const BB &bb) const
    {
        if (this->lb == bb.lb && this->ub == bb.ub)
            return true;
        return false;
    }

    data operator-(const Point &p) const
    {
        if(!this->valid())
            return -1;
        // if (this->contains(p))
        //     return 0;
        // else
            return max(this->lb - p, this->ub - p);
    }

    data operator-(const int p_id) const
    {
        return *this - skyline[p_id];
    }

    data operator-(const BB bb) const
    {
        if(!this->valid() || !bb.valid())
            return -1;
        // if(this->contains(bb) || bb.contains(*this))
        //     return 0;
        data dist = max(this->lb - bb.lb, this->ub - bb.lb);
        dist = max(dist, max(this->lb - bb.ub, this->ub - bb.ub));
        return dist;
    }

    /* checks if p is competitive with whole bb */
    bool operator^(const Point &p) const
    {
        // if (*this - p <= EPSILON)
        return BB::crange(p).contains(*this);
    }

    bool operator^(const int p_id) const
    {
        return *this ^ skyline[p_id];
    }

    bool operator^(const BB bb) const
    {
        if(!this->valid() || !bb.valid())
            return true;
        return BB::crange(bb).contains(*this);
    }

    bool operator^=(const BB bb) const
    {
        if(!this->valid() || !bb.valid())
            return true;
        return BB::crange(bb).intersects(*this);
    }

    /* check if p is partially competitive with bb */
    bool operator^=(const Point &p) const
    {
        return BB::crange(p).intersects(*this);
    }

    bool operator^=(const int p_id) const
    {
        return *this ^= skyline[p_id];
    }

    static BB crange(int p_id)
    {
        return BB::crange(skyline[p_id]);
    }

    bool intersects(const BB bb){
        if(!this->valid() || !bb.valid()){
            if(!this->valid() && !bb.valid()){
                return true;
            }
            else{
                return false;
            }
        }
        BB inter = BB::intersectionBB(*this, bb);
        return inter.valid();
    }

    /* returns competitive range of p */
    static BB crange(Point p)
    {
        BB compRange = BB(p);
        for (int i = 0; i < DIMENSIONALITY; i++)
        {
            compRange.lb.point[i] = max(domain(0), p.point[i] - EPSILON);
            compRange.ub.point[i] = min(domain(1), p.point[i] + EPSILON);
        }
        return compRange;
    }

    /* returns competitive range of bb */
    static BB crange(BB bb)
    {
        BB compRange = bb;
        if(!bb.valid()) return BB();
        for (int i = 0; i < DIMENSIONALITY; i++)
        {
            compRange.lb.point[i] = max(domain(0), min(bb.lb.point[i], bb.ub.point[i] - EPSILON));
            compRange.ub.point[i] = min(domain(1), max(bb.ub.point[i], bb.lb.point[i] + EPSILON));
        }
        return compRange;
    }

    static BB partialcrange(BB bb)
    {
        BB compRange = bb;
        if(!bb.valid()) return BB();
        for (int i = 0; i < DIMENSIONALITY; i++)
        {
            compRange.lb.point[i] = max(domain(0), bb.lb.point[i] - EPSILON);
            compRange.ub.point[i] = min(domain(1), bb.ub.point[i] + EPSILON);
        }
        return compRange;
    }

    static BB dominanceRegion(Point p){
        BB domReg(p);
        domReg += Point(domain(1));
        return domReg;
    }

    static BB dominatingRegion(Point p){
        BB domReg(p);
        domReg += Point(domain(0));
        return domReg;
    }

    /* Computes union of the two bounding boxes */
    static BB unionBB(const BB &bb1, const BB &bb2)
    {
        BB unionBB;
        if(!bb1.valid()) return bb2;
        if(!bb2.valid()) return bb1;
        unionBB.lb = Point::minimal(bb1.lb, bb2.lb);
        unionBB.ub = Point::maximal(bb1.ub, bb2.ub);
        return unionBB;
    }

    /* computes intersection of the two bounding boxes */
    static BB intersectionBB(const BB &bb1, const BB &bb2)
    {
        BB intersectionBB;
        if(!bb1.valid() || !bb2.valid()) return BB();
        intersectionBB.lb = Point::maximal(bb1.lb, bb2.lb);
        intersectionBB.ub = Point::minimal(bb1.ub, bb2.ub);
        if (intersectionBB.valid())
            return intersectionBB;
        return {};
    }

    friend ostream &operator<<(ostream &os, BB bb);
};

/* output to stream */
ostream &operator<<(ostream &os, BB bb)
{
    os << "lb: " << bb.lb << " ub: " << bb.ub;
    return os;
}

#endif // COMP_SKY_BOUNDINGBOX_HPP
