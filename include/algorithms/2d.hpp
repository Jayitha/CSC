//
// Created by Jayitha on 14/04/22.
//

#ifndef COMP_SKY_2D_HPP
#define COMP_SKY_2D_HPP

#include "pointSet.hpp"

/**
 * @brief Algorithm uses the sweepline paradigm 
 * 
 * the for loops sets 2 parameters: start and end
 * 
 */
void maximalSweepline(vector<int> & part_skyline_idx, RTree<int, data> & partitionSkylineRTree) {

  /* algorithm should only be used when d = 2 */
  assert(DIMENSIONALITY == 2);
  int pmin = 0;
  int pmax = 0;
  while (max(pmin, pmax) < part_skyline_idx.size()) {
    while (!(skyline[part_skyline_idx[pmin]] ^ skyline[part_skyline_idx[pmax]]))
      pmin++;
    while (pmax + 1 < part_skyline_idx.size() &&
           skyline[part_skyline_idx[pmin]] ^ skyline[part_skyline_idx[pmax + 1]])
      pmax++;
    PointSet mcs;
    for (int i = pmin; i <= pmax; i++)
      mcs += part_skyline_idx[i];
    CompSky.emplace_back(mcs);
    pmax++;
  }
}
#endif // COMP_SKY_2D_HPP
