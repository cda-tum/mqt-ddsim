extern "C" double Microsoft__Quantum__Samples__Chemistry__SimpleVQE__GetEnergyHydrogenVQE__Interop(double, double, double, long);
extern "C" void Microsoft__Quantum__Samples__Chemistry__SimpleVQE__GetEnergyHydrogenVQE(double, double, double, long);

void get_energy(double theta1, double theta2, double theta3, long nsamples) {
    double jwTermEnergy = Microsoft__Quantum__Samples__Chemistry__SimpleVQE__GetEnergyHydrogenVQE__Interop(theta1, theta2, theta3, nsamples);
}

int main(int argc, char *argv[]) {
    double theta1 = 0.1;
    double theta2 = 0.2;
    double theta3 = 0.3;
    Microsoft__Quantum__Samples__Chemistry__SimpleVQE__GetEnergyHydrogenVQE(theta1, theta2, theta3, 1);
    return 0;
}