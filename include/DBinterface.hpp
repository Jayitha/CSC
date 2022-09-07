//
// Created by Jayitha on 14/04/22.
//

#ifndef COMP_SKY_DBINTERFACE_HPP
#define COMP_SKY_DBINTERFACE_HPP

#include "metrics.hpp"
#include "point.hpp"
#include "pointSet.hpp"

class AttPreference {
  public:
    string att_name;
    enum Preference { MIN, MAX } att_pref;

    AttPreference(string att_name, string att_pref) : att_name(att_name) {
        this->att_pref =
            att_pref == "MIN" ? AttPreference::MIN : AttPreference::MAX;
    }
    static void defaultAttPreferences(vector<AttPreference>& attPreferences) {
        attPreferences.clear();
        for (int i = 1; i <= DIMENSIONALITY; i++) {
            attPreferences.emplace_back("D" + to_string(i), "MIN");
        }
    }
    friend ostream& operator<<(ostream& os, AttPreference& attPref);
};

ostream& operator<<(ostream& os, AttPreference& attPref) {
    os << "(" << attPref.att_name << ", ";
    if (attPref.att_pref == AttPreference::MIN)
        os << "MIN";
    else
        os << "MAX";
    os << ")";
    return os;
}

//################################# IMPORT #####################################

/* loads d-dimensional (dimensionality) skyline from dataset */
/**
 * Reading skyline attributes and preferences from skyline_attributes Reading
 * from DATASETTYPE_DIMEN_CARD Writes to dataset table the size of the skyline
 * if not already present
 */
