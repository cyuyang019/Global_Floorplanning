#ifndef PARSER_H
#define PARSER_H

#include "globmodule.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_set>

namespace PushPull {
    struct ConnectionInfo {
        std::string m0;
        std::string m1;
        float value;
        ConnectionInfo(std::string m0, std::string m1, float value) {
            this->m0 = m0;
            this->m1 = m1;
            this->value = value;
        }
    };

    class Parser {
    private:
        float DieWidth, DieHeight;
        int softModuleNum, fixedModuleNum, moduleNum, connectionNum;
        std::vector<GlobalModule> modules;
        std::vector<ConnectionInfo> connectionList;
    public:
        Parser();
        Parser(std::string file_name);
        ~Parser();
        void read_input(std::string file_name);
        float getDieWidth();
        float getDieHeight();
        int getSoftModuleNum();
        int getFixedModuleNum();
        int getModuleNum();
        int getConnectionNum();
        GlobalModule getModule(int index);
        ConnectionInfo getConnection(int index);
        std::vector<ConnectionInfo> getConnectionList() const;
    };
}


namespace RectGrad {
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
} // namespace RectGrad

#endif