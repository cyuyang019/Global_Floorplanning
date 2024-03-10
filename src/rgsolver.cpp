#include "rgsolver.h"


GlobalSolver::GlobalSolver() {
    std::srand(std::time(NULL));
    softModuleNum = 0;
    fixedModuleNum = 0;
    punishment = 0.05;
    maxAspectRatio = 2.;
    sizeScalar = 0;
    overlapped = true;
    timeStep = 0;
    toggle = 1;
}

GlobalSolver::~GlobalSolver() {
    // do nothing
    // dynamic memory will be freed by parser
}

void GlobalSolver::setOutline(int width, int height) {
    DieWidth = ( double ) width;
    DieHeight = ( double ) height;
    xMaxMovement = DieWidth / 4000.;
    yMaxMovement = DieHeight / 4000.;
}

void GlobalSolver::setSoftModuleNum(int num) {
    softModuleNum = num;
    moduleNum = softModuleNum + fixedModuleNum;
    xGradient.resize(moduleNum);
    yGradient.resize(moduleNum);
    wGradient.resize(moduleNum);
    hGradient.resize(moduleNum);
    xFirstMoment.resize(moduleNum, 0.);
    yFirstMoment.resize(moduleNum, 0.);
    xSecondMoment.resize(moduleNum, 0.);
    ySecondMoment.resize(moduleNum, 0.);
}

void GlobalSolver::setFixedModuleNum(int num) {
    fixedModuleNum = num;
    moduleNum = softModuleNum + fixedModuleNum;
    xGradient.resize(moduleNum);
    yGradient.resize(moduleNum);
    wGradient.resize(moduleNum);
    hGradient.resize(moduleNum);
    xFirstMoment.resize(moduleNum, 0.);
    yFirstMoment.resize(moduleNum, 0.);
    xSecondMoment.resize(moduleNum, 0.);
    ySecondMoment.resize(moduleNum, 0.);
}

void GlobalSolver::setConnectionNum(int num) {
    connectionNum = num;
}

void GlobalSolver::addModule(GlobalModule *in_module) {
    modules.push_back(in_module);
}

void GlobalSolver::addConnection(const std::vector<std::string> &in_modules, double in_value) {
    std::vector<GlobalModule *> connectedModules;

    for ( const std::string &modName : in_modules ) {
        for ( int i = 0; i < modules.size(); i++ ) {
            if ( modules[i]->name == modName ) {
                connectedModules.push_back(modules[i]);
            }
        }
    }

    for ( int i = 0; i < connectedModules.size(); ++i ) {
        std::vector<GlobalModule *> connModules;
        connModules = connectedModules;
        connModules.erase(connModules.begin() + i);
        connectedModules[i]->addConnection(connModules, in_value);
    }
}

void GlobalSolver::readFloorplan(Parser &parser) {
    setOutline(parser.getDieWidth(), parser.getDieHeight());
    setSoftModuleNum(parser.getSoftModuleNum());
    setFixedModuleNum(parser.getFixedModuleNum());
    setConnectionNum(parser.getConnectionNum());
    for ( int i = 0; i < this->modules.size(); i++ ) {
        delete this->modules[i];
    }
    this->modules.clear();
    for ( int i = 0; i < this->moduleNum; i++ ) {
        GlobalModule *newModule = parser.getModule(i);
        this->module2index[newModule] = i;
        this->modules.push_back(newModule);
    }
    double scalar = -1.;
    for ( int i = 0; i < connectionNum; i++ ) {
        ConnectionInfo *conn = parser.getConnection(i);
        if ( ( double ) conn->value > scalar ) {
            scalar = ( double ) conn->value;
        }
        this->connectionList.push_back(conn);
    }
    connectNormalize = 1. / scalar;
}

