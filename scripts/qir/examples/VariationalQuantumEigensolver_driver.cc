// ---------------------------------------------------------------------------
// NWQSim: Northwest Quantum Simulation Environment
// ---------------------------------------------------------------------------
// Ang Li, Senior Computer Scientist
// Pacific Northwest National Laboratory(PNNL), U.S.
// Homepage: http://www.angliphd.com
// GitHub repo: http://www.github.com/pnnl/SV-Sim
// PNNL-IPID: 32166, ECCN: EAR99, IR: PNNL-SA-161181
// BSD Lincese.
// ---------------------------------------------------------------------------
#include <cassert>
#include <iostream>
#include <memory>
#include <mpi.h>
#include "QirRuntimeApi_I.hpp"
#include "QirContext.hpp"

extern "C" double Microsoft__Quantum__Samples__Chemistry__SimpleVQE__GetEnergyHydrogenVQE__Interop(double, double, double, long);
extern "C" Microsoft::Quantum::IRuntimeDriver* GetDMSim();
extern "C" void Microsoft__Quantum__Samples__Chemistry__SimpleVQE__GetEnergyHydrogenVQE(double, double, double, long);

void get_energy(double theta1, double theta2, double theta3, long nsamples)
{
    int i_gpu = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &i_gpu);
    double jwTermEnergy = Microsoft__Quantum__Samples__Chemistry__SimpleVQE__GetEnergyHydrogenVQE__Interop(theta1, theta2, theta3, nsamples);
    if (i_gpu == 0)
    {
        std::cout << "*** Testing QIR VQE example with DM-Sim ***" << std::endl;
        std::cout << "\n===============================\n";
        std::cout << "VQE_jwTermEnergy is " << jwTermEnergy << std::endl;
        std::cout << "===============================\n";
        //std::cout << jwTermEnergy << std::endl;
    }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    double theta1 = atof(argv[1]);
    double theta2 = atof(argv[2]);
    double theta3 = atof(argv[3]);
    Microsoft::Quantum::IRuntimeDriver* dmsim = GetDMSim();
    Microsoft::Quantum::InitializeQirContext(dmsim, false);
    Microsoft__Quantum__Samples__Chemistry__SimpleVQE__GetEnergyHydrogenVQE(theta1, theta2, theta3, 1);
    MPI_Finalize();
    return 0;
}