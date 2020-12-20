//
// Created by Konstantin Rybkin on 08/11/2019.
//

#ifndef SRC_F1SCORE_H
#define SRC_F1SCORE_H

#include <iostream>
#include <vector>

#include <boost/python.hpp>
#include "iterable_converter.h"

using std::vector;

typedef vector<vector<double>> matrix_;
typedef vector<double> vector_;

namespace core::metrics {
    class F1Score {
    public:
        static double f1(double precision, double recall);

        static double f1_macro(const matrix_ &cm);

        static double f1_micro(const matrix_ &cm);

    private:
        static double safe_div(double a, double b) {
            if (b == 0) return 0;
            return a / b;
        }

        static double vector_sum(const vector_& v);

        static vector_ calc_class_weights(const matrix_& cm);

        static double weighted_mean(const vector_& v, const vector_& weights);
    };
}


#endif //SRC_F1SCORE_H