void loadDataset() {
    cout << "Loading normalized skyline..." << flush;
    try {

        /* clearing skyline if exists */
        // skyline = PointSet();
        skyline.clear();
        // skylineRtree = RTree<Point, data>();
        skylineRTree.RemoveAll();

        /* connect to database */
        SQLite::Database db(DATA_PATH + DATA_DB_NAME, SQLite::OPEN_READWRITE);

        /* getting skyline attributes and preferences */
        vector<AttPreference> skylineAttributePreferences;
        if (DATASET == "ANTI" || DATASET == "CORR" || DATASET == "INDEP") {
            AttPreference::defaultAttPreferences(skylineAttributePreferences);
        } else {
            string skyAttPrefsQuery =
                "SELECT attribute, minmax FROM skyline_attributes "
                "WHERE dataset_ID = \"" +
                DATASET +
                "\" AND dimensionality = " + to_string(DIMENSIONALITY) +
                " AND cardinality = " + to_string(DATA_CARDINALITY) +
                " ORDER BY attribute;";
            SQLite::Statement prefQuery(db, skyAttPrefsQuery);
            while (prefQuery.executeStep()) {
                skylineAttributePreferences.emplace_back(
                    prefQuery.getColumn(0), prefQuery.getColumn(1));
            }

            assert(DIMENSIONALITY == skylineAttributePreferences.size());
        }
        /* getting normalized skyline points */
        string datasetQuery = "SELECT d.ID"; /* getting the dataset, trimming
                                                non-essential columns */
        string normalizedDatasetQuery = "SELECT nd.ID"; /* normalized data */
        string
            skylineNormalizedDatasetQuery; /* getting only the skyline points */
        string ANDconditions; /* AND conditions of the skyline query */
        string ORconditions;  /* OR conditions of the skyline query */
        string createIndex =
            "CREATE INDEX \"ind\" ON nd("; /* index creation query for faster
                                              skyline computation */

        for (auto& sky_att : skylineAttributePreferences) {
            string att = sky_att.att_name;

            datasetQuery += ", d." + att + ", min(d." + att +
                            ") OVER() AS min_" + att + ",  max(d." + att +
                            ") OVER() - min(d." + att + ") OVER()  as range_" +
                            att;

            normalizedDatasetQuery += ", round(1.000*(nd." + att +
                                      " - nd.min_" + att + ")/nd.range_" + att +
                                      ", 3) AS " + att;

            /* picks operator based on if the attribute should be minimized or
             * maximized */
            string dom_operator =
                sky_att.att_pref == AttPreference::MIN ? "<" : ">";
            ANDconditions +=
                "snd1." + att + " " + dom_operator + "= snd." + att + " AND ";
            ORconditions +=
                "snd1." + att + " " + dom_operator + " snd." + att + " OR ";

            createIndex += sky_att.att_name + ", ";
        }
        ORconditions += "0"; /* padding OR condition with 0 for trailing OR */

        /* removing trailing ", " */
        createIndex.pop_back();
        createIndex.pop_back();
        createIndex += ")";

        datasetQuery += " FROM " + DATASET + "_" + to_string(DIMENSIONALITY) +
                        "_" + to_string(DATA_CARDINALITY) + " d";

        normalizedDatasetQuery += " FROM (" + datasetQuery + ") nd";

        db.exec("CREATE TEMPORARY TABLE nd AS " +
                normalizedDatasetQuery); /* creates a temporary table of
                                            normalized dataset */
        /**
         * We don't create views because views cannot be indexed. Instead
         * temporary tables are created. Temporary tables are created in a
         * separate temp folder that is erased when the "session" is terminated.
         * I'm assuming that means that the temporary table gets deleted when
         * the function ends.
         */

        /* creating index on temp table nd */
        db.exec(createIndex);

        /* computing skyline */
        skylineNormalizedDatasetQuery =
            "SELECT * FROM nd snd WHERE NOT EXISTS(SELECT 1 FROM nd snd1 "
            "WHERE " +
            ANDconditions + "(" + ORconditions + ") LIMIT 1);";
        SQLite::Statement skylineQuery(db, skylineNormalizedDatasetQuery);
        while (skylineQuery.executeStep()) {
            ID id = skylineQuery.getColumn(0).getUInt();
            vector<data> vals;
            for (int i = 1; i <= DIMENSIONALITY; i++)
                vals.push_back(domain(skylineQuery.getColumn(i).getDouble()));
            Point new_point(id, vals);
            // skyline += new_point;
            skyline.emplace_back(new_point);
        }
        sort(skyline.begin(), skyline.end());
        for (int i = 0; i < skyline.size(); i++) {
            skylineRTree.Insert(skyline[i].point.data(),
                                skyline[i].point.data(), i);
        }

        SKY_CARDINALITY = skyline.size();

        cout << "Skyline size: " << SKY_CARDINALITY << endl;

        if (EXPORT_DISTANCE_DISTRIBUTION) {
            cout << "Exporting dist distributions..." << endl;
            string tableName = "dist_distributions";
            //   SQLite::Statement checkQuery(
            //     db, "SELECT 1 FROM " + tableName + " WHERE dataset_ID = \"" +
            //     DATASET +
            //             "\" AND dimensionality = " +
            //             to_string(DIMENSIONALITY) + " AND cardinality = " +
            //             to_string(DATA_CARDINALITY) +
            //             + " LIMIT 1");

            // if (!checkQuery.executeStep()) {

            db.exec("CREATE TEMPORARY TABLE norm_sky AS " +
                    skylineNormalizedDatasetQuery);
            string chebyQuery = "MAX(";
            for (auto& att_pref : skylineAttributePreferences) {
                string att = att_pref.att_name;
                chebyQuery += "ABS(snd1." + att + " - snd2." + att + "), ";
            }
            chebyQuery += "0 )";
            string pairwiseDistances =
                "INSERT INTO dist_distributions SELECT \"" + DATASET + "\", " +
                to_string(DIMENSIONALITY) + ", " + to_string(DATA_CARDINALITY) +
                ", snd1.ID, snd2.ID, " + chebyQuery +
                " FROM norm_sky snd1, norm_sky snd2 WHERE snd1.ID < snd2.ID "
                "GROUP BY snd1.ID, snd2.ID;";

            // cout << pairwiseDistances << endl;
            SQLite::Statement insert(db, pairwiseDistances);
            assert(insert.exec());
            db.exec("DROP TABLE IF EXISTS norm_sky;");
            // }
            cout << "Done." << endl;
        }

        /* dropping index on table nd */
        db.exec("DROP INDEX ind");

        /* dropping temporary table*/
        db.exec("DROP TABLE IF EXISTS nd;");

        /* Update skyline cardinality in the datasets table */
        SQLite::Database db_stats(DATA_PATH + STAT_DB_NAME,
                                  SQLite::OPEN_READWRITE);

        SQLite::Statement checkQuery(
            db_stats,
            "SELECT 1 FROM datasets WHERE dataset_ID = \"" + DATASET +
                "\" AND dimensionality = " + to_string(DIMENSIONALITY) +
                " AND cardinality = " + to_string(DATA_CARDINALITY) +
                " LIMIT 1");

        if (checkQuery.executeStep())
            return;

        string insertSkySize = "INSERT INTO datasets VALUES (\"" + DATASET +
                               "\", " + to_string(DIMENSIONALITY) + ", " +
                               to_string(DATA_CARDINALITY) + ", " +
                               to_string(SKY_CARDINALITY) + ")";
        SQLite::Statement insert(db_stats, insertSkySize);
        assert(insert.exec());
        cout << "Done." << endl;
    } catch (std::exception& e) {
        std::cout << "SQLite exception: " << e.what() << std::endl;
    }
}

