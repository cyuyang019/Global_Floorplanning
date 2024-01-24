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
    GlobalModule::GlobalModule() {
        // Empty
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
        this->connections.push_back(nc);
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

    void GlobalModule::setWidth(double width) {
        // Empty
    }
    void GlobalModule::setHeight(double height) {
        // Empty
    }
    void GlobalModule::setArea(double area) {
        // Empty
    }
    void GlobalModule::scaleSize(double ratio) {
        // Empty
    }


    SoftModule::SoftModule() {
        // Empty
    }

    SoftModule::SoftModule(std::string name, double centerX, double centerY, int width, int height, int area) {
        this->name = name;
        this->centerX = centerX;
        this->centerY = centerY;
        this->width = width;
        this->height = height;
        this->area = area;
        this->fixed = false;
    }

    SoftModule::SoftModule(std::string name, double centerX, double centerY, int area) {
        this->name = name;
        this->centerX = centerX;
        this->centerY = centerY;
        this->area = area;
        this->width = std::ceil(std::sqrt(( double ) area));
        this->height = std::ceil(std::sqrt(( double ) area));
        this->fixed = false;
    }

    SoftModule::~SoftModule() {
        // Empty
    }

    void SoftModule::setWidth(double width) {
        double aspectRatio = this->currentArea / width / width;
        if ( aspectRatio > 2. ) {
            aspectRatio = 2.;
        }
        else if ( aspectRatio < 0.5 ) {
            aspectRatio = 0.5;
        }
        if ( std::abs(this->currentArea - this->area) < 1e-8 ) {
            this->width = std::round(std::sqrt(this->currentArea / aspectRatio));
            this->height = std::ceil(this->currentArea / this->width);
        }
        else {
            // this->width = std::sqrt(this->currentArea / aspectRatio);
            // this->height = this->currentArea / this->width;
            this->width = std::round(std::sqrt(this->currentArea / aspectRatio));
            this->height = std::ceil(this->currentArea / this->width);
        }
        while ( this->height / this->width > 2. ) {
            this->height -= 1.;
            this->width = std::ceil(this->currentArea / this->height);
        }
        while ( this->height / this->width < 0.5 ) {
            this->width -= 1.;
            this->height = std::ceil(this->currentArea / this->width);
        }
    }

    void SoftModule::setHeight(double height) {
        double aspectRatio = height * height / this->currentArea;
        if ( aspectRatio > 2 ) {
            aspectRatio = 2;
        }
        else if ( aspectRatio < 0.5 ) {
            aspectRatio = 0.5;
        }
        if ( std::abs(this->currentArea - this->area) < 1e-8 ) {
            this->height = std::round(std::sqrt(this->currentArea * aspectRatio));
            this->width = std::ceil(this->currentArea / this->height);
        }
        else {
            // this->height = std::sqrt(this->currentArea * aspectRatio);
            // this->width = this->currentArea / this->height;
            this->height = std::round(std::sqrt(this->currentArea * aspectRatio));
            this->width = std::ceil(this->currentArea / this->height);
        }
        while ( this->height / this->width > 2. ) {
            this->height -= 1.;
            this->width = std::ceil(this->currentArea / this->height);
        }
        while ( this->height / this->width < 0.5 ) {
            this->width -= 1.;
            this->height = std::ceil(this->currentArea / this->width);
        }
    }

    void SoftModule::setArea(double area) {
        this->currentArea = area;
        this->width = std::sqrt(area / ( this->height / this->width ));
        this->height = area / this->width;
    }

    void SoftModule::scaleSize(double ratio) {
        this->currentArea = this->area * ratio;
        if ( std::abs(ratio - 1.) < 1e-8 ) {
            this->width = std::round(std::sqrt(this->currentArea / ( this->height / this->width )));
            this->height = std::ceil(this->currentArea / this->width);
        }
        else {
            // this->width = std::sqrt(this->currentArea / ( this->height / this->width ));
            // this->height = this->currentArea / this->width;
            this->width = std::round(std::sqrt(this->currentArea / ( this->height / this->width )));
            this->height = std::ceil(this->currentArea / this->width);
        }
        while ( this->height / this->width > 2. ) {
            this->height -= 1.;
            this->width = std::ceil(this->currentArea / this->height);
        }
        while ( this->height / this->width < 0.5 ) {
            this->width -= 1.;
            this->height = std::ceil(this->currentArea / this->width);
        }
    }


    FixedModule::FixedModule() {
        // Empty
    }

    FixedModule::FixedModule(std::string name, int x, int y, int width, int height, int area) {
        this->name = name;
        this->x = ( double ) x;
        this->y = ( double ) y;
        this->width = width;
        this->height = height;
        this->area = area;
        this->centerX = x + width / 2.;
        this->centerY = y + height / 2.;
        this->fixed = true;
        // std::cout << "Fixed Module " << name << " " << x << " , " << y << " , " << width << " , " << height << std::endl;
    }

    FixedModule::~FixedModule() {
        // Empty
    }


} // namespace RectGrad
