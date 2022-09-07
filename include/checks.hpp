// //
// // Created by Jayitha on 17/05/22.
// //

// #ifndef COMP_SKY_CHECKS_HPP
// #define COMP_SKY_CHECKS_HPP

// #include "global.hpp"
// #include "pointSet.hpp"

// /**
//  * @brief partially checks for normal competitive skyline
//  *
//  * @param comp_sky 
//  * @return true 
//  * @return false 
//  */
// bool checkCompSky(vector<PointSet> comp_sky = CompSky) {

//   cout << "Checking if competitive skyilne (partial): " << endl;
//   /* check if mbbs match set */
//   for (auto &cs : comp_sky) {
//     for (auto &p : cs.points)
//       if (!cs.mbb.contains(p)) {
//         cout << "FAILED: [1/3]: mbb of set does not match set: " << cs.mbb << " and p: " << p << endl;
//         return false;
//       }
//   }
//   cout << "PASSED: [1/3]: mbb of set matches set" << endl;

//   /* check to see if every cs is competitive */
//   for (auto &cs : comp_sky) {
//     if (!cs.isCompetitive()) {
//       cout << "FAILED: [2/3]: cs is not competitive: " << cs << endl;
//       return false;
//     }
//   }
//   cout << "PASSED: [2/3]: every cs is competitive" << endl;

//   /* check if every skyline point appears in at least one cs */
//   for (auto &p : skyline.points) {
//     bool contained = false;
//     for (auto &cs : comp_sky) {
//       if (cs.contains(p)) {
//         contained = true;
//         break;
//       }
//     }
//     if (!contained) {
//       cout << "FAILED: [3/3]: skyline point not in any set: " << p << endl;
//       return false;
//     }
//   }
//   cout << "PASSED: [3/3]: every skyline point belongs to SOME set" << endl;
//   return true;
// }

// /**
//  * @brief checks if cs is closed using the closure operator. A competitive set
//  * cs is closed iff cs = closure(cs) 
//  *
//  * cs = skyline.range(cs.mbb)) 
//  *
//  * @param cs 
//  * @return true 
//  * @return false 
//  */
// bool isClosed(PointSet cs){
//   PointSet closedcs = skyline.range(cs.mbb);
//   if (closedcs.size() != cs.size() || closedcs.points != cs.points) {
//     return false;
//   }
//   return true;
// }

// /**
//  * @brief partially checks if competitive skyline is closed by ensuring each
//  * competitive skyline is closed
//  *
//  * @param comp_sky 
//  * @return true 
//  * @return false 
//  */
// bool checkClosedCompSky(vector<PointSet> comp_sky = CompSky) {

//   if (!checkCompSky())
//     return false;

//     cout << "Checking if competitive skyilne (partial) is CLOSED: " << endl;

//   /* checks to make sure each set is closed */
//   for (auto &cs : comp_sky) {
//     PointSet closedcs = skyline.range(cs.mbb);
//     if (!isClosed(cs)) {
//         cout << "FAILED: [1/1]: cs is not closed: " << cs << endl;
//       return false;
//     }
//   }

//   cout << "PASSED: [1/1]: all competitive sets are closed" << endl;
//   return true;
// }

// /**
//  * @brief checks if cs is maximal. If a cs is maximal, then it cannot be
//  * extended. Therefore, the crange should be the same as the set itself. 
//  *
//  * @param cs 
//  * @return true 
//  * @return false 
//  */
// bool isMaximal(PointSet cs){
//   BB crange = BB::crange(cs.mbb);
//   PointSet maxcs = skyline.range(crange);
//   if (maxcs.size() != cs.size() || maxcs.points != cs.points) {
//     return false;
//   }
//   return true;
// }

// /**
//  * @brief partially checks if competitive skyline is maximal by ensuring each
//  * competitive skyline is maximal
//  *
//  * @param comp_sky 
//  * @return true 
//  * @return false 
//  */
// bool checkMaxCompSky(vector<PointSet> comp_sky = CompSky) {

//   if (!checkCompSky())
//     return false;

//     cout << "Checking if competitive skyilne (partial) is MAXIMAL: " << endl;

//   /* check if some set is subsumed */
//   for (int i = 0; i < comp_sky.size(); i++) {
//     for (int j = 0; j < comp_sky.size(); j++) {
//       if (i == j)
//         continue;
//       if (comp_sky[j].subsumes(comp_sky[i])) {
//           cout << "FAILED: [1/2]: cs subsumed: " << comp_sky[j] << " subsumes " << comp_sky[i] << endl;
//         return false;
//       }
//     }
//   }
//     cout << "PASSED: [1/2]: no cs is subsumed" << endl;

//   /* check to make sure every set is maximal */
//   for(int i = 0; i < comp_sky.size(); i++){
//     if (!isMaximal(comp_sky[i])) {
//         cout << "FAILED: [2/2]: cs is not maximal: " << comp_sky[i] << endl;
//       return false;
//     }
//   }
//   cout << "PASSED: [2/2]: all competitive sets are maximal" << endl;
//   return true;
// }


// vector<PointSet> split_dataset() {
//   vector<PointSet> point_sets;
//   point_sets.emplace_back(PointSet());
//   point_sets.back().emplace_back(skyline.points[0]);
//   for (int i = 1; i < skyline.points.size(); i++) {
//     if (abs(skyline.points[i].point[SORT_DIMENSION] -
//             skyline.points[i - 1].point[SORT_DIMENSION]) <= EPSILON)
//       point_sets.back().emplace_back(skyline.points[i]);
//     else {
//       point_sets.emplace_back(PointSet());
//       point_sets.back().emplace_back(skyline.points[i]);
//     }
//   }
//   return point_sets;
// }

// void show_comp_graph() {
//   for (int i = 0; i < skyline.size(); i++) {
//     for (int j = i + 1; j < skyline.size(); j++) {
//       if (skyline.points[i] ^ skyline.points[j]) {
//         cout << skyline.points[i].id << " ---|"
//              << skyline.points[i] - skyline.points[j] << "| "
//              << skyline.points[j].id << endl;
//       }
//     }
//   }
// }

// void compare_comp_skys(vector<PointSet> comp_sky1, vector<PointSet> comp_sky2){
//   bool allequal = true;
//   int itr1 = 0, itr2 = 0;

//   while(itr1 < comp_sky1.size() && itr2 < comp_sky2.size()){

//     if(comp_sky1[itr1] == comp_sky2[itr2]){
//       itr1++;
//       itr2++;
//     } 

//     else if (comp_sky1[itr1] < comp_sky2[itr2]){
//       cout << "[1]: (" << itr1 << ") " << comp_sky1[itr1] << endl;
//       itr1++;
//       allequal = false;
//     }

//     else {
//       cout << "[2]: (" << itr1 << ") "<< comp_sky2[itr2] << endl;
//       itr2++;
//       allequal = false;
//     }
//   }

//   if(itr1 < comp_sky1.size()){
//     for(; itr1 < comp_sky1.size(); itr1++){
//       cout << "[1]: " << comp_sky1[itr1] << endl;
//     }
//     allequal = false;
//   }

//   if(itr2 < comp_sky2.size()){
//     for(; itr2 < comp_sky2.size(); itr2++){
//       cout << "[2]: " << comp_sky2[itr2] << endl;
//     }
//     allequal = false;
//   }

//   if(allequal)
//     cout << "All equal" << endl;
// }



// #endif // COMP_SKY_CHECKS_HPP