void GlobalSolver::readConfig(Parser &parser, std::string punishmentArg, std::string maxAspectRatioArg) {
    // specify punishment
    if ( punishmentArg != "" ) {
        this->punishment = std::stod(punishmentArg.c_str());
        std::cout << "[GlobalSolver] Note: Punishment is set to " << punishmentArg << std::endl;
    }
    else {
        if ( parser.getPunishment() != "" ) {
            std::string configPunishment = parser.getPunishment();
            std::cout << "[GlobalSolver] Note: Punishment is set to " << configPunishment << std::endl;
            this->punishment = std::stod(configPunishment.c_str());
        }
        else {
            std::cout << "[GlobalSolver] Note: Punishment is not set. Use default value 0.05." << std::endl;
            this->punishment = 0.05;
        }
    }

    // specify maximum aspect ratio
    double maxAspectRatio;
    if ( maxAspectRatioArg != "" ) {
        maxAspectRatio = std::stod(maxAspectRatioArg.c_str());
        std::cout << "[GlobalSolver] Note: Maximum Aspect Ratio is set to " << maxAspectRatioArg << std::endl;
    }
    else {
        if ( parser.getMaxAspectRatio() > 0.99 ) {
            maxAspectRatio = parser.getMaxAspectRatio();
            std::cout << "[GlobalSolver] Note: Maximum Aspect Ratio is set to " << maxAspectRatio << std::endl;
        }
        else {
            std::cout << "[GlobalSolver] Note: Maximum Aspect Ratio is not set. Use default value 2." << std::endl;
            maxAspectRatio = 2.;
        }
    }
    this->setMaxAspectRatio(maxAspectRatio);

    // specify shape constraint
    std::vector< std::vector<std::string> > shapeConstraintMods = parser.getShapeConstraints();
    for ( int i = 0; i < shapeConstraintMods.size(); ++i ) {
        std::cout << "[GlobalSolver] Note: Shape Constraint: ";
        this->sameShapeMods.push_back(std::vector<GlobalModule *>());
        for ( int j = 0; j < shapeConstraintMods[i].size(); ++j ) {
            if ( j != 0 ) {
                std::cout << ", ";
            }
            std::string tarModName = shapeConstraintMods[i][j];
            std::cout << tarModName;
            for ( GlobalModule *mod : this->modules ) {
                if ( mod->name == tarModName ) {
                    this->sameShapeMods.back().push_back(mod);
                    break;
                }
            }
        }
        std::cout << std::endl;
    }
    for ( int i = 0; i < this->sameShapeMods.size(); ++i ) {
        double area = sameShapeMods[i][0]->area;
        for ( GlobalModule *mod : sameShapeMods[i] ) {
            if ( std::abs(mod->area - area) > 1e-5 ) {
                std::cout << "[GlobalSolver] Warning: Modules with shape constraint should have same area." << std::endl;
            }
        }
    }
}

void GlobalSolver::currentPosition2txt(std::string file_name) {
    for ( GlobalModule *mod : modules ) {
        mod->updateCord(( int ) this->DieWidth, ( int ) this->DieHeight, 1.);
    }
    std::ofstream ostream(file_name);
    ostream << "BLOCK " << moduleNum << " CONNECTION " << connectionNum << std::endl;
    ostream << DieWidth << " " << DieHeight << std::endl;
    for ( GlobalModule *mod : modules ) {
        ostream << mod->name << " ";
        ostream << ( ( mod->fixed ) ? "FIXED" : "SOFT" ) << " ";
        ostream << mod->area << " ";
        ostream << mod->x << " " << mod->y << " ";
        ostream << mod->width << " " << mod->height << std::endl;

    }
    for ( int i = 0; i < connectionNum; i++ ) {
        ConnectionInfo *conn = connectionList[i];
        for ( int j = 0; j < conn->modules.size(); ++j ) {
            ostream << conn->modules[j] << " ";
        }
        ostream << conn->value << std::endl;
    }
    ostream.close();
}

void GlobalSolver::generateCluster() {
    std::cout << "[GlobalSolver] Note: Generating clusters." << std::endl;
    Cluster cluster(this->connectionList);
    cluster.louvain();
    std::vector<std::vector<GlobalModule *>> moduleCluster = cluster.getCluster();
    GlobalModule *mod1, *mod2;
    for ( int i = 0; i < moduleCluster.size(); ++i ) {
        for ( int j = 0; j < moduleCluster[i].size() - 1; ++j ) {
            for ( int k = j + 1; k < moduleCluster[i].size(); ++k ) {
                mod1 = moduleCluster[i][j];
                mod2 = moduleCluster[i][k];
                mod1->addConnection(std::vector<GlobalModule *> { mod2 }, 0.5 * this->connectNormalize);
                mod2->addConnection(std::vector<GlobalModule *> { mod1 }, 0.5 * this->connectNormalize);
            }
        }
    }
}

double GlobalSolver::calcDeadspace() {
    double dieArea = DieWidth * DieHeight;
    double moduleArea = 0;
    for ( int i = 0; i < moduleNum; i++ ) {
        moduleArea += modules[i]->area;
    }
    return 1. - moduleArea / dieArea;
}

