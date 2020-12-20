cmake_minimum_required(VERSION 2.8.3)
project(core)

PYTHON_ADD_MODULE(metrics F1Score.h F1Score.cpp)
PYTHON_ADD_MODULE(knn NonParametricRegressor.h NonParametricRegressor.cpp)