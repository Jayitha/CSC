//
// Created by Jayitha on 12/04/22.
//

#ifndef TSUKIYAMA_HPP
#define TSUKIYAMA_HPP

/**
 * @file tsukiyama.hpp
 * @author Jayitha (jayithareddyp@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-22
 * 
 * @copyright Copyright (c) 2022
 * 
 * The TSUKIYAMA algorithm is way too slow I suspect this is because it's generating multiple duplicates. 
 */
#include "pointSet.hpp"
#include "omp.h"

void generateChildren(PointSet, int);

void satisfiesConditionsBCD(PointSet pmcs, int i)
{
    // condition (b)
    if (pmcs.contains(i))
    {
#ifdef DEBUG
        cout << "Condition (b) is not satisfied" << endl;
#endif

        return;
    }

#ifdef DEBUG
    cout << "K = " << pmcs << endl;
    cout << "i = " << skyline[i].id << endl;
    cout << "K[i] = K[i-1] = " << pmcs.slice(i) << endl;
    cout << "N(i) = " << neighboursof(i) << endl;
    cout << "K[i] cap N(i) = " << pmcs.slice(i).neighboursof(i) << endl;
    cout << "C(K[i] cap N(i)) = " << pmcs.slice(i).neighboursof(i).closure() << endl;
    cout << "C(K[i] cap N(i))[i]" << pmcs.slice(i).neighboursof(i).closure().slice(i) << endl;
    cout << "K' = C(K[i] cap N(i) + i) = " << (pmcs.slice(i).neighboursof(i) + i).closure() << endl;
#endif

    /**
     * condition (c)
     * K'_{<= i-1} = K_{<= i} \cap N(i) */
    PointSet i_neighbours = pmcs.slice(i).neighboursof(i);
    PointSet K_child = (i_neighbours + i).closure();
    if (!(K_child.slice(i - 1) == i_neighbours))
    {
        duplicates++;
#ifdef DEBUG
        cout << "(c) not satisfied" << endl;
        cout << "(c) K'[i-1] = K[i] cap N(i)" << endl;
        cout << K_child.slice(i - 1) << " != " << i_neighbours << endl;
#endif
        return;
    }

    /**
     * condition (d)
     * K_{<= i} = C(K_{<= i} \cap N(i))_{<= i}
     */
    if (!(pmcs.slice(i) == i_neighbours.closure().slice(i)))
    {
        duplicates++;
#ifdef DEBUG
        cout << "(d) not satisfied" << endl;
        cout << "(d) K[i] = C(K[i] cap N(i))[i]" << endl;
        cout << pmcs.slice(i) << " != " << i_neighbours.closure().slice(i) << endl;
#endif
        return;
    }

    generateChildren(K_child, i);
}

void generateChildren(PointSet pmcs, int core_pmcs)
{
#ifdef DEBUG
    cout << "pmcs: " << pmcs << endl;
    press_enter_to_continue();
#endif
    CompSky.emplace_back(pmcs);

    if (core_pmcs == skyline.size() - 1)
        return;

    // if(core_pmcs == 0){
    if (1){
    // condition (a)
    #pragma omp parallel for
    for (int i = core_pmcs + 1; i < skyline.size(); i++)
    {
#ifdef DEBUG
        cout << "i = " << skyline[i].id << endl;
#endif
        
        satisfiesConditionsBCD(pmcs, i);
        
    }
    }

    else{
        auto candidates = neighboursofsome(pmcs).point_idx;
        for(auto i : candidates){
            satisfiesConditionsBCD(pmcs, i);
        }
    }
}
void tsukiyama()
{
    cout << "Computing tsukiyama ..." << endl;
    generateChildren(PointSet::K0(), 0);
}


#endif // TSUKIYAMA_HPP
