//
// Created by Konstantin Rybkin on 08/11/2019.
//

#include "NonParametricRegressor.h"

core::knn::Dataset::Dataset(matrix_ const &raw_data) {
    size_t n = raw_data.size(), m = raw_data[0].size() - 1;
    X.resize(n);
    y.resize(n);
    for (size_t i = 0; i < n; i++) {
        X[i].resize(m);
        for (size_t j = 0; j < m; j++) {
            X[i][j] = raw_data[i][j];
        }
        y[i] = raw_data[i][m];
    }
}

matrix_ core::knn::Dataset::get_X() {
    return this->X;
}

vector_ core::knn::Dataset::get_y() {
    return this->y;
}

matrix_ core::knn::Dataset::to_raw() {
    matrix_ raw_matrix(X);
    for (size_t i = 0; i < get_n(); i++) {
        raw_matrix[i].push_back(y[i]);
    }
    return raw_matrix;
}

double core::knn::Dataset::y_mean() {
    double mean = 0;
    for (double i : y) {
        mean += i;
    }
    return mean / y.size();
}

core::knn::NonParametricRegressor::NonParametricRegressor(Dataset dataset) : dataset(std::move(dataset)) {}

double core::knn::NonParametricRegressor::predict(vector_ const &x, string const &distance,
                                                  string const &kernel, string const &window, double window_param, int drop) {
    auto distance_f = choose_distance_metric(distance);
    auto kernel_f = choose_kernel(kernel);
    if (window == "variable") {
        Dataset sorted_df = sort_by_distance(x, distance_f);
        window_param = distance_f(sorted_df.X[int(window_param)], x);
    }
    map<double, double> weighted_classes;
    for (size_t i = 0; i < dataset.get_n(); i++) {
        if (i == drop) continue;
        double dist_to_current = distance_f(dataset.X[i], x);
        if (window_param != 0) {
            dist_to_current = dist_to_current / window_param;
        } else {
            if (dist_to_current != 0) {
                dist_to_current = MAX_VAL;
            }
        }
        double smoothed_distance = kernel_f(dist_to_current);
        weighted_classes[dataset.y[i]] += smoothed_distance;
    }
    double result_class = 0, result_sum = 0;
    for (auto class_res: weighted_classes) {
        if (class_res.second > result_sum) {
            result_class = class_res.first;
            result_sum = class_res.second;
        }
    }
    return result_class;
}

vector_ core::knn::NonParametricRegressor::loo_validate(int i, string const &distance,
                                                       string const &kernel, string const &window,
                                                       double window_param) {
    double predicted = predict(dataset.X[i], distance, kernel, window, window_param, i);
    double actual = dataset.y[i];

    return vector_({predicted, actual});
}

core::knn::Dataset
core::knn::NonParametricRegressor::sort_by_distance(vector_ const &x, NonParametricRegressor::vec_signature &dist_f) {
    matrix_ raw_data = this->dataset.to_raw();
    sort(raw_data.begin(), raw_data.end(), [this, dist_f, x](vector_ const &a, vector_ const &b) {
        return dist_f(a, x) < dist_f(b, x);
    });
    return Dataset(raw_data);
}

double core::knn::NonParametricRegressor::manhattan(vector_ const &x, vector_ const &y) {
    size_t n = min(x.size(), y.size());
    double distance = 0;
    for (size_t i = 0; i < n; i++) {
        distance += abs(x[i] - y[i]);
    }
    return distance;
}

double core::knn::NonParametricRegressor::euclidean(vector_ const &x, vector_ const &y) {
    size_t n = min(x.size(), y.size());
    double distance = 0;
    for (size_t i = 0; i < n; i++) {
        distance += (x[i] - y[i]) * (x[i] - y[i]);
    }
    return sqrt(distance);
}

double core::knn::NonParametricRegressor::chebyshev(vector_ const &x, vector_ const &y) {
    size_t n = min(x.size(), y.size());
    double distance = 0;
    for (size_t i = 0; i < n; i++) {
        distance = max(distance, abs(x[i] - y[i]));
    }
    return distance;
}

