//
// Created by Jayitha on 18/04/22.
//

#ifndef COMP_SKY_MAXCLIQUEENUM_HPP
#define COMP_SKY_MAXCLIQUEENUM_HPP

// #include "bbSet.hpp"
#include "boundingBox.hpp"
#include "point.hpp"
#include "pointSet.hpp"

// void maximalCliqueEnumeration(vector<int> &part_skyline, RTree<int, data> &part_skylineRtree) {

//   RTree<BB, data> MCSRTree;
//   int num_thousands = 0;
//   int num_sets = 0;
//   int num_points = 0;

//   for (auto &p : part_skyline) {
//     num_points++;
//     if(num_sets / 1000 > num_thousands) {
//       num_thousands = num_sets / 1000;
//       cout << "Processed " << num_thousands << " sets and " << num_points << " points." << endl;
//     }

//     BB crange = BB::crange(p);

//     vector<BB> fullSets;
//     RTree<BB, data> partialSetsRtree;
//     bool partialSetsEmpty = true;

//     vector<BB> candidates =
//         MCSRTree.Range(crange.lb.point.data(), crange.ub.point.data());

//     for (auto &cs : candidates) {

//       /* if fully competitive */
//       if (cs ^ p) {
//         MCSRTree.Remove(cs.lb.point.data(), cs.ub.point.data(), cs);
//         cs += p;
//         MCSRTree.Insert(cs.lb.point.data(), cs.ub.point.data(), cs);
//         fullSets.emplace_back(cs);
//       }
//       /* if partially competitive */
//       else if (cs ^= p) {
//         BB intersection = BB::intersectionBB(crange, cs);
//         PointSet new_cs1 = PointSet(skylineRtree.Subsumed(
//             intersection.lb.point.data(), intersection.ub.point.data()));
//         PointSet new_cs = range(intersection);
//         if(!(new_cs1 == new_cs)){
//           cout << "ERROR: Problem with range query!" << endl;
//           cout << intersection << endl;
//           cout << new_cs1 << endl;
//           cout << new_cs << endl;
//         }

//         if (new_cs.empty())
//           continue;
//         new_cs += p;
//         partialSetsRtree.Insert(new_cs.mbb.lb.point.data(),
//                                 new_cs.mbb.ub.point.data(), new_cs.mbb);
//         partialSetsEmpty = false;
//       }
//     }

//     /* if p is not competiitve with any point seen so far */
//     if (partialSetsEmpty && fullSets.empty()) {
//       MCSRTree.Insert(skyline[p].point.data(), skyline[p].point.data(), BB(p));
//       num_sets++;
//       continue;
//     }

//     /* if only full sets exist, no full set subsumes any other */
//     if (partialSetsEmpty) {
//       continue;
//     }

//     /* remove all partial sets subsumed by some full sets */
//     for (auto &mcs : fullSets) {
//       vector<BB> subsumed_cs =
//           partialSetsRtree.Subsumed(mcs.lb.point.data(), mcs.ub.point.data());
//       for (auto &subcs : subsumed_cs)
//         partialSetsRtree.Remove(subcs.lb.point.data(), subcs.ub.point.data(),
//                                 subcs);
//     }

//     /* remove partial sets subsumed by some partial set */
//     vector<BB> partialSets =
//         partialSetsRtree.Range(Point(domain(0)).point.data(), Point(domain(1)).point.data());
//     for (auto &cs : partialSets) {
//       if (!partialSetsRtree.Search(cs.lb.point.data(), cs.ub.point.data(),
//                                    [&cs](BB bb) -> bool { return (bb == cs); }))
//         continue;
//       vector<BB> subsumed_cs =
//           partialSetsRtree.Subsumed(cs.lb.point.data(), cs.ub.point.data());
//       for (auto &subcs : subsumed_cs)
//         partialSetsRtree.Remove(subcs.lb.point.data(), subcs.ub.point.data(),
//                                 subcs);
//       partialSetsRtree.Insert(cs.lb.point.data(), cs.ub.point.data(), cs);
//     }

//     /* insert all non-subsumed partial sets */
//     partialSets.clear();
//     partialSets =
//         partialSetsRtree.Range(Point(domain(0)).point.data(), Point(domain(1)).point.data());
//     for (auto &mcs : partialSets){
//       num_sets++;
//       MCSRTree.Insert(mcs.lb.point.data(), mcs.ub.point.data(), mcs);
//     }
//   }

//   /* all maximal sets found -> convert BBs to point sets */
//   vector<BB> MCSBBs =
//       MCSRTree.Range(Point(domain(0)).point.data(), Point(domain(1)).point.data());
//   for (auto &mcs : MCSBBs) {
//     CompSky.emplace_back(
//         PointSet(part_skylineRtree.Range(mcs.lb.point.data(), mcs.ub.point.data())));
//   }
// }

void maximalCliqueEnumeration(vector<int> &part_skyline, RTree<int, data> &part_skylineRtree) {

  RTree<BB, data> MCSRTree;
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

    bool inserted = false;
    vector<BB> partialSets;

    vector<BB> candidates =
        MCSRTree.Range(crange.lb.point.data(), crange.ub.point.data());

    for (auto &cs : candidates) {

      /* if fully competitive */
      if (cs ^ p) {
        MCSRTree.Remove(cs.lb.point.data(), cs.ub.point.data(), cs);
        cs += p;
        MCSRTree.Insert(cs.lb.point.data(), cs.ub.point.data(), cs);
        inserted = true;
      }
      /* if partially competitive */
      else if (cs ^= p) {
        BB intersection = BB::intersectionBB(crange, cs);
        PointSet new_cs = range(intersection, part_skyline);
        if (new_cs.empty())
          continue;
        new_cs += p;
        if(new_cs.size() == range(BB::crange(new_cs.mbb), part_skyline, p).size()){
          partialSets.emplace_back(new_cs.mbb);
        }
        inserted = true;
      }
    }

    /* if p is not competiitve with any point seen so far */
    if (!inserted) {
      MCSRTree.Insert(skyline[p].point.data(), skyline[p].point.data(), BB(p));
      num_sets++;
      continue;
    }

    /* if only full sets exist, no full set subsumes any other */
    if (partialSets.empty()) {
      continue;
    }

    sort(partialSets.begin(), partialSets.end());
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
      MCSRTree.Insert(mcs.lb.point.data(), mcs.ub.point.data(), mcs);
    }
  }

  /* all maximal sets found -> convert BBs to point sets */
  vector<BB> MCSBBs =
      MCSRTree.Range(Point(domain(0)).point.data(), Point(domain(1)).point.data());
  for (auto &mcs : MCSBBs) {
    CompSky.emplace_back(
        PointSet(part_skylineRtree.Range(mcs.lb.point.data(), mcs.ub.point.data())));
  }
}

void maximalCliqueEnumeration(){
    vector<int> skyline_ids(SKY_CARDINALITY);
    std::iota(skyline_ids.begin(), skyline_ids.end(), 0);
    maximalCliqueEnumeration(skyline_ids, skylineRTree);
}
#endif // COMP_SKY_MAXCLIQUEENUM_HPP
