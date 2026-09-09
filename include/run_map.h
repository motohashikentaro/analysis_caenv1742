#ifndef RUN_MAP_H
#define RUN_MAP_H

#include <map>
#include <string>

struct RunInfo{
    double energy;
    int nw;
};

inline const std::map<std::string, RunInfo> run_map = {
    {"05088", {2.0, 2}},
    {"05298", {2.0, 6}},
    {"05174", {5.0, 2}},
    {"05265", {5.0, 6}}
};

#endif