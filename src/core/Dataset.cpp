//
// Created by Konstantin Rybkin on 28/11/2019.
//

#include "Dataset.h"

using namespace core::dataset;

Dataset::Dataset(matrix_ const &raw_data): n(raw_data.size()), m(raw_data[0].size() - 1) {
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

void Dataset::add_column(double val) {
    for (size_t i = 0; i < n; i++) {
        X[i].push_back(val);
    }
    m++;
}

void Dataset::print() {
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            cout << X[i][j] << " ";
        }
        cout << "| " << y[i] << endl;
    }
    cout << endl;
}

vector<Dataset::Scale> Dataset::normalize() {
    vector<Scale> scale_params(m, {0, 0});
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            scale_params[j].mean += X[i][j] / n;
        }
    }
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            scale_params[j].std_dev += pow(X[i][j] - scale_params[j].mean, 2) / n;
        }
    }
    for (size_t j = 0; j < m; j++) {
        scale_params[j].std_dev = sqrt(scale_params[j].std_dev);
    }
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            if (scale_params[j].std_dev == 0) {
                X[i][j] = 0;
            } else {
                X[i][j] = (X[i][j] - scale_params[j].mean) / scale_params[j].std_dev;
            }
        }
    }
    return scale_params;
}

Dataset::Scale Dataset::normalize_y() {
    Scale scale_param = {0, 0};
    for (size_t i = 0; i < n; i++) {
        scale_param.mean += y[i] / n;
    }
    for (size_t i = 0; i < n; i++) {
        scale_param.std_dev += pow(y[i] - scale_param.mean, 2) / n;
    }
    scale_param.std_dev = sqrt(scale_param.std_dev);
    for (size_t i = 0; i < n; i++) {
        if (scale_param.std_dev == 0) {
            y[i] = 0;
        } else {
            y[i] = (y[i] - scale_param.mean) / scale_param.std_dev;
        }
    }
    return scale_param;
}

bool Dataset::empty() {
    return n == 0 || m == 0;
}

vector_ core::dataset::operator+(vector_ const& v1, vector_ const& v2) {
    assert(v1.size() == v2.size());
    vector_ v3 = v1;
    for (size_t i = 0; i < v3.size(); i++) {
        v3[i] += v2[i];
    }
    return v3;
}

vector_ core::dataset::operator-(vector_ const& v1, vector_ const& v2) {
    assert(v1.size() == v2.size());
    vector_ v3 = v1;
    for (size_t i = 0; i < v3.size(); i++) {
        v3[i] -= v2[i];
    }
    return v3;
}

vector_ core::dataset::operator*(vector_ const& v1, vector_ const& v2) {
    assert(v1.size() == v2.size());
    vector_ v3 = v1;
    for (size_t i = 0; i < v3.size(); i++) {
        v3[i] *= v2[i];
    }
    return v3;
}

vector_ core::dataset::operator*(double s, vector_ const& v1) {
    vector_ v2 = v1;
    for (size_t i = 0; i < v1.size(); i++) {
        v2[i] *= s;
    }
    return v2;
}

double core::dataset::dot(vector_ const& v1, vector_ const& v2) {
    assert(v1.size() == v2.size());
    double sum = 0;
    for (size_t i = 0; i < v1.size(); i++) {
        sum += v1[i] * v2[i];
    }
    return sum;
}

vector_ core::dataset::dot(matrix_ const& m1, vector_ const& v2) {
    assert(m1[0].size() == v2.size());
    vector_ v3(m1.size());
    for (size_t i = 0; i < m1.size(); i++) {
        for (size_t j = 0; j < m1[0].size(); j++) {
            v3[i] += m1[i][j] * v2[j];
        }
    }
    return v3;
}

matrix_ core::dataset::transpose(matrix_ const& m) {
    matrix_ m2(m[0].size(), vector_(m.size()));

    for (size_t i = 0; i < m.size(); i++) {
        for (size_t j = 0; j < m[0].size(); j++) {
            m2[j][i] = m[i][j];
        }
    }
    return m2;
}

double core::dataset::vector_mean(vector_ const& v) {
    double mean = 0;
    for (double i : v) {
        mean += i;
    }
    return mean / v.size();
}

double core::dataset::vector_min(vector_ const& v) {
    double min_val = v[0];
    for (double i : v) {
        min_val = std::min(min_val, i);
    }
    return min_val;
}

double core::dataset::vector_max(vector_ const& v) {
    double max_val = v[0];
    for (double i : v) {
        max_val = std::max(max_val, i);
    }
    return max_val;
}

BOOST_PYTHON_MODULE (dataset) {
    using namespace boost::python;
    using namespace core::dataset;

    iterable_converter()
            .from_python<vector_>()
            .from_python<matrix_>()
            .to_python<vector_>()
            .to_python<matrix_>();

    class_<Dataset>("Dataset", init<matrix_>())
            .def("to_raw", &Dataset::to_raw)
            .def_readonly("X", &Dataset::get_X)
            .def_readonly("y", &Dataset::get_y)
            .def_readonly("n", &Dataset::n)
            .def_readonly("m", &Dataset::m)
            .def("y_mean", &Dataset::y_mean);
}