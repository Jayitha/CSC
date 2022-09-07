//
// Created by Jayitha on 15/04/22.
//

#ifndef COMP_SKY_ALGORITHMS_HPP
#define COMP_SKY_ALGORITHMS_HPP

#include "global.hpp"
#include "2d.hpp"
#include "approx.hpp"
#include "bk.hpp"
#include "cs.hpp"
#include "maxCliqueEnum.hpp"
#include "tsukiyama.hpp"
#include "maximalBrute.hpp"

ostream& operator<<(ostream& os, const vector<PointSet>& partitions){
  for(auto &partition: partitions){
    os << partition << endl;
  }
  return os;
}

void partitionSkyline(){
  vector<vector<int>> oldPartition(1, partitionSkylines[0]);
  vector<vector<int>> newPartition;

  for(int i = 0; i < DIMENSIONALITY; i++){
    newPartition.clear();

    for(auto &partition : oldPartition){
      BB bb(partition);
      if((bb.ub.point[i] - bb.lb.point[i]) <= EPSILON) {
        newPartition.emplace_back(partition);
        continue;
      }

      sort(partition.begin(), partition.end(), [i](const int &a, const int &b) {
        if(skyline[a].point[i] > skyline[b].point[i])
          return true;
        else if(skyline[a].point[i] == skyline[b].point[i])
          return skyline[a].id > skyline[b].id;
        else
          return false;
      });

      newPartition.emplace_back(vector<int>());
      newPartition.back().emplace_back(partition.back());
      partition.pop_back();

      while(!partition.empty()){
        if((skyline[partition.back()].point[i] - skyline[newPartition.back().back()].point[i]) > EPSILON)
          newPartition.emplace_back(vector<int>());
        newPartition.back().emplace_back(partition.back());
        partition.pop_back();
      }
    }
    oldPartition.swap(newPartition);
  }

  partitionSkylines.swap(oldPartition);

  for(auto &partition : partitionSkylines){
    sort(partition.begin(), partition.end(), [](const int &a, const int &b) {
        return skyline[a] < skyline[b];
      });
    partitionSkylineRTrees.emplace_back(RTree<int, data>());
    for(auto &p : partition){
      partitionSkylineRTrees.back().Insert(skyline[p].point.data(), skyline[p].point.data(), p);
    }
  }

}

void pickAlgorithm(vector<int> partitionSkyline, RTree<int, data> partitionSkylineRTree){
    switch (algorithm) {
    case MAXIMAL_2D:
      maximalSweepline(partitionSkyline, partitionSkylineRTree);
      break;
    case MAXIMAL_CLIQUE_ENUM:
      maximalCliqueEnumeration(partitionSkyline, partitionSkylineRTree);
      break;
    case APPROX_CS:
        approxCS(partitionSkyline, partitionSkylineRTree);
        break;
    case APPROX_MCS:
        approxMCS(partitionSkyline, partitionSkylineRTree);
        break;
    case BK:
        bk(partitionSkyline, partitionSkylineRTree);
        break;
    case MAXIMAL_BRUTEFORCE:
      maximalBruteforce(partitionSkyline, partitionSkylineRTree);
      break;
    case CLIQUE_ENUM:
        cliqueEnumeration(partitionSkyline, partitionSkylineRTree);
        break;
    case TSUKIYAMA:
    //TODO: modify tsukiyama
        maximalCliqueEnumeration(partitionSkyline, partitionSkylineRTree);
        break;
    default:
      assert(false);
    }
  }


/* calls right algorithm */
void callAlgorithm() {
  CompSky.clear(); /* clearing precomputed competitive skyline */
  vector<int> skyline_ids(SKY_CARDINALITY);
  std::iota(skyline_ids.begin(), skyline_ids.end(), 0);
  partitionSkylines.clear();
  partitionSkylineRTrees.clear();

  partitionSkylines.emplace_back(skyline_ids);
  if(PARTITION){
  partitionSkyline();
  cout << "\n#Partitions: " << partitionSkylines.size() << endl;
  }
  else{
    partitionSkylineRTrees.emplace_back(skylineRTree);
  }
  for(int i = 0; i < partitionSkylines.size(); i++)
    pickAlgorithm(partitionSkylines[i], partitionSkylineRTrees[i]);
}

#endif // COMP_SKY_ALGORITHMS_HPP