void GlobalSolver::calcGradient() {
    GlobalModule *curModule;
    for ( int i = 0; i < moduleNum; i++ ) {
        if ( modules[i]->fixed == true ) {
            continue;
        }

        curModule = modules[i];
        double x_grad = 0;
        double y_grad = 0;
        double w_grad = 0;
        double h_grad = 0;

        // gradient for HPWL
        for ( int j = 0; j < curModule->connections.size(); j++ ) {
            double pullValue = curModule->connections[j]->value * connectNormalize;
            if ( curModule->connections[j]->modules.size() > 1 ) {
                int x_sign = 0, y_sign = 0;
                bool x_is_max = true;
                bool x_is_min = true;
                bool y_is_max = true;
                bool y_is_min = true;
                for ( GlobalModule *pullModule : curModule->connections[j]->modules ) {
                    if ( pullModule->centerX > curModule->centerX ) {
                        x_is_max = false;
                    }
                    if ( pullModule->centerX < curModule->centerX ) {
                        x_is_min = false;
                    }
                    if ( pullModule->centerY > curModule->centerY ) {
                        y_is_max = false;
                    }
                    if ( pullModule->centerY < curModule->centerY ) {
                        y_is_min = false;
                    }
                }
                if ( x_is_max ) {
                    ++x_sign;
                }
                if ( x_is_min ) {
                    --x_sign;
                }
                if ( y_is_max ) {
                    ++y_sign;
                }
                if ( y_is_min ) {
                    --y_sign;
                }
                x_grad += pullValue * x_sign;
                y_grad += pullValue * y_sign;
            }
            else {
                GlobalModule *pullModule = curModule->connections[j]->modules[0];
                double x_diff, y_diff;

                x_diff = curModule->centerX - pullModule->centerX;
                y_diff = curModule->centerY - pullModule->centerY;
                if ( x_diff == 0 && y_diff == 0 ) {
                    continue;
                }

                double x_sign = ( x_diff == 0 ) ? 0. : ( x_diff > 0 ) ? 1. : -1.;
                double y_sign = ( y_diff == 0 ) ? 0. : ( y_diff > 0 ) ? 1. : -1.;
                x_grad += pullValue * x_sign;
                y_grad += pullValue * y_sign;
            }

        }

        // gradient for overlapped area
        for ( int j = 0; j < moduleNum; j++ ) {
            if ( j == i ) {
                continue;
            }
            GlobalModule *pushModule = modules[j];

            // Calculate the movement gradient
            double overlappedWidth, overlappedHeight, x_diff, y_diff;

            x_diff = curModule->centerX - pushModule->centerX;
            y_diff = curModule->centerY - pushModule->centerY;
            if ( x_diff == 0 && y_diff == 0 ) {
                continue;
            }

            double curWidth = curModule->width;
            double pushWidth = pushModule->width;
            double curHeight = curModule->height;
            double pushHeight = pushModule->height;

            double max_xl = std::max(curModule->centerX - curWidth / 2., pushModule->centerX - pushWidth / 2.);
            double min_xr = std::min(curModule->centerX + curWidth / 2., pushModule->centerX + pushWidth / 2.);
            double max_yd = std::max(curModule->centerY - curHeight / 2., pushModule->centerY - pushHeight / 2.);
            double min_yu = std::min(curModule->centerY + curHeight / 2., pushModule->centerY + pushHeight / 2.);

            overlappedWidth = min_xr - max_xl;
            overlappedHeight = min_yu - max_yd;
            if ( overlappedWidth <= 0. || overlappedHeight <= 0. ) {
                continue;
            }

            bool width_cover = ( overlappedWidth >= curWidth || overlappedWidth >= pushWidth );
            bool height_cover = ( overlappedHeight >= curHeight || overlappedHeight >= pushHeight );

            if ( width_cover && !height_cover ) {
                overlappedHeight = 0;
            }
            else if ( height_cover && !width_cover ) {
                overlappedWidth = 0;
            }

            double x_sign = ( x_diff == 0 ) ? 0 : ( x_diff > 0 ) ? 1. : -1.;
            double y_sign = ( y_diff == 0 ) ? 0 : ( y_diff > 0 ) ? 1. : -1.;
            x_grad += -punishment * x_sign * overlappedHeight / DieHeight * DieWidth;
            y_grad += -punishment * y_sign * overlappedWidth;
            // double x_unitPush = x_sign * overlappedHeight / ( overlappedWidth * overlappedWidth + overlappedHeight * overlappedHeight );
            // double y_unitPush = y_sign * overlappedWidth / ( overlappedWidth * overlappedWidth + overlappedHeight * overlappedHeight );
            // x_grad += -punishment * x_unitPush;
            // y_grad += -punishment * y_unitPush;

            // Calculate the dimension gradient
            w_grad += overlappedHeight / DieHeight * DieWidth;
            h_grad += overlappedWidth;
        }

        xGradient[i] = x_grad;
        yGradient[i] = y_grad;
        wGradient[i] = w_grad;
        hGradient[i] = h_grad;
    }

    //for ( int i = 0; i < moduleNum; i++ ) {
    //    std::cout << modules[i]->name << ": " << xGradient[i] << " " << yGradient[i] << std::endl;
    //}
}

