#include<iostream>
#include <math.h>
#include<fstream>
#include<string>
#include<cmath>
#include<cstdlib>
#include<iomanip>
#include <vector>
#include <algorithm>

// *** The code to calculate the dNch/deta and dN/2pipTdpTdy *** //
// *** from iSS binary output ***//
// Copyright @  Wenbin Zhao, 2023 //

using namespace std;
int main(int argc, char* argv[] )
{
    const int length = 120;
    double eta_low = -7.0, eta_upper = length/2.;
    const double rapidity_shift = 0.0; // in the proton going direction for pPb
    double dndptdypion[length] = {0.0}, dndptdyproton[length] = {0.0},dndptdykaon[length] = {0.0};
    double dndptdypionplus[length]={0.0}, dndptdypionmius[length]={0.0}, dndptdypion0[length] = {0.0};
    double dndptdyk0s[length]={0.0}, dndptdyLambda[length] = {0.0};
    double dndptdyLambdacms1[length]={0.0}, dndptdyLambdacms2[length] = {0.0};
    double dndptdyXsicms1[length]={0.0}, dndptdyXsicms2[length] = {0.0};
    double dndptdyOmegacms1[length]={0.0}, dndptdyOmegacms2[length] = {0.0};
    double dndptdetacharge[length]={0.0}, dNch_deta[length] = {0.0};
    double dE_all_deta[length] = {0.0}, dET_all_deta[length] = {0.0};
    int total_number_of_particles;
    int pid;
    int Nevent = 0;
    float px, py, pz, energy, mass, dummpx, dummpy, dummpz, dummpt;
    double stepp = 0.3, rapidity_cut = 0.5, eta_cut = 0.8, eta_step = 14./length;
    double add_piece = 1.0/rapidity_cut/2.0/stepp;
    double add_piece_eta = 1.0/eta_cut/2.0/stepp;

    double rapidity_cut_cms1 = -1.8, rapidity_cut_cms2 = 1.8;
    double add_piece_cms = 1.0/rapidity_cut_cms2/2.0/stepp;

    stringstream strparticle_name;
    strparticle_name << "particle_samples.bin";
    std::ifstream InStream;
    InStream.precision(15);
    string particle_name = strparticle_name.str();
    InStream.open(particle_name.c_str(), std::ios::in | std::ios::binary);

    while( InStream.read(reinterpret_cast<char*>(&total_number_of_particles), sizeof(int))) {
    //for (unsigned int iev = 0; iev < Nevent; iev++) {
        //InStream.read(reinterpret_cast<char*>(&total_number_of_particles), sizeof(int));
        for (auto i=0; i<total_number_of_particles; i++) {
            InStream.read(reinterpret_cast<char*>(&pid), sizeof(int));
            InStream.read(reinterpret_cast<char*>(&mass), sizeof(float));
            InStream.read(reinterpret_cast<char*>(&dummpt), sizeof(float));
            InStream.read(reinterpret_cast<char*>(&dummpx), sizeof(float));
            InStream.read(reinterpret_cast<char*>(&dummpy), sizeof(float));
            InStream.read(reinterpret_cast<char*>(&dummpz), sizeof(float));
            InStream.read(reinterpret_cast<char*>(&energy), sizeof(float));
            InStream.read(reinterpret_cast<char*>(&px), sizeof(float));
            InStream.read(reinterpret_cast<char*>(&py), sizeof(float));
            InStream.read(reinterpret_cast<char*>(&pz), sizeof(float));
            // calculate the pT-spectra
            double pT = sqrt(px*px + py*py);
            if (abs(rapidity_shift) > 0.0) {
                double pz_p = pz      * cosh(rapidity_shift) + energy * sinh(rapidity_shift);
                double e_p  = energy  * cosh(rapidity_shift) + pz * sinh(rapidity_shift);
                pz = pz_p;
                energy = e_p;
            }
	    double ET = sqrt(px*px + py*py + mass * mass);
            double rapidity = 0.5 * log((energy+pz) / (energy-pz));
            double pmg = sqrt(pT*pT + pz*pz);
            double sudorapidity = 0.5 * log((pmg+pz) / (pmg-pz));
            if (abs(rapidity) < rapidity_cut) {
                for (int kk = 0; kk < length; kk++) {
                    double midd1 = stepp*kk*1.0;
                    double midd2 = stepp*kk*1.0+stepp;
                    if ((pT > midd1) && (pT <= midd2)) {
                        if (abs(pid) == 111) dndptdypion0[kk] = dndptdypion0[kk] + add_piece;
                        if (pid == 211) dndptdypionplus[kk] = dndptdypionplus[kk] + add_piece;
                        if (pid == -211) dndptdypionmius[kk] = dndptdypionmius[kk] + add_piece;
                        if (abs(pid) == 321) dndptdykaon[kk] = dndptdykaon[kk] + add_piece/2.0;
                        if (abs(pid) == 2212) dndptdyproton[kk] = dndptdyproton[kk] + add_piece/2.0;
                        if (pid == 310) dndptdyk0s[kk] = dndptdyk0s[kk] + add_piece;
                        if ((abs(pid) == 3122) || (abs(pid) == 3212)) dndptdyLambda[kk] = dndptdyLambda[kk] + add_piece/2.0;
                    }
                }
            }
            
            if ((rapidity > rapidity_cut_cms1) && (rapidity <= 0.0) ) {
                for (int kk = 0; kk < length; kk++) {
                    double midd1 = stepp*kk*1.0;
                    double midd2 = stepp*kk*1.0+stepp;
                    if ((pT > midd1) && (pT <= midd2)) {
                        if ((abs(pid) == 3122) || (abs(pid) == 3212)) dndptdyLambdacms1[kk] = dndptdyLambdacms1[kk] + add_piece_cms/2.0;
                        if (abs(pid) == 3334) dndptdyOmegacms1[kk] = dndptdyOmegacms1[kk] + add_piece_cms/2.0;
                        if (abs(pid) == 3312) dndptdyXsicms1[kk] = dndptdyXsicms1[kk] + add_piece_cms/2.0;
                    }
                }
            }
            
            if ((rapidity < rapidity_cut_cms2) && (rapidity > 0.0) ) {
                for (int kk = 0; kk < length; kk++) {
                    double midd1 = stepp*kk*1.0;
                    double midd2 = stepp*kk*1.0+stepp;
                    if ((pT > midd1) && (pT <= midd2)) {
                        if ((abs(pid) == 3122) || (abs(pid) == 3212)) dndptdyLambdacms2[kk] = dndptdyLambdacms2[kk] + add_piece_cms/2.0;
                        if (abs(pid) == 3334) dndptdyOmegacms2[kk] = dndptdyOmegacms2[kk] + add_piece_cms/2.0;
                        if (abs(pid) == 3312) dndptdyXsicms2[kk] = dndptdyXsicms2[kk] + add_piece_cms/2.0;
                    }
                }
            }
            
            if (abs(pid) == 211 || abs(pid) == 321 || abs(pid) == 2212) {
                if (abs(sudorapidity) < eta_cut) {
                    for (int kk = 0; kk < length; kk++) {
                        double midd1 = stepp*kk*1.0;
                        double midd2 = stepp*kk*1.0+stepp;
                        if ((pT > midd1) && (pT <= midd2)) {
                            dndptdetacharge[kk] = dndptdetacharge[kk] + add_piece_eta;
                        }
                    }
                }
            }
            // calculate dNch/deta
            if (abs(pid) == 211 || abs(pid) == 321 || abs(pid) == 2212) {
                for (int ieta = 0; ieta < length; ieta++) {
                    double midd1 = eta_step*ieta*1.0 + eta_low;
                    double midd2 = eta_step*ieta*1.0 + eta_step + eta_low;
                    if ((sudorapidity > midd1) && (sudorapidity <= midd2)) {
                        dNch_deta[ieta] = dNch_deta[ieta] + 1.0/eta_step;
                        dE_all_deta[ieta] = dE_all_deta[ieta] + 1.0/eta_step*energy;
			dET_all_deta[ieta] = dET_all_deta[ieta] + 1.0/eta_step * ET;
                    }
                }
            }
        }
        Nevent++;
    }
    InStream.close();
    cout << " Nevent = " << Nevent << endl;

    //output the results to file
    char output_filename[128];
    sprintf(output_filename,"Soft_hadron_yield");
    ofstream output(output_filename);

    if( ! output.is_open() ) {
        cout << "cannot open output file:"<< endl
             << output_filename << endl;
        return -1;
    }
    output << " # " << "pT,  dN/2pidpTdy of " << " pion0 " << " " << " pionplus " << " " << " pionminus " << " " << " kaon_charge " << " " 
           << " p " << " " << " K0s " << " " << " Lambda;   "
           << " Lambda_cms1,  Lambda_cms2, " << " " << " Omega_cms1,  Omega_cms2,  " << " " << " Xsi_cms1,  Xsi_cms2, charged;   "
           << "eta,  dNch/deta,  dE/deta, dET/deta" << endl; 
    for (auto i = 0; i < length; ++i) {
        output << stepp*i*1.0+ stepp/2.0 << "  " << std::scientific << std::setprecision(4) << dndptdypion0[i]/Nevent/2./M_PI << "  " << dndptdypionplus[i]/Nevent/2./M_PI << "  " 
               << dndptdypionmius[i]/Nevent/2./M_PI << "  " << dndptdykaon[i]/Nevent/2./M_PI << "  " 
               << dndptdyproton[i]/Nevent/2./M_PI << "  " << dndptdyk0s[i]/Nevent/2./M_PI << "  "
               << dndptdyLambda[i]/Nevent/2./M_PI << "  " 
               << dndptdyLambdacms1[i]/Nevent/2./M_PI << "  " << dndptdyLambdacms2[i]/Nevent/2./M_PI << "  "
               << dndptdyOmegacms1[i]/Nevent/2./M_PI << "  " << dndptdyOmegacms2[i]/Nevent/2./M_PI << "  "
               << dndptdyXsicms1[i]/Nevent/2./M_PI << "  " << dndptdyXsicms2[i]/Nevent/2./M_PI << "  "
               << dndptdetacharge[i]/Nevent/2./M_PI << "  "
               << eta_step*i*1.0 + eta_low + eta_step/2.0 << "  " << dNch_deta[i]/Nevent << "  " << dE_all_deta[i]/Nevent << "  " << dET_all_deta[i]/Nevent
               << endl;
    }
    output.close();
}

