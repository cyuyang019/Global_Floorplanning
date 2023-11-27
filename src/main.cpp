#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cfloat>
#include <string>
#include <unistd.h>
#include "parser.h"
#include "ppsolver.h"
#include "rgsolver.h"

namespace rg = RectGrad;

int main(int argc, char *argv[]) {
    // parsing command line arguments
    std::string inputFileName;
    std::string outputFileName;
    double punishment = -1.;

    int opt;

    // optstring "hc:o:" means:
    // h is an option that doesn't require an argument
    // c: is an option that requires an argument (indicated by the colon)
    // o: is another option that requires an argument
    while ( ( opt = getopt(argc, argv, "hi:o:p:") ) != -1 ) {
        switch ( opt ) {
        case 'h':
            std::cout << "Usage: " << argv[0] << " [-h] [-i <input>] [-o <output>] [-p <punishment>]\n";
            return 0;
        case 'i':
            std::cout << "Input file set to " << optarg << std::endl;
            inputFileName = optarg;
            break;
        case 'o':
            std::cout << "Output file set to " << optarg << std::endl;
            outputFileName = optarg;
            break;
        case 'p':
            std::cout << "Punishment set to " << optarg << std::endl;
            punishment = std::stod(optarg);
            break;
        case '?':
            if ( optopt == 'i' || optopt == 'o' || optopt == 'p')
                std::cerr << "Option -" << static_cast< char >( optopt ) << " requires an argument." << std::endl;
            else if ( isprint(optopt) )
                std::cerr << "Unknown option `-" << static_cast< char >( optopt ) << "`." << std::endl;
            else
                std::cerr << "Unknown option character `\\x" << std::hex << optopt << "`." << std::endl;
            return 1;
        default:
            abort();
        }
    }

    // Process any additional arguments that were not option flags.
    for ( int index = optind; index < argc; index++ ) {
        std::cout << "Non-option argument " << argv[index] << std::endl;
    }


    rg::Parser parser(inputFileName);
    rg::GlobalSolver solver;
    solver.readFromParser(parser);

    // applying gradient descent
    int iteration = 1000;
    double lr = 5e-4;
    solver.setMaxMovement(0.001);
    solver.setPullWhileOverlap(true);

    double punishmentValue = ( punishment > 0. ) ? punishment : 0.05;
    solver.setPunishment(punishmentValue);

    for ( int phase = 1; phase <= 50; phase++ ) {
        // solver.setPunishment(punishmentValue * phase);
        if ( phase > 1 ) {
            std::cout << "\r";
        }
        std::cout << "[GlobalSolver] Phase " << std::setw(2) << phase << " / 50" << std::flush;
        solver.setSizeScalar(phase * 0.02);
        solver.resetOptimizer();
        for ( int i = 0; i < iteration; i++ ) {
            solver.calcGradient();
            solver.gradientDescent(lr);
        }
        // solver.currentPosition2txt(outputFileName + "." + std::to_string(phase));
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
        // solver.currentPosition2txt(outputFileName + "." + std::to_string(51));

        if ( ++count >= 5 ) {
            break;
        }
    }

    // report the result
    std::cout << std::endl << std::endl;
    std::cout << "######### Overlap Report ##########" << std::endl;
    solver.reportOverlap();
    std::cout << std::endl;
    std::cout << "######## Dead Space Report ########" << std::endl;
    solver.reportDeadSpace();
    std::cout << std::endl;

    if ( !solver.isAreaLegal() ) {
        std::cout << "[GlobalSolver] ERROR: Area Constraint Violated.\n";
    }
    else {
        std::cout << "[GlobalSolver] Note: Area Constraint Met.\n";
    }

    solver.currentPosition2txt(outputFileName);
    std::cout << std::fixed;
    std::cout << "[GlobalSolver] Estimated HPWL: " << std::setprecision(2) << solver.calcEstimatedHPWL() << std::endl;

    return 0;
}