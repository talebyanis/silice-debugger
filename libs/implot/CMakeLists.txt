cmake_minimum_required(VERSION 2.6)
project(implot)

INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/../LibSL/src/libs/src/imgui/
)

add_library(implot
        implot.h
        implot.cpp
        )