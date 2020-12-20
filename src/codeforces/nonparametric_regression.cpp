#include <iostream>
#include <utility>
#include <vector>

#define _USE_MATH_DEFINES

#include <cmath>
#include <iomanip>
#include <functional>

using namespace std;

typedef vector<vector<double>> matrix_;
typedef vector<double> vector_;

const double MAX_VAL = 100000000000;

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

private:
    matrix_ X;
    vector_ y;
};

class NonParametricRegressor {
public:
    explicit NonParametricRegressor(Dataset dataset);

    double predict(vector_ const &x, string const &distance, string const &kernel, string const &window,
                   double window_param);

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

Dataset::Dataset(matrix_ const &raw_data) {
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

matrix_ Dataset::get_X() {
    return this->X;
}

vector_ Dataset::get_y() {
    return this->y;
}

matrix_ Dataset::to_raw() {
    matrix_ raw_matrix(X);
    for (size_t i = 0; i < get_n(); i++) {
        raw_matrix[i].push_back(y[i]);
    }
    return raw_matrix;
}

double Dataset::y_mean() {
    double mean = 0;
    for (double i : y) {
        mean += i;
    }
    return mean / y.size();
}

NonParametricRegressor::NonParametricRegressor(Dataset dataset) : dataset(std::move(dataset)) {}

double NonParametricRegressor::predict(vector_ const &x, string const &distance,
                                       string const &kernel, string const &window, double window_param) {
    auto distance_f = choose_distance_metric(distance);
    auto kernel_f = choose_kernel(kernel);
    double numerator = 0, denominator = 0;
    if (window == "variable") {
        Dataset sorted_df = sort_by_distance(x, distance_f);
        window_param = distance_f(sorted_df.get_X()[int(window_param)], x);
    }
    for (size_t i = 0; i < dataset.get_n(); i++) {
        double dist_to_current = distance_f(dataset.get_X()[i], x);
        if (window_param != 0) {
            dist_to_current = dist_to_current / window_param;
        } else {
            if (dist_to_current != 0) {
                dist_to_current = MAX_VAL;
            }
        }
        double smoothed_distance = kernel_f(dist_to_current);
        numerator += dataset.get_y()[i] * smoothed_distance;
        denominator += smoothed_distance;
    }
    if (denominator == 0) return dataset.y_mean();
    return numerator / denominator;
}

Dataset NonParametricRegressor::sort_by_distance(vector_ const &x, NonParametricRegressor::vec_signature &dist_f) {
    matrix_ raw_data = this->dataset.to_raw();
    sort(raw_data.begin(), raw_data.end(), [this, dist_f, x](vector_ const &a, vector_ const &b) {
        return dist_f(a, x) < dist_f(b, x);
    });
    return Dataset(raw_data);
}

double NonParametricRegressor::manhattan(vector_ const &x, vector_ const &y) {
    size_t n = min(x.size(), y.size());
    double distance = 0;
    for (size_t i = 0; i < n; i++) {
        distance += abs(x[i] - y[i]);
    }
    return distance;
}

double NonParametricRegressor::euclidean(vector_ const &x, vector_ const &y) {
    size_t n = min(x.size(), y.size());
    double distance = 0;
    for (size_t i = 0; i < n; i++) {
        distance += (x[i] - y[i]) * (x[i] - y[i]);
    }
    return sqrt(distance);
}

double NonParametricRegressor::chebyshev(vector_ const &x, vector_ const &y) {
    size_t n = min(x.size(), y.size());
    double distance = 0;
    for (size_t i = 0; i < n; i++) {
        distance = max(distance, abs(x[i] - y[i]));
    }
    return distance;
}

double NonParametricRegressor::uniform(double u) {
    if (abs(u) >= 1) return 0;
    return 0.5;
}

double NonParametricRegressor::triangular(double u) {
    if (abs(u) >= 1) return 0;
    return 1. - abs(u);
}

double NonParametricRegressor::epanechnikov(double u) {
    if (abs(u) >= 1) return 0;
    return 0.75 * (1 - u * u);
}

double NonParametricRegressor::quartic(double u) {
    if (abs(u) >= 1) return 0;
    return (15.0 / 16.0) * pow(1 - u * u, 2);
}

double NonParametricRegressor::triweight(double u) {
    if (abs(u) >= 1) return 0;
    return (35.0 / 32.0) * pow(1 - u * u, 3);
}

double NonParametricRegressor::tricube(double u) {
    if (abs(u) >= 1) return 0;
    return (70.0 / 81.0) * pow(1 - abs(u * u * u), 3);
}

double NonParametricRegressor::gaussian(double u) {
    return (1. / sqrt(2 * M_PI)) * pow(M_E, -0.5 * u * u);
}

double NonParametricRegressor::cosine(double u) {
    if (abs(u) >= 1) return 0;
    return (M_PI / 4.0) * cos((M_PI / 2) * u);
}

double NonParametricRegressor::logistic(double u) {
    return 1.0 / (pow(M_E, u) + 2 + pow(M_E, -u));
}

double NonParametricRegressor::sigmoid(double u) {
    return (2 / M_PI) * (1.0 / (pow(M_E, u) + pow(M_E, -u)));
}

function<double(vector_ const &, vector_ const &)> NonParametricRegressor::choose_distance_metric(string const &name) {
    if (name == "manhattan") return &manhattan;
    if (name == "euclidean") return &euclidean;
    if (name == "chebyshev") return &chebyshev;
    return nullptr;
}

function<double(double)> NonParametricRegressor::choose_kernel(string const &name) {
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

int main() {
    int n, m;
    cin >> n >> m;
    matrix_ raw_data(n);
    for (size_t i = 0; i < n; i++) {
        raw_data[i].resize(m + 1);
        for (size_t j = 0; j <= m; j++) {
            cin >> raw_data[i][j];
        }
    }
    NonParametricRegressor regressor((Dataset(raw_data)));
    vector_ test_vector(m);
    for (int i = 0; i < m; i++) {
        cin >> test_vector[i];
    }

    string distance_f, kernel_f, window_f;
    double h;
    cin >> distance_f >> kernel_f >> window_f;
    cin >> h;

    cout << setprecision(20) << regressor.predict(test_vector, distance_f, kernel_f, window_f, h);
    return 0;
}