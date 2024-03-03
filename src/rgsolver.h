#ifndef RGSOLVER_H
#define RGSOLVER_H

#include "globmodule.h"
#include "parser.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cassert>

namespace RectGrad {
    class GlobalSolver {
    private:
        double DieWidth, DieHeight;
        int softModuleNum, fixedModuleNum, moduleNum, connectionNum;
        std::vector<GlobalModule *> modules;
        std::unordered_map<GlobalModule *, int> module2index;
        std::vector<ConnectionInfo *> connectionList;
        std::vector<double> xGradient, yGradient;
        std::vector<double> wGradient, hGradient;
        std::vector<double> xFirstMoment, yFirstMoment;
        std::vector<double> xSecondMoment, ySecondMoment;
        std::vector< std::vector<GlobalModule *> > sameShapeMods;
        double xMaxMovement, yMaxMovement;
        double sizeScalar;
        double punishment;
        double maxAspectRatio;
        double connectNormalize;
        bool toggle;
        bool overlapped;
        int timeStep;
    public:
        GlobalSolver();
        ~GlobalSolver();
        void setOutline(int width, int height);
        void setSoftModuleNum(int num);
        void setFixedModuleNum(int num);
        void setConnectionNum(int num);
        void addModule(GlobalModule *in_module);
        void addConnection(const std::vector<std::string> &in_modules, double in_value);
        void readFloorplan(Parser &parser);
        void readConfig(Parser &parser, std::string punishmentArg, std::string maxAspectRatioArg);
        void calcGradient();
        void gradientDescent(double lr, bool squeeze = false);
        void currentPosition2txt(std::string file_name);
        void roundToInteger();
        double calcDeadspace();
        double calcEstimatedHPWL();
        void setSizeScalar(double scalar, bool overlap_aware = false);
        void setPunishment(double force);
        void setupPunishment(double amplification = 1.);
        void setMaxAspectRatio(double aspect_ratio);
        void setMaxMovement(double ratio = 0.001);
        bool hasOverlap();
        void squeezeToFit();
        bool isAreaLegal();
        bool isAspectRatioLegal();
        void resetOptimizer();
        void reportOverlap();
        void reportDeadSpace();
    };
} // namespace RectGrad


#endif
