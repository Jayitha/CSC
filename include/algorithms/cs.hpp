//
// Created by Jayitha on 18/04/22.
//

#ifndef COMP_SKY_HPP
#define COMP_SKY_HPP

#include "boundingBox.hpp"
#include "point.hpp"
#include "pointSet.hpp"

void cliqueEnumeration(vector<int> &part_skyline, RTree<int, data> &part_skylineRtree) {

  RTree<PointSet, data> CSRTree;
  int num_thousands = 0;
  int num_sets = 0;
  int num_points = 0;

  for (auto &p : part_skyline) {
    num_points++;
    if(num_sets / 1000 > num_thousands) {
      num_thousands = num_sets / 1000;
      cout << "Processed " << num_thousands << " sets and " << num_points << " points." << endl;
    }

    BB crange = BB::crange(p);
    vector<PointSet> candidates = CSRTree.Subsumed(crange.lb.point.data(), crange.ub.point.data());

    for (auto &cs : candidates) {
        PointSet new_cs = cs + p;
        CSRTree.Insert(new_cs.mbb.lb.point.data(), new_cs.mbb.ub.point.data(), new_cs);
        CompSky.emplace_back(new_cs);
    }

    if(candidates.empty()){
        PointSet new_cs(p);
        CSRTree.Insert(new_cs.mbb.lb.point.data(), new_cs.mbb.ub.point.data(), new_cs);
        CompSky.emplace_back(new_cs);
    }
  }
}

void cliqueEnumeration(){
    vector<int> skyline_ids(SKY_CARDINALITY);
    std::iota(skyline_ids.begin(), skyline_ids.end(), 0);
    cliqueEnumeration(skyline_ids, skylineRTree);
}
#endif // COMP_SKY_MAXCLIQUEENUM_HPP
