//
// Created by Jayitha on 17/05/22.
//

#ifndef COMP_SKY_RUNS_HPP
#define COMP_SKY_RUNS_HPP

#include "global.hpp"
#include "algorithms/algorithms.hpp"
#include "DBinterface.hpp"

void exportMetrics(){
    if(EXPORT_METRICS){
        exportCSSizes();
        exportcsProperties();
    }

    if(EXPORT_COMPETITIVE_SETS)
        exportCS();

    if(EXPORT_TIME_METRICS)
        exportTimeStat();
}

void basicRun(){
    // if(checkMetrics()){
    //     cout << "Metrics already computed or not needed" << endl;
    //     return;
    // }
    cout << "##################### " << DATASET << " " << DIMENSIONALITY << " "
       << DATA_CARDINALITY << " " << SKY_CARDINALITY << " " << redomain(EPSILON)
       << " #####################" << endl;
    cout << "Computing competitive skyline... " << flush;
    if(EXPORT_TIME_METRICS){
        timeStat = 0;
        for(int i = 0; i < NUM_ITERS; i++){
              auto start_time_scan = chrono::high_resolution_clock::now();
            callAlgorithm();
            auto end_time_scan = std::chrono::high_resolution_clock::now();
            auto duration_scan = end_time_scan - start_time_scan;

            double newDur = chrono::duration_cast<std::chrono::nanoseconds>(duration_scan).count() *1e-9;
            timeStat += newDur/NUM_ITERS;
        }
    } else {
        callAlgorithm();
    }
    cout << "Done." << endl;
    cout << "Size: " << CompSky.size() << endl;
    exportMetrics();
}

void varyAlgorithm(){
    if(!VARY_ALGORITHMS || DIMENSIONALITY != 2 || VARY_DIMENSIONALITY){
        basicRun();
        return;
    }
    if(DIMENSIONALITY == 2){
        algorithm = MAXIMAL_2D;
        basicRun();
        cout<<"ALG: " << algToStr[algorithm] << endl;
        algorithm = MAXIMAL_CLIQUE_ENUM;
        basicRun();
        cout<<"ALG: " << algToStr[algorithm] << endl;
    }
}

void varyEps(vector<double> epsilons = eps){
    loadSkyline();

    if(!EXPORT_COMPETITIVE_SETS && !EXPORT_OVERLAP_METRICS && !EXPORT_TIME_METRICS && !EXPORT_METRICS){
        return;
    }

    if(!VARY_EPSILON){
        varyAlgorithm();
        return;
    }
    for(double epsilon : epsilons){
        EPSILON = domain(epsilon);
        varyAlgorithm();
    }
}
void varyCard(vector<unsigned int> cards = ns){
    if(!VARY_CARDINALITY){
        cout << "n = " << DATA_CARDINALITY << endl;
        varyEps();
        return;
    }
    for(auto &card: cards){
        DATA_CARDINALITY = card;
        cout << "n = " << DATA_CARDINALITY << endl;
        varyEps();
    }
}
void varyDim(vector<unsigned int> dims = ds){
    if(!VARY_DIMENSIONALITY){
        cout << "d = " << DIMENSIONALITY << endl;
        varyCard();
        return;
    }
    for(auto &d : dims){
        DIMENSIONALITY = d;
        cout << "d = " << DIMENSIONALITY << endl;
        NUMDIMS = DIMENSIONALITY;
        varyCard();
    }
}

void run(){
    cout << "Dataset = " << DATASET << endl;
    varyDim();
}

#endif // COMP_SKY_RUNS_HPP