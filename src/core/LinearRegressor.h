#ifndef SRC_LINEARREGRESSOR_H
#define SRC_LINEARREGRESSOR_H

#include <iostream>
#include <utility>
#include <vector>
#include <cmath>
#include <iomanip>
#include <functional>
#include <map>

#include <boost/python.hpp>
#include "Dataset.h"

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

using namespace core::dataset;

namespace core::lr {
    class LinearRegressor {
    public:
        LinearRegressor() = default;

        vector_ fit(Dataset& df, int n_epochs=3, double alpha=1, Dataset test_df=Dataset());

        double evaluate(Dataset df, vector_ const& custom_theta=vector_());

        vector_ train_nrmse, test_nrmse;

        vector_ get_train_nrmse() {
            return train_nrmse;
        }

        vector_ get_test_nrmse() {
            return test_nrmse;
        }
    private:
        vector_ theta;

        vector_ get_theta(const vector<Dataset::Scale>& X_scales, Dataset::Scale y_scales);

        double calc_nrmse(vector_ const& step_theta, Dataset const& df);
    };
}


#endif //SRC_LINEARREGRESSOR_H
