#include <iostream>
#include <vector>

#define _USE_MATH_DEFINES

#include <cmath>
#include <iomanip>
#include <functional>
#include <cassert>

using namespace std;

typedef vector<double> vector_;
typedef vector<vector<double>> matrix_;

vector_ operator+(vector_ const& v1, vector_ const& v2) {
    assert(v1.size() == v2.size());
    vector_ v3 = v1;
    for (size_t i = 0; i < v3.size(); i++) {
        v3[i] += v2[i];
    }
    return v3;
}

vector_ operator-(vector_ const& v1, vector_ const& v2) {
    assert(v1.size() == v2.size());
    vector_ v3 = v1;
    for (size_t i = 0; i < v3.size(); i++) {
        v3[i] -= v2[i];
    }
    return v3;
}

vector_ operator*(vector_ const& v1, vector_ const& v2) {
    assert(v1.size() == v2.size());
    vector_ v3 = v1;
    for (size_t i = 0; i < v3.size(); i++) {
        v3[i] *= v2[i];
    }
    return v3;
}

vector_ operator*(double s, vector_ const& v1) {
    vector_ v2 = v1;
    for (size_t i = 0; i < v1.size(); i++) {
        v2[i] *= s;
    }
    return v2;
}

double dot(vector_ const& v1, vector_ const& v2) {
    assert(v1.size() == v2.size());
    double sum = 0;
    for (size_t i = 0; i < v1.size(); i++) {
        sum += v1[i] * v2[i];
    }
    return sum;
}

vector_ dot(matrix_ const& m1, vector_ const& v2) {
    assert(m1[0].size() == v2.size());
    vector_ v3(m1.size());
    for (size_t i = 0; i < m1.size(); i++) {
        for (size_t j = 0; j < m1[0].size(); j++) {
            v3[i] += m1[i][j] * v2[j];
        }
    }
    return v3;
}

matrix_ transpose(matrix_ const& m) {
    matrix_ m2(m[0].size(), vector_(m.size()));

    for (size_t i = 0; i < m.size(); i++) {
        for (size_t j = 0; j < m[0].size(); j++) {
            m2[j][i] = m[i][j];
        }
    }
    return m2;
}

double vector_mean(vector_ const& v) {
    double mean = 0;
    for (double i : v) {
        mean += i;
    }
    return mean / v.size();
}

double vector_min(vector_ const& v) {
    double min_val = v[0];
    for (double i : v) {
        min_val = std::min(min_val, i);
    }
    return min_val;
}

double vector_max(vector_ const& v) {
    double max_val = v[0];
    for (double i : v) {
        max_val = std::max(max_val, i);
    }
    return max_val;
}

class Dataset {
public:
    int n, m;
    vector_ y;
    matrix_ X;

    struct Scale {
        double mean = 0, std_dev = 0;
    };

    Dataset();

    matrix_ to_raw();

    void add_column(double val);

    vector<Scale> normalize();

    Scale normalize_y();

    void print();

};

Dataset::Dataset(): n(0), m(0) {}

//Dataset::Dataset(matrix_ const &raw_data): n(raw_data.size()),
//                                           m(raw_data[0].size() - 1),
//                                           X(n, vector_(m)),
//                                           y(n) {
//    for (size_t i = 0; i < n; i++) {
//        for (size_t j = 0; j < m; j++) {
//            X[i][j] = raw_data[i][j];
//        }
//        y[i] = raw_data[i][m];
//    }
//}

matrix_ Dataset::to_raw() {
    matrix_ raw_matrix(X);
    for (size_t i = 0; i < n; i++) {
        raw_matrix[i].push_back(y[i]);
    }
    return raw_matrix;
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

void print(vector_ const& v, string const& title="") {
    cout << title << endl;
    for (double x : v) {
        cout << x << " ";
    }
    cout << endl << endl;
}

void print(matrix_ const& m, string const& title="") {
    cout << title << endl;
    for (const auto & row : m) {
        for (double el : row) {
            cout << el << " ";
        }
        cout << endl;
    }
    cout << endl;
}

class LinearRegression {
public:
    LinearRegression() = default;

    vector_ fit(Dataset& df, int n_epochs=3, double alpha=1);

    double evaluate(Dataset df, vector_ const& custom_theta=vector_());
private:
    vector_ theta;

    vector_ get_theta(const vector<Dataset::Scale>& X_scales, Dataset::Scale y_scales);
};

// adam
vector_ LinearRegression::fit(Dataset& df, int n_epochs, double alpha) {
    auto X_norm_params = df.normalize();
    auto y_norm_params = df.normalize_y();
    df.add_column(1);
    theta.assign(df.m, 0);

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
    }
    return get_theta(X_norm_params, y_norm_params);
}

double LinearRegression::evaluate(Dataset df, vector_ const& custom_theta) {
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

vector_ LinearRegression::get_theta(const vector<Dataset::Scale>& X_scales, Dataset::Scale y_scales) {
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


int main() {
    Dataset df;
    scanf("%d", &df.n);
    scanf("%d", &df.m);
    df.X.assign(df.n, vector_(df.m));
    df.y.assign(df.n, 0);
    int tmp;
    for (size_t i = 0; i < df.n; i++) {
        for (size_t j = 0; j < df.m; j++) {
            scanf("%d", &tmp);
            df.X[i][j] = tmp;
        }
        scanf("%d", &tmp);
        df.y[i] = tmp;
    }
    LinearRegression lr;
    vector_ coef = lr.fit(df, 100000, 0.1);
//    cout << lr.evaluate(df_copy, coef) << endl;
    for (double c : coef) {
        printf("%lf\n", c);
    }
    return 0;
}