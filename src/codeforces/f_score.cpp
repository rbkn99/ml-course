#include <iostream>
#include <vector>

using namespace std;

double safe_div(double a, double b) {
    if (b == 0) return 0;
    return a / b;
}

double vector_sum(const vector<double>& v) {
    double cum_sum = 0;
    for (double x : v) {
        cum_sum += x;
    }
    return cum_sum;
}

double weighted_mean(const vector<double>& v, const vector<double>& weights) {
    double sum = 0;
    for (int i = 0; i < (int)v.size(); i++) {
        sum += v[i] * weights[i];
    }
    return sum / vector_sum(weights);
}

double f1_score(double precision, double recall) {
    return safe_div(2 * precision * recall, precision + recall);
}

vector<double> calc_class_weights(const vector<vector<double>>& cm) {
    int n = cm.size();
    vector<double> weights(n);
    for (int i = 0; i < n; i++) {
        weights[i] = vector_sum(cm[i]);
    }
    return weights;
}

double macro_f1(const vector<vector<double>>& cm) {
    int n = cm.size();
    vector<double> precision(n), recall(n), class_weights = calc_class_weights(cm);

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
    return f1_score(macro_precision, macro_recall);
}

double micro_f1(const vector<vector<double>>& cm) {
    int n = cm.size();
    double precision_numerator, precision_denominator;
    double recall_numerator, recall_denominator;
    vector<double> class_weights = calc_class_weights(cm);
    double sum_classes = vector_sum(class_weights);
    vector<double> f1_scores(n);

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
        f1_scores[i] = f1_score(precision, recall) * class_weights[i] / sum_classes;
    }
    return vector_sum(f1_scores);
}

int main() {
    int k;
    cin >> k;
    vector<vector<double>> cm(k);
    for (int i = 0; i < k; i++) {
        cm[i].resize(k);
        for (int j = 0; j < k; j++) {
            cin >> cm[i][j];
        }
    }
    cout << macro_f1(cm) << endl;
    cout << micro_f1(cm) << endl;
    return 0;
}