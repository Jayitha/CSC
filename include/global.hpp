//
// Created by Jayitha on 12/04/22.
//

#ifndef COMP_SKY_GLOBAL_HPP
#define COMP_SKY_GLOBAL_HPP

#include "/usr/local/opt/libomp/include/omp.h"
#include "RTree.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>
#include <yaml-cpp/yaml.h>

using namespace std;
using namespace RT;
using namespace boost;
typedef int data;
typedef unsigned int ID;

class Point;
class BB;
class PointSet;
// class BBSet;

RTree<int, data> skylineRTree; // RTree for the skyline points
vector<PointSet>
    CompSky; /* set of (closed / maximal / normal) competitive sets. */
vector<vector<int>> partitionSkylines;
vector<RTree<int, data>> partitionSkylineRTrees;
vector<Point> skyline; /* set of skyline points */

/**
 * @brief The name of the algorithm that has to be run.
 *
 */
enum Algorithm {
    MAXIMAL_2D,
    MAXIMAL_CLIQUE_ENUM,
    APPROX_CS,
    APPROX_MCS,
    BK,
    CLIQUE_ENUM,
    TSUKIYAMA,
    MAXIMAL_BRUTEFORCE
};

static string algToStr[] = {
    "MAXIMAL_2D", "MAXIMAL_CLIQUE_ENUM", "APPROX_CS", "APPROX_MCS",
    "BK",         "CLIQUE_ENUM",         "TSUKIYAMA", "MAXIMAL_BRUTEFORCE"};

Algorithm strToAlg(string s) {
    for (int i = 0; i < 8; i++) {
        if (s == algToStr[i]) {
            return (Algorithm)i;
        }
    }
    return MAXIMAL_CLIQUE_ENUM;
}

istream& operator>>(istream& is, Algorithm& alg) {
    string s;
    alg = strToAlg(s);
    return is;
}

/**
 * @brief The kind of competitive skyline (normal, closed and maximal) to be
 * computed
 *
 */
enum CSType { CS, APPROXCS, APPROXMCS, MCS };

static string cstypeToStr[] = {"CS", "APPROX_CS", "APPROX_MCS", "MCS"};

CSType strToCSType(string s) {
    for (int i = 0; i < 4; i++) {
        if (s == cstypeToStr[i]) {
            return (CSType)i;
        }
    }
    return MCS;
}

istream& operator>>(istream& is, CSType& cstype) {
    string s;
    is >> s;
    cstype = strToCSType(s);
    return is;
}

unsigned int PRECISION = 6; // precision for the output, see @domain()

data domain(double, unsigned int = PRECISION);
double redomain(data, unsigned int = PRECISION);

string DATASET = "ANTI";              // type of dataset
unsigned int DIMENSIONALITY = 2;      // dimensionality of the dataset
unsigned int DATA_CARDINALITY = 1000; // number of data points
data EPSILON = domain(0.3); // epsilon for the competitive skyline algorithm
unsigned int SORT_DIMENSION = 1;           // dimension to sort the points
unsigned int SKY_CARDINALITY = 0;          // number of skyline points
Algorithm algorithm = MAXIMAL_CLIQUE_ENUM; // algorithm to run
CSType csType = MCS;                       // type of competitive skyline
double timeStat = 0;                       // time taken by the algorithm
unsigned int NUM_ITERS = 1; // number of iterations for the algorithm
bool VARY_EPSILON = true;
bool VARY_CARDINALITY = true;
bool VARY_DIMENSIONALITY = true;
bool VARY_ALGORITHMS = true;
bool EXPORT_METRICS = false;
bool EXPORT_COMPETITIVE_SETS = false;
bool EXPORT_OVERLAP_METRICS = false;
bool EXPORT_TIME_METRICS = false;
bool EXPORT_DISTANCE_DISTRIBUTION = false;
bool EXPORT_PARTITIONING_METRICS = false;
bool PARTITION = false;
bool COMPUTE_SKYLINE = false;
int duplicates = 0;
vector<int> num_parts;
vector<double> avg_part_size;

string DATA_PATH =
    "/Users/jayitha/Documents/CSAlgorithms/data/"; // path to the data
string CONFIG_FILE = "config.yml";                 // path to the config file
string DATA_DB_NAME = "data1.db"; // name of the database with experiment data
string STAT_DB_NAME =
    "comp_sky_stats1.db"; // name of the database to emit metrics
string SKY_ATTS =
    "skyline_attributes"; // name of the table to find profile of preferences

// possible dataset types, dimensionalities, cardinalities and epsilons
// TODO: move to script so the database can remain extendable
vector<string> datasets = {"hotels", "UCars", "ANTI", "INDEP", "CORR"};
// vector<unsigned int> ds = {2, 4, 5, 6, 10, 14, 16, 18, 20};
vector<unsigned int> ds = {2, 3, 4, 6, 8, 10, 15, 20}; // with overlap
// vector<unsigned int> ns = {10, 100, 10000};
vector<unsigned int> ns = {100, 1000, 10000, 100000, 1000000};
// vector<double> eps = {0.001, 0.005, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07,
// 0.08, 0.09, 0.1, 0.15}; vector<double> eps = {0.001, 0.005, 0.02, 0.03, 0.04,
// 0.06, 0.07, 0.08, 0.09, 0.2, 0.4, 0.5};
vector<double> eps = {0.001, 0.005, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06,
                      0.07,  0.08,  0.09, 0.1,  0.15, 0.2}; // with overlap
// vector<double> eps = {0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1};

/**
 * @brief data from the database is provided in float or double format in the
 * range [0,1]. This function converts float into integer retaining `precision`
 * number of digits from value
 *
 * @param value - value in float or double in range [0, 1]
 * @param precision - number of digits to retain
 * @return data
 */
data domain(double value, unsigned int precision) {
    const double multiplier = std::pow(10.0, precision);
    return (data)(value * multiplier);
}

double redomain(data value, unsigned int precision) {
    const double divider = std::pow(10.0, precision);
    return (double)((double)value / divider);
}

/**
 * @brief converst c type char * to string
 *
 * @param s
 * @return string
 */
string to_string(const char* s) {
    string res(s);
    return s;
}

// Used when debugging
void press_enter_to_continue() {
    cout << "Press Enter to Continue";
    cin.ignore();
}

#endif // COMP_SKY_GLOBAL_HPP