void computeSkyline() {
    cout << "Loading normalized skyline..." << flush;
    try {

        skyline.clear();
        skylineRTree.RemoveAll();

        /* connect to database */
        SQLite::Database db(DATA_PATH + DATA_DB_NAME, SQLite::OPEN_READWRITE);

        assert(DATASET == "ANTI" || DATASET == "CORR" || DATASET == "INDEP");
        /* getting skyline attributes and preferences */
        vector<AttPreference> skylineAttributePreferences;
        AttPreference::defaultAttPreferences(skylineAttributePreferences);

        /* getting normalized points */
        string datasetQuery = "SELECT d.ID"; /* getting the dataset, trimming
                                                non-essential columns */
        string normalizedDatasetQuery = "SELECT nd.ID"; /* normalized data */

        for (auto& sky_att : skylineAttributePreferences) {
            string att = sky_att.att_name;

            datasetQuery += ", d." + att + ", min(d." + att +
                            ") OVER() AS min_" + att + ",  max(d." + att +
                            ") OVER() - min(d." + att + ") OVER()  as range_" +
                            att;

            normalizedDatasetQuery += ", round(1.000*(nd." + att +
                                      " - nd.min_" + att + ")/nd.range_" + att +
                                      ", 3) AS " + att;
        }

        datasetQuery += " FROM " + DATASET + "_" + to_string(DIMENSIONALITY) +
                        "_" + to_string(DATA_CARDINALITY) + " d";

        normalizedDatasetQuery += " FROM (" + datasetQuery + ") nd";

        db.exec("CREATE TEMPORARY TABLE nd AS " +
                normalizedDatasetQuery); /* creates a temporary table of
                                            normalized dataset */

        /* computing skyline */
        string iteratingNormalizedData = "SELECT * FROM nd;";

        int num_points = 0;
        SQLite::Statement iteratingNormalizedDataQuery(db,
                                                       iteratingNormalizedData);

        RTree<Point, int> skylineCompRTree;

        while (iteratingNormalizedDataQuery.executeStep()) {
            num_points++;
            if (num_points % 10000 == 0)
                cout << "Processed " << num_points / 10000 << " points."
                     << endl;

            ID id = iteratingNormalizedDataQuery.getColumn(0).getUInt();
            vector<data> vals;
            for (int i = 1; i <= DIMENSIONALITY; i++)
                vals.push_back(domain(
                    iteratingNormalizedDataQuery.getColumn(i).getDouble()));
            Point new_point(id, vals);

            // check if point is dominated
            BB dominatingBB = BB::dominatingRegion(new_point);
            vector<Point> dominatingPoints = skylineCompRTree.Range(
                dominatingBB.lb.point.data(), dominatingBB.ub.point.data());
            bool dominated = false;
            for (auto& p : dominatingPoints) {
                if (!(p == new_point)) {
                    dominated = true;
                    break;
                }
            }

            if (dominated)
                continue;

            // check which points point dominates
            BB dominanceBB = BB::dominanceRegion(new_point);
            vector<Point> dominatedPoints = skylineCompRTree.Range(
                dominanceBB.lb.point.data(), dominanceBB.ub.point.data());
            for (auto& p : dominatedPoints) {
                if (!(p == new_point)) {
                    skylineCompRTree.Remove(p.point.data(), p.point.data(), p);
                }
            }

            skylineCompRTree.Insert(new_point.point.data(),
                                    new_point.point.data(), new_point);
        }

        skyline = skylineCompRTree.Range(Point(domain(0)).point.data(),
                                         Point(domain(1)).point.data());
        sort(
            skyline.begin(), skyline.end(),
            [](const Point& a, const Point& b) -> bool { return a.id < b.id; });

        db.exec("CREATE TABLE " + DATASET + "_" + to_string(DIMENSIONALITY) +
                "_" + to_string(DATA_CARDINALITY) +
                "_skyline AS SELECT * FROM " + DATASET + "_" +
                to_string(DIMENSIONALITY) + "_" + to_string(DATA_CARDINALITY) +
                " WHERE 0");

        string insertStatement =
            "INSERT INTO " + DATASET + "_" + to_string(DIMENSIONALITY) + "_" +
            to_string(DATA_CARDINALITY) + "_skyline VALUES (";

        for (auto& p : skyline) {
            string pointSpecificInsert = to_string(p.id);
            for (int i = 0; i < DIMENSIONALITY; i++) {
                pointSpecificInsert += ", " + to_string(redomain(p.point[i]));
            }
            pointSpecificInsert += ");";
            assert(db.exec(insertStatement + pointSpecificInsert));
        }

        sort(skyline.begin(), skyline.end());

        for (int i = 0; i < skyline.size(); i++) {
            skylineRTree.Insert(skyline[i].point.data(),
                                skyline[i].point.data(), i);
        }

        SKY_CARDINALITY = skyline.size();

        cout << "Skyline size: " << SKY_CARDINALITY << endl;

        /* dropping temporary table*/
        db.exec("DROP TABLE IF EXISTS nd;");

        /* Update skyline cardinality in the datasets table */
        SQLite::Database db_stats(DATA_PATH + STAT_DB_NAME,
                                  SQLite::OPEN_READWRITE);

        SQLite::Statement checkQuery(
            db_stats,
            "SELECT 1 FROM datasets WHERE dataset_ID = \"" + DATASET +
                "\" AND dimensionality = " + to_string(DIMENSIONALITY) +
                " AND cardinality = " + to_string(DATA_CARDINALITY) +
                " LIMIT 1");

        if (checkQuery.executeStep())
            return;

        string insertSkySize = "INSERT INTO datasets VALUES (\"" + DATASET +
                               "\", " + to_string(DIMENSIONALITY) + ", " +
                               to_string(DATA_CARDINALITY) + ", " +
                               to_string(SKY_CARDINALITY) + ")";
        SQLite::Statement insert(db_stats, insertSkySize);
        assert(insert.exec());
        cout << "Done." << endl;
    } catch (std::exception& e) {
        std::cout << "SQLite exception: " << e.what() << std::endl;
    }
}

