//
// Created by Konstantin Rybkin on 08/11/2019.
//

#include "F1Score.h"

double core::metrics::F1Score::f1(double precision, double recall) {
    return safe_div(2 * precision * recall, precision + recall);
}

double core::metrics::F1Score::f1_macro(const matrix_ &cm) {
    int n = cm.size();
    vector_ precision(n), recall(n), class_weights = calc_class_weights(cm);

    for (int i = 0; i < n; i++) {
        int positive_sum = 0;
        for (int j = 0; j < n; j++) {
            positive_sum += cm[j][i];
        }
        precision[i] = safe_div(cm[i][i], positive_sum);
    }

    for (int i = 0; i < n; i++) {
        int positive_sum = 0;
        for (int j = 0; j < n; j++) {
            positive_sum += cm[i][j];
        }
        recall[i] = safe_div(cm[i][i], positive_sum);
    }
    double macro_precision = weighted_mean(precision, class_weights),
            macro_recall = weighted_mean(recall, class_weights);
    return f1(macro_precision, macro_recall);
}

vector_ core::metrics::F1Score::calc_class_weights(const vector<vector<double>> &cm) {
    int n = cm.size();
    vector_ weights(n);
    for (int i = 0; i < n; i++) {
        weights[i] = vector_sum(cm[i]);
    }
    return weights;
}

double core::metrics::F1Score::vector_sum(const vector_ &v) {
    double cum_sum = 0;
    for (double x : v) {
        cum_sum += x;
    }
    return cum_sum;
}

double core::metrics::F1Score::weighted_mean(const vector_ &v, const vector_ &weights) {
    double sum = 0;
    for (int i = 0; i < (int) v.size(); i++) {
        sum += v[i] * weights[i];
    }
    return sum / vector_sum(weights);
}

double core::metrics::F1Score::f1_micro(const matrix_ &cm) {
    int n = cm.size();
    double precision_numerator, precision_denominator;
    double recall_numerator, recall_denominator;
    vector_ class_weights = calc_class_weights(cm);
    double sum_classes = vector_sum(class_weights);
    vector_ f1_scores(n);

    for (int i = 0; i < n; i++) {
        precision_numerator = recall_numerator = cm[i][i];
        precision_denominator = 0, recall_denominator = 0;
        for (int j = 0; j < n; j++) {
            precision_denominator += cm[j][i];
        }
        for (int j = 0; j < n; j++) {
            recall_denominator += cm[i][j];
        }
        double precision = safe_div(precision_numerator, precision_denominator);
        double recall = safe_div(recall_numerator, recall_denominator);
        f1_scores[i] = f1(precision, recall) * class_weights[i] / sum_classes;
    }
    return vector_sum(f1_scores);
}

BOOST_PYTHON_MODULE (metrics) {
    using namespace boost::python;
    using namespace core::metrics;

    iterable_converter()
            .from_python<vector<double>>()
            .from_python<vector<vector<double>>>();
    class_<F1Score>("F1Score")
            .def("f1", &F1Score::f1)
            .def("f1_macro", &F1Score::f1_macro)
            .def("f1_micro", &F1Score::f1_micro);
}
