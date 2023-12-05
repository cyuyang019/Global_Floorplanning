#include "globmodule.h"

namespace PushPull {
    GlobalModule::GlobalModule(std::string in_name, float in_x, float in_y, float in_area, bool in_fixed) {
        name = in_name;
        x = in_x;
        y = in_y;
        area = in_area;
        fixed = in_fixed;
        radius = std::sqrt(area / 3.1415926);
        fx = 0;
        fy = 0;
        fw = 0;
        fh = 0;
    }

    GlobalModule::~GlobalModule() {
        for ( int i = 0; i < connections.size(); i++ ) {
            delete connections[i];
        }
        //std::cout << "GlobalModule Deleted Successfully." << std::endl;
    }

    void GlobalModule::addConnection(GlobalModule *in_module, float in_value) {
        Connection *nc = new Connection;
        nc->module = in_module;
        nc->value = in_value;
        connections.push_back(nc);
    }

    void GlobalModule::addFixedOutline(float fx, float fy, float fw, float fh) {
        this->fx = fx;
        this->fy = fy;
        this->fw = fw;
        this->fh = fh;
    }
} // namespace PushPull


namespace RectGrad {
    GlobalModule::GlobalModule(std::string name, double centerX, double centerY, int width, int height, int area, bool is_fixed) {
        this->name = name;
        this->centerX = centerX;
        this->centerY = centerY;
        this->width = width;
        this->height = height;
        this->area = area;
        fixed = is_fixed;
    }

    GlobalModule::GlobalModule(std::string name, double centerX, double centerY, int area, bool is_fixed) {
        this->name = name;
        this->centerX = centerX;
        this->centerY = centerY;
        this->area = area;
        fixed = is_fixed;
        width = std::ceil(std::sqrt(( double ) area));
        height = std::ceil(std::sqrt(( double ) area));
    }

    GlobalModule::GlobalModule(std::string name, int x, int y, int width, int height, int area, bool is_fixed) {
        this->name = name;
        this->x = ( double ) x;
        this->y = ( double ) y;
        this->width = width;
        this->height = height;
        this->area = area;
        fixed = is_fixed;
        this->centerX = x + width / 2.;
        this->centerY = y + height / 2.;
        // std::cout << "Fixed Module " << name << " " << x << " , " << y << " , " << width << " , " << height << std::endl;
    }

    GlobalModule::~GlobalModule() {
        for ( int i = 0; i < connections.size(); i++ ) {
            delete connections[i];
        }
    }

    void GlobalModule::addConnection(const std::vector<GlobalModule *> &in_modules, double in_value) {
        Connection *nc = new Connection;
        nc->modules = in_modules;
        nc->value = in_value;
        connections.push_back(nc);
    }

    void GlobalModule::updateCord(int DieWidth, int DieHeight, double sizeScalar) {
        if ( this->fixed ) {
            return;
        }

        // this->x = (int) ( std::round(centerX - width * sizeScalar / 2.) );
        // this->y = (int) ( std::round(centerY - height * sizeScalar / 2.) );

        this->x = centerX - width * sizeScalar / 2.;
        this->y = centerY - height * sizeScalar / 2.;

        if ( this->x < 0 ) {
            this->x = 0;
        }
        else if ( this->x > DieWidth - this->width ) {
            this->x = DieWidth - this->width;
        }
        if ( this->y < 0 ) {
            this->y = 0;
        }
        else if ( this->y > DieHeight - this->height ) {
            this->y = DieHeight - this->height;
        }

        this->centerX = this->x + this->width * sizeScalar / 2.;
        this->centerY = this->y + this->height * sizeScalar / 2.;

        // std::cout << this->name << " " << this->width << " , " << this->height << std::endl;
        // std::cout << this->centerX << " , " << this->centerY << std::endl;
    }
} // namespace RectGrad