void loadSkyline() {

    try {

        skyline.clear();
        skylineRTree.RemoveAll();
        /* connect to database */
        SQLite::Database db(DATA_PATH + DATA_DB_NAME, SQLite::OPEN_READWRITE);

        string skyline_table = DATASET + "_" + to_string(DIMENSIONALITY) + "_" +
                               to_string(DATA_CARDINALITY) + "_skyline";

        /* check to make sure CS not already exported */
        SQLite::Statement checkQuery(db,
                                     "SELECT 1 FROM " + skyline_table + ";");

        checkQuery.executeStep();

        string loadSkylineStatement = "SELECT * FROM " + skyline_table + ";";
        cout << "Loading precomputed skyline ..." << endl;
        SQLite::Statement loadSkylineQuery(db, loadSkylineStatement);

        while (loadSkylineQuery.executeStep()) {
            ID id = loadSkylineQuery.getColumn(0).getUInt();
            vector<data> vals;
            for (int i = 1; i <= DIMENSIONALITY; i++)
                vals.push_back(
                    domain(loadSkylineQuery.getColumn(i).getDouble()));
            Point new_point(id, vals);
            skyline.emplace_back(new_point);
            skylineRTree.Insert(new_point.point.data(), new_point.point.data(),
                                skyline.size() - 1);
        }
        SKY_CARDINALITY = skyline.size();
        sort(skyline.begin(), skyline.end());
        cout << "Done." << endl;
    } catch (std::exception& e) {
        std::cout << "SQLite exception: " << e.what() << std::endl;
        if (DATA_CARDINALITY >= 10000 && COMPUTE_SKYLINE) {
            cout << "Computing Skyline in Code ..." << endl;
            computeSkyline();
            return;
        }
        cout << "Coputing skyline in SQL ..." << endl;
        loadDataset();
    }
}

