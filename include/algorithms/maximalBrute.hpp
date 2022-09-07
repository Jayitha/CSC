//
// Created by Jayitha on 19/04/22.
//

#ifndef COMP_SKY_MAXIMALBRUTE_HPP
#define COMP_SKY_MAXIMALBRUTE_HPP

#include "boundingBox.hpp"
#include "point.hpp"
#include "pointSet.hpp"

void maximalBruteforce(vector<int> &part_skyline, RTree<int, data> &part_skylineRtree) {

  int num_thousands = 0;
  int num_sets = 0;
  int num_points = 0;

  for (int i = 0; i < part_skyline.size(); i++) {
    int p = part_skyline[i];
    num_points++;
    if(num_sets / 1000 > num_thousands) {
      num_thousands = num_sets / 1000;
      cout << "Processed " << num_thousands << " sets and " << num_points << " points." << endl;
    }

    bool inserted = false;
    vector<PointSet> partialSets;

    for (auto &cs : CompSky) {

      /* if fully competitive */
      bool partiallyCompetitive = false;
      bool fullCompetitive = true;

      for(auto &p1 : cs.point_idx){
        if(skyline[p1] ^ skyline[p]){
            partiallyCompetitive = true;
        } else {
            fullCompetitive = false;
        }

        if(partiallyCompetitive && !fullCompetitive)
            break;
      }

      if(fullCompetitive){
        cs += p;
        inserted = true;
      }

      /* if partially competitive */
      else if (partiallyCompetitive) {
        PointSet new_cs;

        for(auto &p1 : cs.point_idx)
            if(skyline[p1]^skyline[p])
                new_cs += p1;

        new_cs += p;

        bool maximal = true;
        for(int j = 0; j < i; j++){
            int p1 = part_skyline[j];
                bool comp = true;
                for(auto &p2 : new_cs.point_idx){
                    if(!(skyline[p1]^skyline[p2]))
                        comp = false;
                }
                if(comp && new_cs.point_idx.find(p1) == new_cs.point_idx.end()){
                    maximal = false;
                    break;
                }
        }

        if(maximal){
          partialSets.emplace_back(new_cs);
        }
        inserted = true;
      }
    }

    /* if p is not competiitve with any point seen so far */
    if (!inserted) {
        CompSky.emplace_back(PointSet(p));
      num_sets++;
      continue;
    }

    /* if only full sets exist, no full set subsumes any other */
    if (partialSets.empty()) {
      continue;
    }

    sort(partialSets.begin(), partialSets.end(), [](const PointSet &a, const PointSet &b) -> bool
    {
        return a.mbb < b.mbb;
    });
    int itr = 0;
    while(itr < partialSets.size() - 1){
      if(partialSets[itr] == partialSets[itr + 1]){
        partialSets.erase(partialSets.begin() + itr + 1);
      }
      else{
        itr++;
      }
    }
    
    for (auto &mcs : partialSets){
      num_sets++;
      CompSky.emplace_back(mcs);
    }
  }
}

void maximalBruteforce(){
    vector<int> skyline_ids(SKY_CARDINALITY);
    std::iota(skyline_ids.begin(), skyline_ids.end(), 0);
    maximalBruteforce(skyline_ids, skylineRTree);
}
#endif // COMP_SKY_MAXIMALBRUTE_HPP
