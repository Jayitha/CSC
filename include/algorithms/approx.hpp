//
// Created by Jayitha on 14/04/22.
//

#ifndef APPROX_HPP
#define APPROX_HPP

#include "pointSet.hpp"

void approxMCS(vector<int> &partitionSkyline, RTree<int, data>& partitionSkylineRTree) {
    RTree<BB, data> MCSRtree;
    for(int i = 0; i < skyline.size(); i++){
        BB crange = BB::crange(i);
        PointSet new_cs = range(crange, partitionSkyline);
        if(!MCSRtree.Subsumes(new_cs.mbb.lb.point.data(), new_cs.mbb.ub.point.data())){
            vector<BB> subsumed = MCSRtree.Subsumed(new_cs.mbb.lb.point.data(), new_cs.mbb.ub.point.data());
            for(auto bb : subsumed){
                MCSRtree.Remove(bb.lb.point.data(), bb.ub.point.data(), bb);
            }
            MCSRtree.Insert(new_cs.mbb.lb.point.data(), new_cs.mbb.ub.point.data(), new_cs.mbb);
        }
    }

    vector<BB> MCS = MCSRtree.Range(Point(domain(0)).point.data(), Point(domain(1)).point.data());
    for(auto bb : MCS){
        CompSky.emplace_back(range(bb, partitionSkyline));
    }
}

void approxCS(vector<int> &partitionSkyline, RTree<int, data>& partitionSkylineRTree) {
    RTree<BB, data> MCSRtree;
    for(int i = 0; i < skyline.size(); i++){
        BB crange = BB::crange(i);
        PointSet new_cs = range(crange, partitionSkyline);
        if(!MCSRtree.Subsumes(new_cs.mbb.lb.point.data(), new_cs.mbb.ub.point.data())){
            MCSRtree.Insert(new_cs.mbb.lb.point.data(), new_cs.mbb.ub.point.data(), new_cs.mbb);
        }
    }

    vector<BB> MCS = MCSRtree.Range(Point(domain(0)).point.data(), Point(domain(1)).point.data());
    for(auto bb : MCS){
        CompSky.emplace_back(range(bb, partitionSkyline));
    }
}
#endif // APPROX_HPP
