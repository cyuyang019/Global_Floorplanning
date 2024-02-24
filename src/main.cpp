#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cfloat>
#include <string>
#include <unistd.h>
#include <ctime>
#include "parser.h"
#include "ppsolver.h"
#include "rgsolver.h"

namespace rg = RectGrad;

int main(int argc, char *argv[]) {
    // parsing command line arguments
    std::string inputFileName;
    std::string outputFileName;
    double punishmentArg = -1.;
    double maxAspectRatioArg = -1.;

    int opt;

    // optstring "hc:o:" means:
    // h is an option that doesn't require an argument
    // c: is an option that requires an argument (indicated by the colon)
    // o: is another option that requires an argument
    while ( ( opt = getopt(argc, argv, "hi:o:p:a:") ) != -1 ) {
        switch ( opt ) {
        case 'h':
            std::cout << "Usage: " << argv[0] << " [-h] [-i <input>] [-o <output>] [-p <punishment>] [-a <aspect_ratio>]\n";
            return 0;
        case 'i':
            std::cout << "[GlobalSolver] Note: Input file is set to " << optarg << std::endl;
            inputFileName = optarg;
            break;
        case 'o':
            std::cout << "[GlobalSolver] Note: Output file is set to " << optarg << std::endl;
            outputFileName = optarg;
            break;
        case 'p':
            std::cout << "[GlobalSolver] Note: Punishment is set to " << optarg << std::endl;
            punishmentArg = std::stod(optarg);
            break;
        case 'a':
            std::cout << "[GlobalSolver] Note: Maximum aspect ratio is set to " << optarg << std::endl;
            maxAspectRatioArg = std::stod(optarg);
            break;
        case '?':
            if ( optopt == 'i' || optopt == 'o' || optopt == 'p' || optopt == 'a' )
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

    // Start measuring CPU time
    struct timespec start, end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

    // parse the input file
    rg::Parser parser(inputFileName);
    rg::GlobalSolver solver;
    solver.readFromParser(parser);

    // specify gradient descent parameters
    int iteration = 1000;
    double lr = 5e-4;
    solver.setMaxMovement(0.001);
    solver.setPullWhileOverlap(true);

    // specify punishment
    if ( punishmentArg <= 0. ) {
        std::cout << "[GlobalSolver] Note: Punishment is not set. Use default value 0.05.\n";
    }
    double punishment = ( punishmentArg > 0. ) ? punishmentArg : 0.05;
    solver.setPunishment(punishment);

    // specify maximum aspect ratio
    if ( maxAspectRatioArg <= 0. ) {
        std::cout << "[GlobalSolver] Note: Maximum aspect ratio is not set. Use default value 2.\n";
    }
    double maxAspectRatio = ( maxAspectRatioArg > 0. ) ? maxAspectRatioArg : 2.;
    solver.setMaxAspectRatio(maxAspectRatio);

    // apply gradient descent
    for ( int phase = 1; phase <= 50; phase++ ) {
        // solver.setPunishment(punishment * phase);
        if ( phase > 1 ) {
            std::cout << "\r";
        }
        std::cout << "[GlobalSolver] Phase " << std::setw(2) << phase << " / 50" << std::flush;
        solver.setSizeScalar(phase * 0.02 * phase * 0.02, phase >= 40);
        solver.resetOptimizer();
        for ( int i = 0; i < iteration; i++ ) {
            solver.calcGradient();
            solver.gradientDescent(lr, phase >= 40);
        }
        // solver.currentPosition2txt("animation/" + std::to_string(phase) + ".txt");
    }

    solver.roundToInteger();

    solver.resetOptimizer();
    for ( int i = 0; i < 1000; i++ ) {
        solver.calcGradient();
        solver.gradientDescent(lr);
    }

    // Stop measuring CPU time
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    
    // report the result
    std::cout << std::endl << std::endl;
    solver.reportDeadSpace();
    std::cout << std::endl;
    solver.reportOverlap();
    std::cout << std::endl;

    if ( !solver.isAreaLegal() ) {
        std::cout << "[GlobalSolver] ERROR: Area Constraint Violated.\n";
    }
    else {
        std::cout << "[GlobalSolver] Note: Area Constraint Met.\n";
    }

    if ( !solver.isAspectRatioLegal() ) {
        std::cout << "[GlobalSolver] ERROR: Aspect Ratio Constraint Violated.\n";
    }
    else {
        std::cout << "[GlobalSolver] Note: Aspect Ratio Constraint Met.\n";
    }

    solver.currentPosition2txt(outputFileName);
    std::cout << std::fixed;
    std::cout << "[GlobalSolver] Estimated HPWL: " << std::setprecision(2) << solver.calcEstimatedHPWL() << std::endl;
    double elapsed = ( end.tv_sec - start.tv_sec ) + ( end.tv_nsec - start.tv_nsec ) / 1e9;

    std::cout << "[GlobalSolver] CPU time used: " << elapsed << " seconds." << std::endl;

    return 0;
}