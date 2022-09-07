//
// Created by Jayitha on 17/05/22.
//

#ifndef COMP_SKY_CONFIG_HPP
#define COMP_SKY_CONFIG_HPP

#include "global.hpp"
#include "runs.hpp"

/* verifies if configuration is right */
bool checkConfig() {

  /* real dataset hotels has only 2 dimensions and 10 points */
  if (DATASET == "hotels") {
    if (DIMENSIONALITY != 2 || DATA_CARDINALITY != 10)
      return false;
  }

  /* CLOSED_2D and MAXIMAL_2D are only applicable when d=2 */
  if (DIMENSIONALITY != 2 &&
      (algorithm == MAXIMAL_2D))
    return false;

  /* Only one algorithm computes the whole competitive skyline */
  if (csType == CS && algorithm != CLIQUE_ENUM)
    return false;

  /* Only CLOSED_BACKTRACK and CLOSED_2D compute CLOSED */
  if (csType == APPROXMCS &&
      (algorithm != APPROX_MCS))
    return false;

    if (csType == APPROXCS &&
      (algorithm != APPROX_CS))
    return false;

  /* Only MAXIMAL_BACKTRACK and MAXIMAL_2D compute MAXIMAL */
  if (csType == MCS &&
      (algorithm != MAXIMAL_2D && algorithm != BK &&
       algorithm != MAXIMAL_CLIQUE_ENUM && algorithm != TSUKIYAMA))
    return false;

  /* sort dimension should be within specified number of dimensions */
  if (SORT_DIMENSION <= 0 || SORT_DIMENSION > DIMENSIONALITY)
    return false;

  if (EPSILON < domain(0) || EPSILON > domain(1))
    return false;

  return true;
}

/**
 * @brief Load params from `config.yml`, all params are optional.
 * 
 */
void loadParams(){
  YAML::Node config = YAML::LoadFile(DATA_PATH + CONFIG_FILE);

  if(YAML::Node parameter = config["precision"]){
    PRECISION = parameter.as<int>();
  }

  if(YAML::Node parameter = config["dataset"]){
    DATASET = parameter.as<string>();
  }

  if(YAML::Node parameter = config["dimensionality"]){
    DIMENSIONALITY = parameter.as<int>();
    VARY_DIMENSIONALITY = false;
  }

  if(YAML::Node parameter = config["cardinality"]){
    DATA_CARDINALITY = parameter.as<int>();
    VARY_CARDINALITY = false;
  }

  if(YAML::Node parameter = config["epsilon"]){
    EPSILON = domain(parameter.as<double>());
    VARY_EPSILON = false;
  }

  if(YAML::Node parameter = config["cs_type"]){
    csType = strToCSType(parameter.as<string>());
  }

  if(YAML::Node parameter = config["algorithm"]){
    algorithm = strToAlg(parameter.as<string>());
    VARY_ALGORITHMS = false;
  }

  if(YAML::Node parameter = config["sort_dimension"]){
    SORT_DIMENSION = parameter.as<int>();
  }

  if(YAML::Node parameter = config["num_iters"]){
    NUM_ITERS = parameter.as<int>();
  }

  if(YAML::Node parameter = config["export_metrics"]){
    EXPORT_METRICS = parameter.as<bool>();
  }

  if(YAML::Node parameter = config["export_competitive_sets"]){
    EXPORT_COMPETITIVE_SETS = parameter.as<bool>();
  }

  if(YAML::Node parameter = config["export_overlap_metrics"]){
    EXPORT_OVERLAP_METRICS = parameter.as<bool>();
  }

  if(YAML::Node parameter = config["export_time_metrics"]){
    EXPORT_TIME_METRICS = parameter.as<bool>();
  }

  if(YAML::Node parameter = config["export_dist_distribution"]){
    EXPORT_DISTANCE_DISTRIBUTION = parameter.as<bool>();
  }

  if(YAML::Node parameter = config["partition"]){
    PARTITION = parameter.as<bool>();
  }

  if(YAML::Node parameter = config["compute_skyline"]){
    COMPUTE_SKYLINE = parameter.as<bool>();
  }

  NUMDIMS = DIMENSIONALITY;
 }
#endif // COMP_SKY_CONFIG_HPP
