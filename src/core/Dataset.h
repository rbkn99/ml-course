//
// Created by Konstantin Rybkin on 28/11/2019.
//

#ifndef SRC_DATASET_H
#define SRC_DATASET_H

#include <iostream>
#include <utility>
#include <vector>
#include <cmath>
#include <iomanip>
#include <functional>
#include <map>

#include <boost/python.hpp>
#include "iterable_converter.h"

using std::vector;
using std::map;
using std::string;
using std::function;
using std::min;
using std::max;
using std::cout;
using std::endl;

typedef vector<vector<double>> matrix_;
typedef vector<double> vector_;

namespace core::dataset {
    class Dataset {
    public:
        size_t n, m;

        matrix_ X;
        vector_ y;

        struct Scale {
            double mean = 0, std_dev = 0;
        };

        Dataset() = default;

        explicit Dataset(matrix_ const &raw_data);

        matrix_ to_raw();

        matrix_ get_X();

        vector_ get_y();

        size_t get_n() {
            return X.size();
        }

        double y_mean();

        void add_column(double val);

        vector<Scale> normalize();

        Scale normalize_y();

        void print();

        bool empty();
    };

    vector_ operator+(vector_ const& v1, vector_ const& v2);
    vector_ operator-(vector_ const& v1, vector_ const& v2);
    vector_ operator*(vector_ const& v1, vector_ const& v2);
    vector_ operator*(double s, vector_ const& v1);

    double dot(vector_ const& v1, vector_ const& v2);

    vector_ dot(matrix_ const& m1, vector_ const& v2);

    matrix_ transpose(matrix_ const& m);

    double vector_mean(vector_ const& v);

    double vector_min(vector_ const& v);

    double vector_max(vector_ const& v);
}


#endif //SRC_DATASET_H