//################################# EXPORT #####################################

bool checkMetrics() {
    cout << "Checking if metrics already generated..." << endl;
    SQLite::Database db(DATA_PATH + STAT_DB_NAME, SQLite::OPEN_READWRITE);

    if (!EXPORT_COMPETITIVE_SETS && !EXPORT_METRICS &&
        !EXPORT_OVERLAP_METRICS && !EXPORT_TIME_METRICS)
        return true;

    // checks if CS size and cs properties are already computed
    if (EXPORT_METRICS) {
        string competitive_set_table = "competitive_set";
        SQLite::Statement checkQuery(
            db, "SELECT 1 FROM " + competitive_set_table +
                    " WHERE dataset_ID = \"" + DATASET +
                    "\" AND dimensionality = " + to_string(DIMENSIONALITY) +
                    " AND cardinality = " + to_string(DATA_CARDINALITY) +
                    " AND epsilon = " + to_string(redomain(EPSILON)) +
                    " AND CS = \"" + cstypeToStr[csType] + "\" LIMIT 1");
        if (checkQuery.executeStep()) {
            return true;
        }

        string competitive_set_properties_table = "competitive_set_properties";

        SQLite::Statement checkQuery2(
            db, "SELECT 1 FROM " + competitive_set_properties_table +
                    " WHERE dataset_ID = \"" + DATASET +
                    "\" AND dimensionality = " + to_string(DIMENSIONALITY) +
                    " AND cardinality = " + to_string(DATA_CARDINALITY) +
                    " AND epsilon = " + to_string(redomain(EPSILON)) +
                    " AND CS = \"" + cstypeToStr[csType] + "\" LIMIT 1");

        if (checkQuery2.executeStep()) {
            return true;
        }
    }

    if (EXPORT_OVERLAP_METRICS) {
        string competitive_set_properties_table = "competitive_set_properties";

        SQLite::Statement checkQuery(
            db, "SELECT overlap FROM " + competitive_set_properties_table +
                    " WHERE dataset_ID = \"" + DATASET +
                    "\" AND dimensionality = " + to_string(DIMENSIONALITY) +
                    " AND cardinality = " + to_string(DATA_CARDINALITY) +
                    " AND epsilon = " + to_string(redomain(EPSILON)) +
                    " AND CS = \"" + cstypeToStr[csType] + "\" LIMIT 1");

        if (checkQuery.executeStep())
            if (checkQuery.getColumn(0).getDouble() >= 0) {
                return true;
            }
    }

    if (EXPORT_COMPETITIVE_SETS) {
        string competitive_set_table = "competitive_set";

        SQLite::Statement checkQuery(
            db, "SELECT 1 FROM " + competitive_set_table +
                    " WHERE dataset_ID = \"" + DATASET +
                    "\" AND dimensionality = " + to_string(DIMENSIONALITY) +
                    " AND cardinality = " + to_string(DATA_CARDINALITY) +
                    " AND epsilon = " + to_string(redomain(EPSILON)) +
                    " AND CS = \"" + cstypeToStr[csType] + "\" LIMIT 1");

        if (checkQuery.executeStep()) {
            return true;
        }
    }

    // if(EXPORT_TIME_METRICS){
    //   //TODO: write code for time stats
    // }
    return false;
}
/* Competitive skyline is exported The function write the content of each
   competitive set in the competitive skyline into a table titled
   `competitive_set`. The columsn are:

   dataset_ID, dimensionality, cardinality, ... <cs_ID>, <point_ID>
 */