void GlobalSolver::gradientDescent(double lr, bool squeeze) {
    timeStep += 1;
    // move soft modules
    GlobalModule *curModule;
    for ( int i = 0; i < moduleNum; i++ ) {
        if ( modules[i]->fixed ) {
            continue;
        }

        curModule = modules[i];

        xFirstMoment[i] = 0.9 * xFirstMoment[i] + 0.1 * xGradient[i];
        yFirstMoment[i] = 0.9 * yFirstMoment[i] + 0.1 * yGradient[i];

        xSecondMoment[i] = 0.999 * xSecondMoment[i] + 0.001 * xGradient[i] * xGradient[i];
        ySecondMoment[i] = 0.999 * ySecondMoment[i] + 0.001 * yGradient[i] * yGradient[i];

        double xFirstMomentHat = xFirstMoment[i] / ( 1 - std::pow(0.9, timeStep) );
        double yFirstMomentHat = yFirstMoment[i] / ( 1 - std::pow(0.9, timeStep) );

        double xSecondMomentHat = xSecondMoment[i] / ( 1 - std::pow(0.999, timeStep) );
        double ySecondMomentHat = ySecondMoment[i] / ( 1 - std::pow(0.999, timeStep) );

        double xMovement = ( lr * xFirstMomentHat / ( std::sqrt(xSecondMomentHat) + 1e-8 ) ) * DieWidth;
        double yMovement = ( lr * yFirstMomentHat / ( std::sqrt(ySecondMomentHat) + 1e-8 ) ) * DieHeight;

        if ( std::abs(xMovement) > xMaxMovement ) {
            xMovement = ( xMovement > 0 ) ? xMaxMovement : -xMaxMovement;
        }
        if ( std::abs(yMovement) > yMaxMovement ) {
            yMovement = ( yMovement > 0 ) ? yMaxMovement : -yMaxMovement;
        }

        curModule->centerX -= xMovement;
        curModule->centerY -= yMovement;

        if ( curModule->centerX < curModule->width / 2. ) {
            curModule->centerX = curModule->width / 2.;
            wGradient[i] += curModule->height * 0.5;
        }
        if ( curModule->centerY < curModule->height / 2. ) {
            curModule->centerY = curModule->height / 2.;
            hGradient[i] += curModule->width * 0.5;
        }
        if ( curModule->centerX > DieWidth - curModule->width / 2. ) {
            curModule->centerX = DieWidth - curModule->width / 2.;
            wGradient[i] += curModule->height * 0.5;
        }
        if ( curModule->centerY > DieHeight - curModule->height / 2. ) {
            curModule->centerY = DieHeight - curModule->height / 2.;
            hGradient[i] += curModule->width * 0.5;
        }
    }

    // handle shape constraint
    for ( int i = 0; i < sameShapeMods.size(); ++i ) {
        double sharedWidthGrad = 0, sharedHeightGrad = 0;
        for ( GlobalModule *mod : sameShapeMods[i] ) {
            sharedWidthGrad += wGradient[module2index[mod]];
            sharedHeightGrad += hGradient[module2index[mod]];
        }
        sharedWidthGrad /= ( double ) sameShapeMods[i].size();
        sharedHeightGrad /= ( double ) sameShapeMods[i].size();
        for ( GlobalModule *mod : sameShapeMods[i] ) {
            wGradient[module2index[mod]] = sharedWidthGrad;
            hGradient[module2index[mod]] = sharedHeightGrad;
        }
    }

    if ( squeeze ) {
        for ( int i = 0; i < moduleNum; i++ ) {
            if ( modules[i]->fixed ) {
                continue;
            }
            curModule = modules[i];
            if ( toggle ) {
                curModule->setHeight(curModule->height - lr * hGradient[i]);
            }
            else {
                curModule->setWidth(curModule->width - lr * wGradient[i]);
            }
        }
    }
    toggle = !toggle;
}

void GlobalSolver::checkShapeConstraint() {
    for ( int i = 0; i < sameShapeMods.size(); ++i ) {
        double sharedWidth = 0, sharedHeight = 0;
        for ( GlobalModule *mod : sameShapeMods[i] ) {
            sharedWidth += mod->width;
            sharedHeight += mod->height;
        }
        sharedWidth /= ( double ) sameShapeMods[i].size();
        sharedHeight /= ( double ) sameShapeMods[i].size();
        for ( GlobalModule *mod : sameShapeMods[i] ) {
            mod->width = sharedWidth;
            mod->height = sharedHeight;
        }
    }
}

