// //
// // Created by Jayitha on 12/04/22.
// //

// #ifndef COMP_SKY_BBSET_HPP
// #define COMP_SKY_BBSET_HPP

// #include "boundingBox.hpp"
// #include "point.hpp"

// /**
//  * @brief A set of bounding boxes.
//  *
//  */
// class BBSet {
// public:
//   vector<BB> bbs;
//   BB mbb;

//   BBSet() {
//     this->bbs.clear();
//     this->mbb = BB();
//   }

//   /* set of bbs */
//   BBSet(const vector<BB> &bbs) {
//     this->bbs = bbs;
//     sort(this->bbs.begin(), this->bbs.end());
//     this->mbb = BB(bbs);
//   }

//   /* cardinality of set */
//   int size() const { return this->bbs.size(); }

//   /* size of mbb */
//   data mbbSize() const { return this->mbb.size(); }

//   /* competitiveness check */
//   bool isCompetitive() const {
//     if (this->mbbSize() <= EPSILON)
//       return true;
//     return false;
//   }

//   /* add BB to set */
//   void operator+=(const BB &bb) {

//     /* if point is to be emplaced back */
//     if(this->size() && this->bbs.back() < bb) {
//       this->bbs.emplace_back(bb);
//       this->mbb += bb;
//       return;
//     }

//     auto equalorgreater =
//         lower_bound(this->bbs.begin(), this->bbs.end(), bb);

//     /* if bb already in vector, don't add point */
//     if (equalorgreater != this->bbs.end() && (*equalorgreater) == bb)
//       return;

//     this->bbs.insert(upper_bound(this->bbs.begin(), this->bbs.end(), bb), bb);
//     this->mbb += bb;
//   }

//   /* checks if bb is subsumed by some bb in boxset */
//   bool subsumes(const BB &bb) {
//     if (!this->mbb.contains(bb))
//       return false;
//     for (int i = 0; i < this->size(); i++) {
//       if (this->bbs[i].contains(bb))
//         return true;
//     }
//     return false;
//   }

//   /* returns a set of all bbs that are contained in bb */
//   /* 
//   * note that this is differenct from the range query in the R-Tree which is
//   * returns the set of all bbs that intersect bb.
//   */
//   BBSet range(const BB &bb) {
//     BBSet rangeResults;
//     for (int i = 0; i < this->size(); i++) {
//       if (bb.contains(this->bbs[i]))
//         rangeResults += this->bbs[i];
//     }
//     return rangeResults;
//   }

//   friend ostream &operator<<(ostream &os, BBSet bbset);
// };

// ostream &operator<<(ostream &os, BBSet bbset) {
//   os << "BB Set: " << bbset.size();
//   for (int i = 0; i < bbset.size(); i++)
//     os << endl << bbset.bbs[i];
//   return os;
// }

// #endif // COMP_SKY_BBSET_HPP
