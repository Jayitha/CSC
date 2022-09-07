//
// Created by Jayitha on 15/04/22.
//

#ifndef COMP_SKY_METRICS_HPP
#define COMP_SKY_METRICS_HPP

#include "pointSet.hpp"

// class Coeff {
// public:
//   double max;
//   double min;
//   double avg;
//   double var;
//   double stdev;

//   Coeff() : max(0), min(1), avg(0), var(0), stdev(0) {}

//   void update(double val, int k) {
//     this->max = std::max(this->max, val);
//     this->min = std::min(this->min, val);
//     float old_avg = this->avg;
//     this->avg += (val - this->avg) / k;
//     this->var +=
//         (val - old_avg) *
//         (val -
//          this->avg); /* See: https://www.johndcook.com/blog/standard_deviation/
//                       */
//   }

//   void finalize(int k) {

//     if(k != CompSky.size() - 1){
//       this->max = std::max(this->max, 0.0);
//       this->min = std::min(this->min, 0.0);
//       double old_avg = this->avg;
//       this->avg = (k * this->avg)/(CompSky.size() - 1);
//       this->var = this->var + k*(pow(old_avg, 2)) - (CompSky.size()*pow(this->avg, 2));
//       k = CompSky.size() - 1;
//     }
//     /* if var != 0 => k != 0 */
//     if (this->var) {
//       this->var /= k;
//       this->stdev = sqrt(this->var);
//     }

//     /* only one competitive set exists */
//     if (this->min == 1 && this->max == 0)
//       this->min = 0;
//   }

//   void singleton(){
//     this->max = 0;
//     this->min = 0;
//     this->avg = 0;
//     this->var = 0;
//     this->stdev = 0;
//   }
// };

// ostream& operator<<(ostream& os, const Coeff& c) {
//   os << "(" << c.max << ", " << c.avg << ", " << c.min << ", " << c.var << ", " << c.stdev << ")";
//   return os;
// }

// class SimilarityCoefficients {
// public:
//   Coeff overlap_size; /* intersection (A.mbb , B.mbb) */
//   Coeff jaccard;      /* |intersection (A, B)| / |union (A, B)| */
//   Coeff sorenson;     /* 2 * |intersection (A, B)| / (|A| + |B|) */
//   Coeff overlap;      /* |intersection (A, B)| / min(|A|, |B|) */
//   int k; /* notation used to indicate number of elements used so far to compute
//             mean and stdev */

//   SimilarityCoefficients()
//       : overlap_size(), jaccard(), sorenson(), overlap(), k(0) {}

//   void update(double overlapSize_val, double jaccard_val, double sorenson_val,
//               double overlap_val) {
//     this->k++;
//     this->overlap_size.update(overlapSize_val, k);
//     this->jaccard.update(jaccard_val, k);
//     this->sorenson.update(sorenson_val, k);
//     this->overlap.update(overlap_val, k);
//   }

//   void finalize() {
//     this->overlap_size.finalize(k);
//     this->jaccard.finalize(k);
//     this->sorenson.finalize(k);
//     this->overlap.finalize(k);
//   }

//   void singleton(){
//     this->overlap_size.singleton();
//     this->jaccard.singleton();
//     this->sorenson.singleton();
//     this->overlap.singleton();
//   }
// };

// ostream& operator<<(ostream& os, const SimilarityCoefficients& sc) {
//   os << "OS: " << sc.overlap_size << endl;
//   os << "JC: " << sc.jaccard << endl;
//   os << "SC: " << sc.sorenson << endl;
//   os << "OC: " << sc.overlap;
//   return os;
// }

// vector<SimilarityCoefficients> overlapCoeffs;

// ostream& operator<<(ostream& os, const vector<SimilarityCoefficients>& v) {
//   for(int i = 0; i < v.size(); i++){
//     os << "cs_ID: " << i + 1 << endl;
//     os << v[i];
//   }
//   return os;
// }

vector<double> overlap_coefficient;

void generateOverlapMetrics() {
#ifdef DEBUG
  cout << "Generating overlap metrics..." << endl;
#endif
  /* verify CompSky has been computed */
  assert(!CompSky.empty());

  int numSets = CompSky.size();
  overlap_coefficient.assign(numSets, 0);

  /* If singleton then no need to compute overlap coefficients */
  bool allSingletons = true;
  for(int i = 0; i < CompSky.size(); i++) {
    if(CompSky[i].size() > 1) {
      allSingletons = false;
      break;
    }
  }

  if(allSingletons){
    cout << "All sets are singletons..." << flush;
    return;
  }

  /* In RTree store cs with ID */
  class csWithID{
    public:
      ID id;
      BB mbb;
      csWithID(int id, BB mbb) : id(id), mbb(mbb) {}
      csWithID(int id, PointSet cs) : id(id), mbb(cs.mbb) {}
      csWithID() : id(0), mbb(BB()) {}
  };

  RTree<csWithID, data> CSTree;

  for (int i = 0; i < CompSky.size(); i++) {
    if(i and (i+1)%500 == 0)
      cout << "Processed " << i+1 << " sets..." << endl;

    /* competitive sets seen till now that intersect with current set */
    vector<csWithID> inter_sets = CSTree.Range(CompSky[i].mbb.lb.point.data(), CompSky[i].mbb.ub.point.data());

    #pragma omp parallel for
    for(int j = 0; j < inter_sets.size(); j++) {
      int AinterB =
          PointSet::intersectionPointSet(CompSky[i], CompSky[inter_sets[j].id]).size();
      

      double coeff = (double)AinterB/(double)CompSky[i].size();

      #pragma omp critical
      {
      overlap_coefficient[i] = coeff > overlap_coefficient[i] ? coeff : overlap_coefficient[i];
      }

      coeff = (double)AinterB/(double)CompSky[inter_sets[j].id].size();

      #pragma omp critical
      {
      overlap_coefficient[inter_sets[j].id] = coeff > overlap_coefficient[inter_sets[j].id] ? coeff : overlap_coefficient[inter_sets[j].id];
      }
    }
    
    /* Finally, Insert current set into the tree */
    CSTree.Insert(CompSky[i].mbb.lb.point.data(), CompSky[i].mbb.ub.point.data(), csWithID(i, CompSky[i]));
  }
}


#endif // COMP_SKY_METRICS_HPP
