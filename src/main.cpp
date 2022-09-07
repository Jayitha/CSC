//
// Created by Jayitha on 12/04/22.
//

#include "algorithms/partitioning.hpp"
#include "config.hpp"
#include "runs.hpp"

int main(int argc, char* argv[]) {
    loadParams();
    // vector<string> datasets_IDs = {"CORR", "INDEP", "ANTI"};
    // for (auto di : datasets_IDs) {
    //     DATASET = di;

    //     for (auto d : ds) {
    //         DIMENSIONALITY = d;

    //         if (DIMENSIONALITY == 4) {
    //             for (auto n : ns) {
    //                 DATA_CARDINALITY = n;
    //                 loadSkyline();

    //                 if (DATA_CARDINALITY == 10000) {
    //                     for (auto e : eps) {
    //                         EPSILON = domain(e);
    //                         partition();
    //                         exportPartitionProperties();
    //                     }
    //                 } else {
    //                     EPSILON = domain(0.05);
    //                     partition();
    //                     exportPartitionProperties();
    //                 }
    //             }
    //         } else {
    //             DATA_CARDINALITY = 10000;
    //             EPSILON = domain(0.05);
    //             loadSkyline();
    //             partition();
    //             exportPartitionProperties();
    //         }
    //     }
    // }
    // loadSkyline();
    // partition();
    // exportPartitionProperties();
    run();

    // DATASET = "INDEP";
    // DATA_CARDINALITY = 10000;
    // DIMENSIONALITY = 4;
    // EPSILON = domain(0.05);
    // loadSkyline();
    // partition();

}