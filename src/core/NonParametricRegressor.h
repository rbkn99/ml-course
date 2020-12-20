//
// Created by Konstantin Rybkin on 08/11/2019.
//

#ifndef SRC_NONPARAMETRICREGRESSOR_H
#define SRC_NONPARAMETRICREGRESSOR_H

#include <iostream>
#include <utility>
#include <vector>
#include <cmath>
#include <iomanip>
#include <functional>

#include <boost/python.hpp>
#include "iterable_converter.h"

using std::vector;
using std::map;
using std::string;
using std::function;
using std::min;
using std::max;

typedef vector<vector<double>> matrix_;
typedef vector<double> vector_;

const double MAX_VAL = 100000000000;

namespace core::knn {
    class Dataset {
    public:
        explicit Dataset(matrix_ const &raw_data);

        matrix_ to_raw();

        matrix_ get_X();

        vector_ get_y();

        size_t get_n() {
            return X.size();
        }

        double y_mean();

        matrix_ X;
        vector_ y;
    };

    class NonParametricRegressor {
    public:
        explicit NonParametricRegressor(Dataset dataset);

        double predict(vector_ const &x, string const &distance, string const &kernel, string const &window,
                       double window_param, int drop=-1);

        vector_ loo_validate(int i, string const &distance,
                            string const &kernel, string const &window, double window_param);

        vector_ vector_distances(string const& dist_name);
    private:
        Dataset dataset;

        static double manhattan(vector_ const &x, vector_ const &y);

        static double euclidean(vector_ const &x, vector_ const &y);

        static double chebyshev(vector_ const &x, vector_ const &y);

        static double uniform(double u);

        static double triangular(double u);

        static double epanechnikov(double u);

        static double quartic(double u);

        static double triweight(double u);

        static double tricube(double u);

        static double gaussian(double u);

        static double cosine(double u);

        static double logistic(double u);

        static double sigmoid(double u);

        typedef function<double(vector_ const &, vector_ const &)> vec_signature;
        typedef function<double(double)> scalar_signature;

        static vec_signature choose_distance_metric(string const &name);

        static scalar_signature choose_kernel(string const &name);

        Dataset sort_by_distance(vector_ const &x, vec_signature &dist_f);
    };
}


#endif //SRC_NONPARAMETRICREGRESSOR_H