void GlobalSolver::roundToInteger() {
    for ( GlobalModule *mod : modules ) {
        if ( mod->fixed ) {
            continue;
        }
        if ( DieWidth > DieHeight ) {
            mod->width = std::round(mod->width);
            mod->height = std::ceil(mod->area / mod->width);
            if ( mod->height / mod->width > maxAspectRatio ) {
                mod->height -= 1.;
                mod->width = std::ceil(mod->area / mod->height);
            }
            else if ( mod->height / mod->width < 1 / maxAspectRatio ) {
                mod->width -= 1.;
                mod->height = std::ceil(mod->area / mod->width);
            }
        }
        else {
            mod->height = std::round(mod->height);
            mod->width = std::ceil(mod->area / mod->height);
            if ( mod->height / mod->width > maxAspectRatio ) {
                mod->height -= 1.;
                mod->width = std::ceil(mod->area / mod->height);
            }
            else if ( mod->height / mod->width < 1 / maxAspectRatio ) {
                mod->width -= 1.;
                mod->height = std::ceil(mod->area / mod->width);
            }
        }
    }
}

double GlobalSolver::calcEstimatedHPWL() {
    double HPWL = 0;
    for ( int i = 0; i < connectionNum; ++i ) {
        ConnectionInfo *conn = connectionList[i];
        double maxX = 0., maxY = 0.;
        double minX = 1e10, minY = 1e10;
        for ( GlobalModule *mod : conn->modulePtrs ) {
            maxX = ( mod->centerX > maxX ) ? mod->centerX : maxX;
            minX = ( mod->centerX < minX ) ? mod->centerX : minX;
            maxY = ( mod->centerY > maxY ) ? mod->centerY : maxY;
            minY = ( mod->centerY < minY ) ? mod->centerY : minY;
        }
        HPWL += ( maxX - minX + maxY - minY ) * conn->value;
    }
    return HPWL;
}