void exportCS() {
    try {

        cout << "Exporting competitive sets... " << flush;

        /* connect to database */
        SQLite::Database db(DATA_PATH + STAT_DB_NAME, SQLite::OPEN_READWRITE);

        /* ensure that the CS has been computed */
        assert(!CompSky.empty());

        string competitive_set_table = "competitive_set";

        /* check to make sure CS not already exported */
        SQLite::Statement checkQuery(
            db, "SELECT 1 FROM " + competitive_set_table +
                    " WHERE dataset_ID = \"" + DATASET +
                    "\" AND dimensionality = " + to_string(DIMENSIONALITY) +
                    " AND cardinality = " + to_string(DATA_CARDINALITY) +
                    " AND epsilon = " + to_string(redomain(EPSILON)) +
                    " AND CS = \"" + cstypeToStr[csType] + "\" LIMIT 1");

        if (checkQuery.executeStep())
            return;

        /* CSs not exported, therefore inserted */
        string insertcsStatement =
            "INSERT INTO " + competitive_set_table + " VALUES(\"" + DATASET +
            "\", " + to_string(DIMENSIONALITY) + ", " +
            to_string(DATA_CARDINALITY) + ", " + to_string(redomain(EPSILON)) +
            ", \"" + cstypeToStr[csType] + "\", ?, ?)"; /* cs_ID, point_ID */

        SQLite::Statement insertcs(db, insertcsStatement);

        for (int i = 1; i <= CompSky.size(); i++) {
            for (auto& p : CompSky[i - 1].point_idx) {
                SQLite::bind(insertcs, i, skyline[p].id);
                assert(insertcs.exec());
                insertcs.reset();
            }
        }
        cout << "Done." << endl;
    } catch (std::exception& e) {
        std::cout << "SQLite exception: " << e.what() << std::endl;
    }
}

/**
 * @brief exports some properties of each competitive set into table
 * `competitive_set_properties`
 *
 * 1. `cs_card` - cardinality of the cs
 * 2. `cs_size` - cs.mbb.size()
 * 3. `overlap` (optional) - overlap of the competitive set
 *
 */
void exportcsProperties() {
    try {
        cout << "Exporting competitive sets properties... " << flush;
        /* connect to database */
        SQLite::Database db(DATA_PATH + STAT_DB_NAME, SQLite::OPEN_READWRITE);

        /* ensure that the CS has been computed */
        assert(!CompSky.empty());

        string competitive_set_properties_table = "competitive_set_properties";

        SQLite::Statement checkQuery(
            db, "SELECT 1 FROM " + competitive_set_properties_table +
                    " WHERE dataset_ID = \"" + DATASET +
                    "\" AND dimensionality = " + to_string(DIMENSIONALITY) +
                    " AND cardinality = " + to_string(DATA_CARDINALITY) +
                    " AND epsilon = " + to_string(redomain(EPSILON)) +
                    " AND CS = \"" + cstypeToStr[csType] + "\" LIMIT 1");

        if (checkQuery.executeStep())
            return;

        if (EXPORT_OVERLAP_METRICS)
            generateOverlapMetrics();
        else
            overlap_coefficient.assign(CompSky.size(), -1);

        string insertcsPropertiesStatement =
            "INSERT INTO " + competitive_set_properties_table + " VALUES(\"" +
            DATASET + "\", " + to_string(DIMENSIONALITY) + ", " +
            to_string(DATA_CARDINALITY) + ", " + to_string(redomain(EPSILON)) +
            ", \"" + cstypeToStr[csType] +
            "\", ?, ?, ?, ?)"; /* cs_ID, cs size, mbb size, cs overlap */

        SQLite::Statement insertcsProperties(db, insertcsPropertiesStatement);

        for (int i = 1; i <= CompSky.size(); i++) {
            SQLite::bind(insertcsProperties, i, CompSky[i - 1].size(),
                         redomain(CompSky[i - 1].mbbSize()),
                         overlap_coefficient[i - 1]);
            assert(insertcsProperties.exec());
            insertcsProperties.reset();
        }
        cout << "Done." << endl;
    } catch (std::exception& e) {
        std::cout << "SQLite exception: " << e.what() << std::endl;
    }
}

