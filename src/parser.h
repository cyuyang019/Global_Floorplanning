#ifndef PARSER_H
#define PARSER_H

#include "globmodule.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_set>

struct ConnectionInfo {
    std::vector<std::string> modules;
    std::vector<GlobalModule *> modulePtrs;
    int value;
    ConnectionInfo(const std::vector<std::string> &modules, int value) {
        this->modules = modules;
        this->value = value;
    }
};

class Parser {
private:
    bool configExists;
    // info of floorplan
    int DieWidth, DieHeight;
    int softModuleNum, fixedModuleNum, moduleNum, connectionNum;
    std::vector<GlobalModule *> modules;
    std::vector<ConnectionInfo *> connectionList;
    // info of solver
    std::string punishment;
    double max_aspect_ratio;
    double lr;
    std::vector< std::vector<std::string> > ShapeConstraintMods;
public:
    Parser();
    ~Parser();
    bool read_input(std::string file_name);
    bool read_config(std::string file_name);
    // info of floorplan
    int getDieWidth();
    int getDieHeight();
    int getSoftModuleNum();
    int getFixedModuleNum();
    int getModuleNum();
    int getConnectionNum();
    GlobalModule *getModule(int index);
    ConnectionInfo *getConnection(int index);
    // info of solver
    std::string getPunishment();
    double getMaxAspectRatio();
    double getLearnRate();
    std::vector< std::vector<std::string> > getShapeConstraints();
};

#endif