void GlobalSolver::setSizeScalar(double scalar, bool overlap_aware) {
    // scalar: new sizeScalar
    if ( scalar <= sizeScalar || !overlap_aware ) {
        for ( GlobalModule *mod : modules ) {
            mod->scaleSize(scalar);
        }
        sizeScalar = scalar;
        return;
    }

    std::vector<double> areaToGrow(moduleNum);
    std::vector<std::vector<double>> direcGrad(moduleNum);
    for ( int i = 0; i < moduleNum; ++i ) {
        areaToGrow[i] = modules[i]->area * ( scalar - sizeScalar );
        direcGrad[i].resize(4); // 0: top, 1: right, 2: bottom, 3: left
    }

    int iterationNum = 8;
    for ( int it = 0; it < iterationNum; ++it ) {
        // calculate gradient
        for ( int i = 0; i < moduleNum; ++i ) {
            if ( modules[i]->fixed ) {
                continue;
            }
            GlobalModule *curModule = modules[i];
            // gradient for overlapped area
            for ( int j = 0; j < moduleNum; j++ ) {
                if ( j == i ) {
                    continue;
                }
                GlobalModule *pushModule = modules[j];

                // Calculate the dimension gradient
                double overlappedWidth, overlappedHeight;

                double curWidth = curModule->width;
                double pushWidth = ( pushModule->fixed ) ? pushModule->width : pushModule->width;
                double curHeight = curModule->height;
                double pushHeight = ( pushModule->fixed ) ? pushModule->height : pushModule->height;

                double max_xl = std::max(curModule->centerX - curWidth / 2., pushModule->centerX - pushWidth / 2.);
                double min_xr = std::min(curModule->centerX + curWidth / 2., pushModule->centerX + pushWidth / 2.);
                double max_yd = std::max(curModule->centerY - curHeight / 2., pushModule->centerY - pushHeight / 2.);
                double min_yu = std::min(curModule->centerY + curHeight / 2., pushModule->centerY + pushHeight / 2.);

                overlappedWidth = min_xr - max_xl;
                overlappedHeight = min_yu - max_yd;
                if ( overlappedWidth <= 0. || overlappedHeight <= 0. ) {
                    continue;
                }

                bool width_cover = ( overlappedWidth >= curWidth || overlappedWidth >= pushWidth );
                bool height_cover = ( overlappedHeight >= curHeight || overlappedHeight >= pushHeight );

                if ( width_cover && !height_cover ) {
                    overlappedHeight = 0;
                }
                else if ( height_cover && !width_cover ) {
                    overlappedWidth = 0;
                }

                if ( curModule->centerY > pushModule->centerY ) {        // bottom
                    direcGrad[i][2] += overlappedWidth;
                }
                else if ( curModule->centerY < pushModule->centerY ) {   // top
                    direcGrad[i][0] += overlappedWidth;
                }
                if ( curModule->centerX > pushModule->centerX ) {        // left
                    direcGrad[i][3] += overlappedHeight;
                }
                else if ( curModule->centerX < pushModule->centerX ) {   // right
                    direcGrad[i][1] += overlappedHeight;
                }
            }
        }
        // handle shape constraint
        for ( int i = 0; i < sameShapeMods.size(); ++i ) {
            double sharedBottomGrad = 0, sharedTopGrad = 0, sharedLeftGrad = 0, sharedRightGrad = 0;
            for ( GlobalModule *mod : sameShapeMods[i] ) {
                sharedTopGrad += direcGrad[module2index[mod]][0];
                sharedRightGrad += direcGrad[module2index[mod]][1];
                sharedBottomGrad += direcGrad[module2index[mod]][2];
                sharedLeftGrad += direcGrad[module2index[mod]][3];
            }
            sharedTopGrad /= ( double ) sameShapeMods[i].size();
            sharedRightGrad /= ( double ) sameShapeMods[i].size();
            sharedBottomGrad /= ( double ) sameShapeMods[i].size();
            sharedLeftGrad /= ( double ) sameShapeMods[i].size();
            for ( GlobalModule *mod : sameShapeMods[i] ) {
                direcGrad[module2index[mod]][0] = sharedTopGrad;
                direcGrad[module2index[mod]][1] = sharedRightGrad;
                direcGrad[module2index[mod]][2] = sharedBottomGrad;
                direcGrad[module2index[mod]][3] = sharedLeftGrad;
            }
        }
        // grow soft modules
        for ( int i = 0; i < moduleNum; ++i ) {
            if ( modules[i]->fixed ) {
                continue;
            }
            GlobalModule *curModule = modules[i];
            double curAreaToGrow = areaToGrow[i] / iterationNum;
            if ( direcGrad[i][0] < 1e-5 && direcGrad[i][1] < 1e-5 && direcGrad[i][2] < 1e-5 && direcGrad[i][3] < 1e-5 ) {
                // grow evenly
                curModule->setArea(curModule->area + curAreaToGrow);
            }
            else {
                double max_val = std::max({ direcGrad[i][0], direcGrad[i][1], direcGrad[i][2], direcGrad[i][3] });
                direcGrad[i][0] /= ( max_val / 5 );
                direcGrad[i][1] /= ( max_val / 5 );
                direcGrad[i][2] /= ( max_val / 5 );
                direcGrad[i][3] /= ( max_val / 5 );
                double denom = std::exp(-direcGrad[i][0]) + std::exp(-direcGrad[i][1]) + std::exp(-direcGrad[i][2]) + std::exp(-direcGrad[i][3]);
                // std::cerr << direcGrad[i][0] << " " << direcGrad[i][1] << " " << direcGrad[i][2] << " " << direcGrad[i][3] << " " << denom << std::endl;
                double topGrowRatio = std::exp(-direcGrad[i][0]) / denom;
                double rightGrowRatio = std::exp(-direcGrad[i][1]) / denom;
                double bottomGrowRatio = std::exp(-direcGrad[i][2]) / denom;
                double leftGrowRatio = std::exp(-direcGrad[i][3]) / denom;
                // grow top
                double topGrowthHeight = curAreaToGrow * topGrowRatio / curModule->width;
                curModule->growHeight(topGrowthHeight);
                curModule->centerY += topGrowthHeight / 2;
                // grow right
                double rightGrowthWidth = curAreaToGrow * rightGrowRatio / curModule->height;
                curModule->growWidth(rightGrowthWidth);
                curModule->centerX += rightGrowthWidth / 2;
                // grow bottom
                double bottomGrowthHeight = curAreaToGrow * bottomGrowRatio / curModule->width;
                curModule->growHeight(bottomGrowthHeight);
                curModule->centerY -= bottomGrowthHeight / 2;
                // grow left
                double leftGrowthWidth = curAreaToGrow * leftGrowRatio / curModule->height;
                curModule->growWidth(leftGrowthWidth);
                curModule->centerX -= leftGrowthWidth / 2;
            }
        }
    }

    // minimize error
    for ( GlobalModule *mod : modules ) {
        mod->scaleSize(scalar);
    }
    sizeScalar = scalar;
}

void GlobalSolver::setPunishment(double force) {
    punishment = force;
}

void GlobalSolver::setupPunishment(double amplification) {
    double maxConnection = 0;
    for ( int i = 0; i < moduleNum; i++ ) {
        GlobalModule *curModule = modules[i];
        double connection = 0;
        for ( int j = 0; j < curModule->connections.size(); j++ ) {
            connection += curModule->connections[j]->value;
        }
        if ( connection > maxConnection ) {
            maxConnection = connection;
        }
    }
    punishment = maxConnection * amplification * connectNormalize;
    // std::cout << "Set push force = " << punishment << std::endl;
}

