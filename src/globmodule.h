#ifndef __GLOBMODULE_H__
#define __GLOBMODULE_H__

#include <string>
#include <vector>
#include <iostream>
#include <cmath>

namespace PushPull {
    struct GlobalModule;

    struct Connection {
        GlobalModule *module;
        float value;
    };

    struct GlobalModule {
        std::string name;
        float x, y;
        float area, radius;
        bool fixed;
        float fx, fy, fw, fh;
        std::vector<Connection *> connections;
        GlobalModule(std::string in_name, float in_x, float in_y, float in_area, bool in_fixed);
        ~GlobalModule();
        void addConnection(GlobalModule *in_module, float in_value);
        void addFixedOutline(float fx, float fy, float fw, float fh);
    };
}

namespace RectGrad {
    class GlobalModule;

    class Connection {
    public:
        std::vector<GlobalModule *> modules;
        double value;
    };

    class GlobalModule {
    public:
        std::string name;
        double centerX, centerY;
        double x, y;
        int area;
        int width, height;
        bool fixed;
        std::vector<Connection *> connections;
        ~GlobalModule();
        void addConnection(const std::vector<GlobalModule *> &in_modules, double in_value);
        void updateCord(int DieWidth, int DieHeight, double sizeScalar);
    };

    class SoftModule : public GlobalModule {
    public:
        // for soft modules with specified width and height
        SoftModule(std::string name, double centerX, double centerY, int width, int height, int area);
        // for general soft modules
        SoftModule(std::string name, double centerX, double centerY, int area);
        ~SoftModule();
    };

    class FixedModule : public GlobalModule {
    public:
        // for fixed modules
        FixedModule(std::string name, int x, int y, int width, int height, int area);
        ~FixedModule();
    };
}

#endif