/**
 * @brief exports size of the competitive skyline (i.e. number of competitive
 * sets) into a table titled `CS_sizes`
 *
 */
void exportCSSizes() {
    try {
        cout << "Exporting competitive skyline sizes... " << flush;
        /* connect to database */
        SQLite::Database db(DATA_PATH + STAT_DB_NAME, SQLite::OPEN_READWRITE);

        /* ensure that the CS has been computed */
        assert(!CompSky.empty());

        string CS_size_table = "CS_sizes";

        /* checking to see if CS Sizes already present */
        SQLite::Statement checkQuery(
            db, "SELECT 1 FROM " + CS_size_table + " WHERE dataset_ID = \"" +
                    DATASET +
                    "\" AND dimensionality = " + to_string(DIMENSIONALITY) +
                    " AND cardinality = " + to_string(DATA_CARDINALITY) +
                    " AND epsilon = " + to_string(redomain(EPSILON)) +
                    " AND CS = \"" + cstypeToStr[csType] + "\" LIMIT 1");
        if (checkQuery.executeStep())
            return;

        /* Inserting */
        string insertCSSizeQuery =
            "INSERT INTO " + CS_size_table + " VALUES (\"" + DATASET + "\", " +
            to_string(DIMENSIONALITY) + ", " + to_string(DATA_CARDINALITY) +
            ", " + to_string(SKY_CARDINALITY) + ", " +
            to_string(redomain(EPSILON)) + ", \"" + cstypeToStr[csType] +
            "\", " + to_string(CompSky.size()) + ")";
        SQLite::Statement insertCSSize(db, insertCSSizeQuery);
        assert(insertCSSize.exec());
        cout << "Done." << endl;
    } catch (std::exception& e) {
        std::cout << "SQLite exception: " << e.what() << std::endl;
    }
}

/**
 * Exports time stats for given configuration into the time_stats table.
 */
void exportTimeStat() {
    try {
        cout << "Exporting time stats... " << flush;
        /* connect to database */
        SQLite::Database db(DATA_PATH + STAT_DB_NAME, SQLite::OPEN_READWRITE);

        /* ensure that the CS has been computed */
        assert(!CompSky.empty());

        /* ensure time has been computed */
        assert(timeStat);

        string tableName = "time_stats";

        /* check to make sure CS not already exported */
        SQLite::Statement checkQuery(
            db, "SELECT 1 FROM " + tableName + " WHERE dataset_ID = \"" +
                    DATASET +
                    "\" AND dimensionality = " + to_string(DIMENSIONALITY) +
                    " AND cardinality = " + to_string(DATA_CARDINALITY) +
                    " AND epsilon = " + to_string(redomain(EPSILON)) +
                    " AND CS = \"" + cstypeToStr[csType] + "\" AND alg = \"" +
                    algToStr[algorithm] +
                    "\" AND sort_dimension = " + to_string(SORT_DIMENSION) +
                    " AND num_iters = " + to_string(NUM_ITERS) + " LIMIT 1");

        if (checkQuery.executeStep()) {
            return;
        }

        /* insert time stat */
        string insertStatement =
            "INSERT INTO " + tableName + " VALUES(\"" + DATASET + "\", " +
            to_string(DIMENSIONALITY) + ", " + to_string(DATA_CARDINALITY) +
            ", " + to_string(redomain(EPSILON)) + ", \"" + cstypeToStr[csType] +
            "\", \"" + algToStr[algorithm] + "\", " +
            to_string(SORT_DIMENSION) + ", " + to_string(NUM_ITERS) + ", " +
            to_string(timeStat) + ")";

        SQLite::Statement insert(db, insertStatement);
        assert(insert.exec());
        cout << "Done." << endl;
    } catch (std::exception& e) {
        std::cout << "SQLite exception: " << e.what() << std::endl;
    }
}

