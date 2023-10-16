#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cfloat>
#include <stdio.h>
#include "parser.h"
#include "ppsolver.h"
#include "rgsolver.h"

namespace rg = RectGrad;

int main(int argc, char const *argv[]) {
    rg::Parser parser(argv[1]);
    rg::GlobalSolver solver;
    solver.readFromParser(parser);

    int iteration = 1000;
    double lr = 5e-4;
    solver.setMaxMovement(0.001);
    solver.setPullWhileOverlap(true);
    
    double punishmentValue = 0.05;
    double toleranceValue = 0.;
    solver.setPunishment(punishmentValue);

    for ( int phase = 1; phase <= 50; phase++ ) {
        // std::cout << "Phase " << phase << std::endl;
        solver.setSizeScalar(phase * 0.02);
        solver.setOverlapTolaranceLen(toleranceValue * phase * 0.02);
        solver.resetOptimizer();
        for ( int i = 0; i < iteration; i++ ) {
            solver.calcGradient();
            solver.gradientDescent(lr);
        }
    }

    solver.setPullWhileOverlap(false);
    solver.setMaxMovement(1e-4);
    solver.setPunishment(1e6);
    solver.setOverlapTolaranceLen(0.);
    solver.setSizeScalar(1.);
    lr = 1e-4;
    int count = 0;
    while ( solver.hasOverlap() ) {
        solver.squeezeToFit();
        solver.resetOptimizer();
        for ( int i = 0; i < 50; i++ ) {
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

    solver.currentPosition2txt(parser, argv[2]);
    std::cout << std::fixed;
    std::cout << "[GlobalSolver] Estimated HPWL: " << std::setprecision(2) << solver.calcEstimatedHPWL() << std::endl;

    return 0;
}