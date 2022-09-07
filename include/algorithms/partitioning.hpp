//
// Created by Jayitha on 19/04/22.
//

#ifndef PARTITIONING_HPP
#define PARTITIONING_HPP

#include "pointSet.hpp"
#include "DBinterface.hpp"

void partition() {
    num_parts.clear();
    avg_part_size.clear();

    int num_part_iterations = 0;

    vector<vector<int>> oldPartitions;
    vector<int> skyline_ids(SKY_CARDINALITY);
    std::iota(skyline_ids.begin(), skyline_ids.end(), 0);
    oldPartitions.emplace_back(skyline_ids);

    vector<vector<int>> newPartitions;

    num_parts.emplace_back(1);
    avg_part_size.emplace_back(SKY_CARDINALITY);

    cout << "Iteration: " << num_part_iterations << endl;
    cout << "Num Partition Sets: " << num_parts[num_part_iterations] << endl;
    cout << "Avg. Partition Size: " << SKY_CARDINALITY << endl;

    exportPartitionSizes(0, vector<int>(1, SKY_CARDINALITY));
    while (num_part_iterations < 1 || num_parts[num_part_iterations] !=
                                          num_parts[num_part_iterations - 1]) {

        num_part_iterations++;

        // for each dimension
        for (int d = 0; d < DIMENSIONALITY; d++) {
            newPartitions.clear();

            // for each partition
            for (auto partition_set : oldPartitions) {

                if (BB(partition_set).competitive()) {
                    newPartitions.emplace_back(partition_set);
                    continue;
                }

                sort(partition_set.begin(), partition_set.end(),
                     [d](const int& a, const int& b) {
                         if (skyline[a].point[d] > skyline[b].point[d])
                             return true;
                         else if (skyline[a].point[d] == skyline[b].point[d])
                             return skyline[a].id > skyline[b].id;
                         else
                             return false;
                     });

                vector<int> new_partition_set;
                for (int i = 0; i < partition_set.size(); i++) {
                    new_partition_set.emplace_back(partition_set[i]);
                    if (i == partition_set.size() - 1 ||
                        abs(skyline[partition_set[i]].point[d] -
                            skyline[partition_set[i + 1]].point[d]) > EPSILON) {

                        newPartitions.emplace_back(new_partition_set);
                        new_partition_set.clear();
                    }
                }
            }

            oldPartitions.swap(newPartitions);
        }
        num_parts.emplace_back(oldPartitions.size());
        double avg_size = 0;
        vector<int> part_sizes;
        for (auto partition_set : oldPartitions) {
            avg_size += partition_set.size();
            part_sizes.emplace_back(partition_set.size());
        }

        avg_size /= num_parts[num_part_iterations];
        avg_part_size.emplace_back(avg_size);

        cout << "Iteration: " << num_part_iterations << endl;
        cout << "Num Partition Sets: " << num_parts[num_part_iterations]
             << endl;
        cout << "Avg. Partition Size: " << avg_size << endl;

        exportPartitionSizes(num_part_iterations, part_sizes);
        
    }
}

#endif // PARTITIONING_HPP