/**
 * @brief exports some properties of partitions
 *
 * 1. `num_iters` - num of iterations of partitioning algorithm
 * 2. `num_partitions` - number of partitions
 * 3. `avg_partition_size` - average size of the partition
 *
 */
void exportPartitionProperties() {
    try {
        cout << "Exporting parition properties... " << flush;
        /* connect to database */
        SQLite::Database db(DATA_PATH + STAT_DB_NAME, SQLite::OPEN_READWRITE);

        string partition_table = "partitioning";

        SQLite::Statement checkQuery(
            db, "SELECT 1 FROM " + partition_table + " WHERE dataset_ID = \"" +
                    DATASET +
                    "\" AND dimensionality = " + to_string(DIMENSIONALITY) +
                    " AND cardinality = " + to_string(DATA_CARDINALITY) +
                    " AND epsilon = " + to_string(redomain(EPSILON)) +
                    " LIMIT 1");

        if (checkQuery.executeStep())
            return;

        string insertPartitionPropertiesStatement =
            "INSERT INTO " + partition_table + " VALUES(\"" + DATASET + "\", " +
            to_string(DIMENSIONALITY) + ", " + to_string(DATA_CARDINALITY) +
            ", " + to_string(redomain(EPSILON)) +
            ", ?, ?, ?)"; /* num_iters, num_partitions, avg_partition_size */

        SQLite::Statement insertPartitionProperties(
            db, insertPartitionPropertiesStatement);

        for (int i = 0; i < num_parts.size(); i++) {
            SQLite::bind(insertPartitionProperties, i, num_parts[i],
                         avg_part_size[i]);
            assert(insertPartitionProperties.exec());
            insertPartitionProperties.reset();
        }
        cout << "Done." << endl;
    } catch (std::exception& e) {
        std::cout << "SQLite exception: " << e.what() << std::endl;
    }
}


void exportPartitionSizes(int iteration_num, vector<int> part_sizes) {
    try {
        cout << "Exporting parition sizes... " << flush;
        /* connect to database */
        SQLite::Database db(DATA_PATH + STAT_DB_NAME, SQLite::OPEN_READWRITE);

        string partition_table = "partition_sizes";

        SQLite::Statement checkQuery(
            db, "SELECT 1 FROM " + partition_table + " WHERE dataset_ID = \"" +
                    DATASET +
                    "\" AND dimensionality = " + to_string(DIMENSIONALITY) +
                    " AND cardinality = " + to_string(DATA_CARDINALITY) +
                    " AND epsilon = " + to_string(redomain(EPSILON)) +
                    " AND num_iters = " + to_string(iteration_num) + 
                    " LIMIT 1");

        if (checkQuery.executeStep())
            return;

        string insertPartitionSizeStatement =
            "INSERT INTO " + partition_table + " VALUES(\"" + DATASET + "\", " +
            to_string(DIMENSIONALITY) + ", " + to_string(DATA_CARDINALITY) +
            ", " + to_string(redomain(EPSILON)) + ", " + to_string(iteration_num) +
            ", ?, ?)"; /* part_ID, part_set_size */

        SQLite::Statement insertPartitionSize(
            db, insertPartitionSizeStatement);

        for (int i = 1; i <= part_sizes.size(); i++) {
            SQLite::bind(insertPartitionSize, i, part_sizes[i-1]);
            assert(insertPartitionSize.exec());
            insertPartitionSize.reset();
        }
        cout << "Done." << endl;
    } catch (std::exception& e) {
        std::cout << "SQLite exception: " << e.what() << std::endl;
    }
}

#endif // COMP_SKY_DBINTERFACE_HPP
