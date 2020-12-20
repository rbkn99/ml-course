#include "LinearRegressor.h"

// adam
vector_ core::lr::LinearRegressor::fit(Dataset& df, int n_epochs, double alpha, Dataset test_df) {
    auto X_norm_params = df.normalize();
    auto y_norm_params = df.normalize_y();
    df.add_column(1);
    theta.assign(df.m, 0);

    if (!test_df.empty()) {
        test_df.add_column(1);
    }

    vector_ m(df.m), v(df.m), m_hat(df.m), v_hat(df.m);
    const double b1 = 0.9;
    const double b2 = 0.999;
    const double eps = 1e-8;
    double b1_pow = b1, b2_pow = b2;

    double initial_alpha = alpha;
    double drop = 0.88;
    double epochs_drop = 1000;

    for (int epoch = 0; epoch < n_epochs; epoch++) {
        alpha = initial_alpha * pow(drop, floor((epoch + 1) / epochs_drop));
        int i = epoch % df.n;
        double h = dot(df.X[i], theta);
        for (size_t j = 0; j < df.m; j++) {
            double grad = (h - df.y[i]) * df.X[i][j];
            m[j] = b1 * m[j] + (1 - b1) * grad;
            v[j] = b2 * v[j] + (1 - b2) * grad * grad;
            m_hat[j] = m[j] / (1. - b1_pow);
            v_hat[j] = v[j] / (1. - b2_pow);
            theta[j] -= alpha * m_hat[j] / (sqrt(v_hat[j]) + eps);
        }
        b1_pow *= b1;
        b2_pow *= b2;
        if (!test_df.empty() && epoch % 1000 == 0) {
            train_nrmse.push_back(calc_nrmse(theta, df));
            test_nrmse.push_back(calc_nrmse(get_theta(X_norm_params, y_norm_params), test_df));
        }
    }
    return get_theta(X_norm_params, y_norm_params);
}

double core::lr::LinearRegressor::evaluate(Dataset df, vector_ const& custom_theta) {
    const double dumb_const1 = 20;
    const double dumb_const2 = 8.68589;

    df.add_column(1);
    double sum_pred = 0, sum_mean = 0;
    vector_ y_pred;
    if (custom_theta.empty()) {
        y_pred = dot(df.X, theta);
    } else {
        y_pred = dot(df.X, custom_theta);
    }
    double mean = vector_mean(y_pred);
    for (size_t i = 0; i < df.n; i++) {
        sum_pred += (df.y[i] - y_pred[i]) * (df.y[i] - y_pred[i]);
        sum_mean += (mean - y_pred[i]) * (mean - y_pred[i]);
    }
    return dumb_const1 - dumb_const2 * log(sum_pred / sum_mean);
}

vector_  core::lr::LinearRegressor::get_theta(const vector<Dataset::Scale>& X_scales, Dataset::Scale y_scales) {
    vector_ scaled_theta = theta;
    size_t m = theta.size();
    double mean_weighted_sum = 0;

    for (size_t i = 0; i < m - 1; i++) {
        if (X_scales[i].std_dev == 0) {
            scaled_theta[i] = 0;
        } else {
            scaled_theta[i] = theta[i] * (y_scales.std_dev / X_scales[i].std_dev);
        }
        mean_weighted_sum -= scaled_theta[i] * X_scales[i].mean;
    }
    scaled_theta.back() = scaled_theta.back() * y_scales.std_dev + mean_weighted_sum + y_scales.mean;
    return scaled_theta;
}

double core::lr::LinearRegressor::calc_nrmse(vector_ const &step_theta, Dataset const& df) {
    vector_ y_pred = dot(df.X, step_theta);
    double min_y = vector_min(y_pred);
    double max_y = vector_max(y_pred);
    double err_sum = 0;
    for (size_t i = 0; i < df.n; i++) {
        err_sum += pow(df.y[i] - y_pred[i], 2);
    }
    return sqrt(err_sum / df.n) / (max_y - min_y);
}

BOOST_PYTHON_MODULE (lr) {
    using namespace boost::python;
    using namespace core::lr;

    class_<LinearRegressor>("LinearRegressor", init<>())
            .def("fit", &LinearRegressor::fit)
            .def("evaluate", &LinearRegressor::evaluate)
            .def_readonly("train_nrmse", &LinearRegressor::get_train_nrmse)
            .def_readonly("test_nrmse", &LinearRegressor::get_test_nrmse);
}