double core::knn::NonParametricRegressor::uniform(double u) {
    if (abs(u) >= 1) return 0;
    return 0.5;
}

double core::knn::NonParametricRegressor::triangular(double u) {
    if (abs(u) >= 1) return 0;
    return 1. - abs(u);
}

double core::knn::NonParametricRegressor::epanechnikov(double u) {
    if (abs(u) >= 1) return 0;
    return 0.75 * (1 - u * u);
}

double core::knn::NonParametricRegressor::quartic(double u) {
    if (abs(u) >= 1) return 0;
    return (15.0 / 16.0) * pow(1 - u * u, 2);
}

double core::knn::NonParametricRegressor::triweight(double u) {
    if (abs(u) >= 1) return 0;
    return (35.0 / 32.0) * pow(1 - u * u, 3);
}

double core::knn::NonParametricRegressor::tricube(double u) {
    if (abs(u) >= 1) return 0;
    return (70.0 / 81.0) * pow(1 - abs(u * u * u), 3);
}

double core::knn::NonParametricRegressor::gaussian(double u) {
    return (1. / sqrt(2 * M_PI)) * pow(M_E, -0.5 * u * u);
}

double core::knn::NonParametricRegressor::cosine(double u) {
    if (abs(u) >= 1) return 0;
    return (M_PI / 4.0) * cos((M_PI / 2) * u);
}

double core::knn::NonParametricRegressor::logistic(double u) {
    return 1.0 / (pow(M_E, u) + 2 + pow(M_E, -u));
}

double core::knn::NonParametricRegressor::sigmoid(double u) {
    return (2 / M_PI) * (1.0 / (pow(M_E, u) + pow(M_E, -u)));
}

function<double(vector_ const &, vector_ const &)>
core::knn::NonParametricRegressor::choose_distance_metric(string const &name) {
    if (name == "manhattan") return &manhattan;
    if (name == "euclidean") return &euclidean;
    if (name == "chebyshev") return &chebyshev;
    return nullptr;
}

function<double(double)> core::knn::NonParametricRegressor::choose_kernel(string const &name) {
    if (name == "uniform") return &uniform;
    if (name == "triangular") return &triangular;
    if (name == "epanechnikov") return &epanechnikov;
    if (name == "quartic") return &quartic;
    if (name == "triweight") return &triweight;
    if (name == "tricube") return &tricube;
    if (name == "gaussian") return &gaussian;
    if (name == "cosine") return &cosine;
    if (name == "logistic") return &logistic;
    if (name == "sigmoid") return &sigmoid;
    return nullptr;
}

vector_ core::knn::NonParametricRegressor::vector_distances(string const &dist_name) {
    auto dist_f = choose_distance_metric(dist_name);
    vector_ distances;
    for (size_t i = 0; i < dataset.get_n(); i++) {
        for (size_t j = i + 1; j < dataset.get_n(); j++) {
            distances.push_back(dist_f(dataset.X[i], dataset.X[j]));
        }
    }
    std::sort(distances.begin(), distances.end());
    return distances;
}

BOOST_PYTHON_MODULE (knn) {
    using namespace boost::python;
    using namespace core::knn;

    iterable_converter()
            .from_python<vector<double>>()
            .from_python<vector<vector<double>>>()
            .to_python<vector<double>>()
            .to_python<vector<vector<double>>>();

    class_<Dataset>("Dataset", init<matrix_>())
            .def("to_raw", &Dataset::to_raw)
            .def("X", &Dataset::get_X)
            .def("y", &Dataset::get_y)
            .def("n", &Dataset::get_n)
            .def("y_mean", &Dataset::y_mean);

    class_<NonParametricRegressor>("NonParametricRegressor", init<Dataset>())
            .def("predict", &NonParametricRegressor::predict)
            .def("vector_distances", &NonParametricRegressor::vector_distances)
            .def("loo_validate", &NonParametricRegressor::loo_validate);
}