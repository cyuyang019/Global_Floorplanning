#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cfloat>
#include <string>
#include "parser.h"
#include "ppsolver.h"
#include "rgsolver.h"

namespace rg = RectGrad;

int main(int argc, char const *argv[]) {
    std::string inputFileName = argv[1];
    std::string outputFileName = argv[2];
    rg::Parser parser(inputFileName);
    rg::GlobalSolver solver;
    solver.readFromParser(parser);

    int iteration = 1000;
    double lr = 5e-4;
    solver.setMaxMovement(0.001);
    solver.setPullWhileOverlap(true);

    double punishmentValue = 0.05;
    solver.setPunishment(punishmentValue);

    for ( int phase = 1; phase <= 50; phase++ ) {
        // solver.setPunishment(punishmentValue * phase);
        solver.setSizeScalar(phase * 0.02);
        solver.resetOptimizer();
        for ( int i = 0; i < iteration; i++ ) {
            solver.calcGradient();
            solver.gradientDescent(lr);
        }
        // solver.currentPosition2txt(parser, outputFileName + "." + std::to_string(phase));
    }

    // for ( int i = 0; i < iteration; i++ ) {
    //     solver.setPunishment(0.005);
    //     solver.calcGradient();
    //     solver.gradientDescent(lr);
    // }

    solver.setPullWhileOverlap(false);
    solver.setMaxMovement(0.001);
    solver.setSizeScalar(1.);
    lr = 5e-4;
    int count = 0;
    while ( solver.hasOverlap() ) {
        solver.squeezeToFit();
        solver.resetOptimizer();
        for ( int i = 0; i < 100; i++ ) {
            solver.calcGradient();
            solver.gradientDescent(lr);
        }

        if ( ++count >= 5 ) {
            break;
        }
    }

    if ( !solver.isAreaLegal() ) {
        std::cout << "[GlobalSolver] ERROR: Area Constraint Violated.\n";
    }
    else {
        std::cout << "[GlobalSolver] Note: Area Constraint Met.\n";
    }

    solver.currentPosition2txt(parser, outputFileName);
    std::cout << std::fixed;
    std::cout << "[GlobalSolver] Estimated HPWL: " << std::setprecision(2) << solver.calcEstimatedHPWL() << std::endl;

    return 0;
}