void GlobalSolver::setMaxAspectRatio(double aspect_ratio) {
    maxAspectRatio = aspect_ratio;
    for ( GlobalModule *mod : modules ) {
        mod->setMaxAspectRatio(aspect_ratio);
    }
}

void GlobalSolver::setMaxMovement(double ratio) {
    xMaxMovement = DieWidth * ratio;
    yMaxMovement = DieHeight * ratio;
}

bool GlobalSolver::hasOverlap() {
    for ( GlobalModule *mod : modules ) {
        mod->updateCord(DieWidth, DieHeight, 1.);
    }
    overlapped = false;
    for ( int i = 0; i < moduleNum - 1; i++ ) {
        for ( int j = i + 1; j < moduleNum; j++ ) {
            GlobalModule *mod1 = modules[i];
            GlobalModule *mod2 = modules[j];
            double overlappedWidth, overlappedHeight, x_diff, y_diff;

            x_diff = mod1->centerX - mod2->centerX;
            y_diff = mod1->centerY - mod2->centerY;

            overlappedWidth = ( mod1->width + mod2->width ) / 2.0 - std::abs(x_diff);
            overlappedHeight = ( mod1->height + mod2->height ) / 2.0 - std::abs(y_diff);

            if ( overlappedWidth > 0. && overlappedHeight > 0. ) {
                overlapped = true;
            }
        }
    }
    return overlapped;
}

void GlobalSolver::squeezeToFit() {
    for ( GlobalModule *mod : modules ) {
        mod->updateCord(DieWidth, DieHeight, 1.);
    }

    std::vector<double> squeezeWidthVec(this->moduleNum, 0);
    std::vector<double> squeezeHeightVec(this->moduleNum, 0);

    for ( int i = 0; i < this->moduleNum; ++i ) {
        GlobalModule *curModule = modules[i];
        if ( curModule->fixed ) {
            continue;
        }
        double totalOverlapWidth = 0.0;
        double totalOverlapHeight = 0.0;
        for ( GlobalModule *tarModule : modules ) {
            if ( curModule == tarModule ) {
                continue;
            }
            double overlappedWidth, overlappedHeight;

            double mod1Width = curModule->width;
            double mod2Width = tarModule->width;
            double mod1Height = curModule->height;
            double mod2Height = tarModule->height;

            double max_xl = std::max(curModule->centerX - mod1Width / 2., tarModule->centerX - mod2Width / 2.);
            double min_xr = std::min(curModule->centerX + mod1Width / 2., tarModule->centerX + mod2Width / 2.);
            double max_yd = std::max(curModule->centerY - mod1Height / 2., tarModule->centerY - mod2Height / 2.);
            double min_yu = std::min(curModule->centerY + mod1Height / 2., tarModule->centerY + mod2Height / 2.);

            overlappedWidth = min_xr - max_xl;
            overlappedHeight = min_yu - max_yd;

            if ( overlappedWidth > 0. && overlappedHeight > 0. ) {
                totalOverlapWidth += overlappedWidth;
                totalOverlapHeight += overlappedHeight;
            }
        }
        if ( totalOverlapWidth > 0. && totalOverlapHeight > 0. ) {
            double aspectRatio = ( double ) totalOverlapHeight / totalOverlapWidth;
            if ( aspectRatio > 10. ) {
                squeezeWidthVec[i] = totalOverlapWidth;
            }
            else if ( aspectRatio < 0.1 ) {
                squeezeHeightVec[i] = totalOverlapHeight;
            }
            // else {
            //     aspectRatio = 0.2 * std::atan(aspectRatio - 1) + 1;
            //     if ( aspectRatio > 2. ) {
            //         aspectRatio = 2.;
            //     }
            //     else if ( aspectRatio < 0.5 ) {
            //         aspectRatio = 0.5;
            //     }
            //     curModule->width = std::ceil(std::sqrt(curModule->area / aspectRatio));
            //     curModule->height = std::ceil(std::sqrt(curModule->area * aspectRatio));
            // }
        }
    }
    for ( int i = 0; i < this->moduleNum; ++i ) {
        GlobalModule *curModule = modules[i];
        if ( curModule->fixed ) {
            continue;
        }
        if ( squeezeWidthVec[i] > 0. ) {
            curModule->setWidth(curModule->width - squeezeWidthVec[i]);
        }
        else if ( squeezeHeightVec[i] > 0. ) {
            curModule->setHeight(curModule->height - squeezeHeightVec[i]);
        }
        // assert(curModule->height * curModule->width >= curModule->area);
        // assert(0.5 <= curModule->height / curModule->width && curModule->height / curModule->width <= 2);
        curModule->updateCord(DieWidth, DieHeight, 1.);
    }
}

