#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int n, m, k;
    cin >> n >> m >> k;
    vector<pair<int, int>> objects(n);
    for (int i = 0; i < n; i++) {
        cin >> objects[i].first;
        objects[i].second = i + 1;
    }

    sort(objects.begin(), objects.end());
    vector<vector<int>> clusters(k);
    for (int i = 0; i < n; i++) {
        clusters[i % k].push_back(objects[i].second);
    }
    for (int i = 0; i < k; i++) {
        cout << clusters[i].size() << " ";
        for (int j : clusters[i]) {
            cout << j << " ";
        }
        cout << endl;
    }
    return 0;
}