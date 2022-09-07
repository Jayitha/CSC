#ifndef BK_HPP
#define BK_HPP

#include "pointSet.hpp"

void ProcMCE(PointSet C, PointSet T, PointSet D)
{
#ifdef DEBUG
    if(!C.empty())
        cout << "C: " << C << " T: " << T << " D: " << D << endl;
#endif
    // if (T.empty() || C ^ T)
    // {
    //     if (!C.empty() && D.empty()){
    //         PointSet new_mcs = PointSet::unionPointSet(C, T);
    //         CompSky.push_back(new_mcs);
    //         return;
    //     }
    //     duplicates++;
    //     return;
    // }

    // if (T ^ D)
    // {
    //     return;
    //     duplicates++;
    // }

    // if(T.empty()){
    //     if(D.empty())
    //         CompSky.push_back(C);
    //     else{
    //         duplicates++;
    //         return;
    //     }
    // }

    // if((!D.empty()) && D^T ){
    //     duplicates++;
    //     return;
    // }

    // if(!C.empty() && C^T){
    //     PointSet new_cand = PointSet::unionPointSet(C, T);
    //     if(!(new_cand ^= D))
    //         CompSky.push_back(new_cand);
    //     else{
    //         duplicates++;
    //         return;
    //     }
    // }

    if(T.empty() && D.empty()){
        if(!CompSky.empty() && CompSky.size() % 100 == 0)
            cout << "Found " << CompSky.size() << " cliques" << endl;

        bool subsumed = false, subsumes = false;
        for(auto &mcs : CompSky){
            if(mcs.subsumes(C)){
                subsumed = true;
                break;
            }
            if(C.subsumes(mcs)){
                subsumed = true;
                break;
            }
            if(subsumed && subsumes){
                break;
            }
        }

        if(subsumed)
            cout << "Not maximal!" << endl;
        if(subsumes)
            cout << "Subsumes non-maximal!" << endl;
        // cout << C << endl;
        CompSky.push_back(C);
        return;
    }

    if(T.empty()){
        duplicates++;
        return;
    }


    int u = T.first();
    PointSet candidates = T.nonneighboursof(u) + u;
#ifdef DEBUG
    cout << skyline[u].id << endl;
    cout << candidates << endl;
#endif
    for (auto p : candidates.point_idx)
    {
        T -= p;
        ProcMCE(C + p, T.neighboursof(p), D.neighboursof(p));
        D += p;
    }
}

void bk(vector<int> & partitionSkyline, RTree<int, data> &partitionSkylineRTree)
{
    cout << "BK" << endl;
    ProcMCE(PointSet(), PointSet(partitionSkyline), PointSet());
}

#endif // BK_HPP