bool GlobalSolver::isAreaLegal() {
    for ( GlobalModule *mod : modules ) {
        if ( mod->fixed ) {
            continue;
        }
        if ( mod->width * mod->height < mod->area ) {
            std::cout << "[GlobalSolver] " << mod->name << ": ";
            std::cout << mod->width << " x " << mod->height << " < " << mod->area << std::endl;
            return false;
        }
    }
    return true;
}

bool GlobalSolver::isAspectRatioLegal() {
    double maxAR = ( maxAspectRatio > 2 ) ? maxAspectRatio : 2;
    for ( GlobalModule *mod : modules ) {
        if ( mod->fixed ) {
            continue;
        }
        if ( mod->height / mod->width > maxAR ) {
            std::cout << "[GlobalSolver] " << mod->name << ": ";
            std::cout << mod->height << " / " << mod->width << " > " << maxAR << std::endl;
            return false;
        }
        else if ( mod->height / mod->width < 1. / maxAR ) {
            std::cout << "[GlobalSolver] " << mod->name << ": ";
            std::cout << mod->height << " / " << mod->width << " < " << 1. / maxAR << std::endl;
            return false;
        }
    }
    return true;
}

void GlobalSolver::resetOptimizer() {
    xFirstMoment.resize(moduleNum, 0.);
    yFirstMoment.resize(moduleNum, 0.);
    xSecondMoment.resize(moduleNum, 0.);
    ySecondMoment.resize(moduleNum, 0.);
    timeStep = 0;
}

void GlobalSolver::reportOverlap() {
    double totalOverlapArea = 0;
    double totalArea = 0;
    printf("╔═════════ Overlap Report ═════════╗\n");
    printf("║   Module Name   │      Area      ║\n");
    printf("╟─────────────────┼────────────────╢\n");

    for ( int i = 0; i < moduleNum - 1; i++ ) {
        for ( int j = i + 1; j < moduleNum; j++ ) {
            GlobalModule *mod1 = modules[i];
            GlobalModule *mod2 = modules[j];

            if ( mod1->fixed && mod2->fixed ) {
                continue;
            }

            double overlappedWidth, overlappedHeight;

            double mod1Width = mod1->width;
            double mod2Width = mod2->width;
            double mod1Height = mod1->height;
            double mod2Height = mod2->height;

            double max_xl = std::max(mod1->centerX - mod1Width / 2., mod2->centerX - mod2Width / 2.);
            double min_xr = std::min(mod1->centerX + mod1Width / 2., mod2->centerX + mod2Width / 2.);
            double max_yd = std::max(mod1->centerY - mod1Height / 2., mod2->centerY - mod2Height / 2.);
            double min_yu = std::min(mod1->centerY + mod1Height / 2., mod2->centerY + mod2Height / 2.);

            overlappedWidth = min_xr - max_xl;
            overlappedHeight = min_yu - max_yd;

            if ( overlappedWidth > 0. && overlappedHeight > 0. ) {
                // std::cout << mod1->name << " & " << mod2->name << "\t: " << overlappedWidth * overlappedHeight << std::endl;
                printf("║ %6s & %-6s │ %14.2lf ║\n", mod1->name.c_str(), mod2->name.c_str(), overlappedWidth * overlappedHeight);
                // std::cout << "Mod1 Width: " << mod1Width << " Height: " << mod1Height << std::endl;
                // std::cout << "Mod2 Width: " << mod2Width << " Height: " << mod2Height << std::endl;
                // std::cout << std::endl;
                totalOverlapArea += overlappedWidth * overlappedHeight;
            }
        }
    }

    for ( GlobalModule *&mod : modules ) {
        if ( !mod->fixed ) {
            totalArea += ( double ) mod->area;
        }
    }

    printf("╠═════════════════╪════════════════╣\n");
    printf("║  Soft Mod Area  │ %14.2lf ║\n", totalArea);
    printf("║  Overlap Area   │ %14.2lf ║\n", totalOverlapArea);
    printf("║  Overlap Ratio  │      %8.5lf%% ║\n", totalOverlapArea / totalArea * 100);
    printf("╚═════════════════╧════════════════╝\n");
}

void GlobalSolver::reportDeadSpace() {
    double totalArea = DieWidth * DieHeight;
    double moduleArea = 0;
    for ( int i = 0; i < moduleNum; i++ ) {
        moduleArea += modules[i]->area;
    }
    printf("╔═══════ Dead Space Report ════════╗\n");
    printf("║   Chip Area     │ %14.0lf ║\n", totalArea);
    printf("║   Module Area   │ %14.0lf ║\n", moduleArea);
    printf("║   Dead Space    │      %8.5lf%% ║\n", ( 1. - moduleArea / totalArea ) * 100);
    printf("╚═════════════════╧════════════════╝\n");
}
