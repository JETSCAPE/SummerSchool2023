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
// *** from X-SCAPE hard hadrons output ***//
// Copyright @  Wenbin Zhao, 2023 //

using namespace std;
int main(int argc, char* argv[] )
{
    const int Nevent = 2000;
    const int length = 120;
    const double rapidity_shift = 0.0; // in the proton going direction
    double eta_low = -7.0;
    double eta_upper = length/2.;
    double dndptdypion[length] = {0.0}, dndptdyproton[length] = {0.0},dndptdykaon[length] = {0.0};
    double dndptdypionplus[length]={0.0}, dndptdypionmius[length]={0.0}, dndptdypion0[length] = {0.0};
    double dndptdyk0s[length]={0.0}, dndptdyLambda[length] = {0.0};
    double dndptdetacharge[length]={0.0}, dNch_deta[length] = {0.0}, dNch_deta_fir_ev[length] = {0.0};
    double dET_deta[length] = {0.0}, dET_deta_fir_ev[length] = {0.0};
    double dE_deta[length] = {0.0};
    
    double dndptdyLambdacms1[length]={0.0}, dndptdyLambdacms2[length] = {0.0};
    double dndptdyXsicms1[length]={0.0}, dndptdyXsicms2[length] = {0.0};
    double dndptdyOmegacms1[length]={0.0}, dndptdyOmegacms2[length] = {0.0};
    
    int total_number_of_particles, pid, event_id, int_temp, status, Ncoll, Npart;
    double px, py, pz, energy, mass, dummpx, dummpy, dummpz, dummpt, weight, b;
    double sigmaGen, sigmaErr, pThat, sigmaGen_ev;
    double stepp = 0.3, rapidity_cut = 0.5, eta_cut = 0.8, eta_step = 14./length;
    double add_piece = 1.0/rapidity_cut/2.0/stepp;
    double add_piece_eta = 1.0/eta_cut/2.0/stepp;
    
    double rapidity_cut_cms1 = -1.8, rapidity_cut_cms2 = 1.8;
    double add_piece_cms = 1.0/rapidity_cut_cms2/2.0/stepp;
    
    //std::vector<std::pair<float>> weight_array;
    char* stemp1;
    char** stemp2;

    const int length_pt = 29;
    double pT_array[] = {0.0, 0.1, 0.2, 0.4, 0.6, 0.8, 1.2, 1.6, 2.0, 2.5, 3.0, 3.5, 4.0,
                         5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 12., 14., 16., 18, 20., 22., 25, 30.,
                         35, 40};

    // read Ncoll from 3DGlauber output
    char inputstringfile[128];
    sprintf(inputstringfile, "strings_event_0.dat");
    FILE* instringfile;
    instringfile = fopen(inputstringfile,"r");
    fscanf(instringfile,"%s %s %s %lf %s %s %s %d %s %s %d",&stemp1, &stemp2,
           &stemp2, &b, &stemp2, &stemp2, &stemp2, &Npart, &stemp2, 
           &stemp2, &Ncoll);
    fclose(instringfile);

    char inputfile[128];
    sprintf(inputfile, "test_out_final_state_hadrons.dat");
    FILE* infile;
    infile = fopen(inputfile,"r");
    fscanf(infile,"%s %s %s %s %s %s %s %s %s %s %s\n",&stemp1, &stemp2,
           &stemp2, &stemp2, &stemp2, &stemp2, &stemp2, &stemp2, &stemp2, &stemp2,
           &stemp2);
    // open file for output
    std::string binary_output_filename = "final_state_hard_hadrons.bin";
    remove(binary_output_filename.c_str());
    FILE *outbin = NULL;
    outbin = fopen(binary_output_filename.c_str(), "wb");
    fwrite(&Ncoll, sizeof(int), 1, outbin);
    float weight_sum = 0.0;
    int even_loop_flag = 1;
    int count_event_number = 0;
    for (int iev = 0; iev < Nevent; iev ++) {
        if(feof(infile)) {
            even_loop_flag = 0;
            cout << " End the event loop ~~~ " << endl;
            break;
        }
        count_event_number++;
        fscanf(infile,"%s %s %d %s %lf %s %d %s %d %lf\n",&stemp1, &stemp2,
               &event_id, &stemp2, &weight, &stemp2, &int_temp, &stemp2, 
               &total_number_of_particles, &sigmaGen_ev);
        fwrite(&total_number_of_particles, sizeof(int), 1, outbin);
        float temp = weight;
        fwrite(&temp, sizeof(float), 1, outbin);
        float temp2 = sigmaGen_ev;
        fwrite(&temp2, sizeof(float), 1, outbin);
        //weight_array.push_back(weight);
        weight_sum = weight_sum + weight;
        for (auto i=0; i<total_number_of_particles; i++) {
            if(feof(infile)) {
                even_loop_flag = 0;
                cout << " End the event loop, and drop last event ~~~ " << endl;
                break;
            }
            fscanf(infile,"%d %d %d %lf %lf %lf %lf\n",&int_temp, &pid,
                   &status, &energy, &px, &py, &pz);
            fwrite(&pid, sizeof(int), 1, outbin);
            fwrite(&status, sizeof(int), 1, outbin);
            float array[] = {
                static_cast<float>(energy),
                static_cast<float>(px), static_cast<float>(py),
                static_cast<float>(pz),
            };
            fwrite(array, sizeof(float), 4, outbin);
            if (status == 11) continue; // don't count hydro hadrons.
            // calculate the pT-spectra
            double pT = sqrt(px*px + py*py);
            if (abs(rapidity_shift) > 0.0) {
                double pz_p = pz      * cosh(rapidity_shift) + energy * sinh(rapidity_shift);
                double e_p  = energy  * cosh(rapidity_shift) + pz * sinh(rapidity_shift);
                pz = pz_p;
                energy = e_p; 
            }
            double rapidity = 0.5 * log((energy+pz) / (energy-pz));
            double pmg = sqrt(pT*pT + pz*pz);
            double sudorapidity = 0.5 * log((pmg+pz) / (pmg-pz));
            double ET = sqrt(energy*energy - pz*pz);
            if (abs(rapidity) < rapidity_cut) {
                for (int kk = 0; kk < length; kk++) {
                    double midd1 = stepp*kk*1.0;
                    double midd2 = stepp*kk*1.0+stepp;
                    if ((pT > midd1) && (pT <= midd2)) {
                        if (abs(pid) == 111) dndptdypion0[kk] = dndptdypion0[kk] + add_piece * weight;
                        if (pid == 211) dndptdypionplus[kk] = dndptdypionplus[kk] + add_piece * weight;
                        if (pid == -211) dndptdypionmius[kk] = dndptdypionmius[kk] + add_piece * weight;
                        if (abs(pid) == 321) dndptdykaon[kk] = dndptdykaon[kk] + add_piece/2.0 * weight;
                        if (abs(pid) == 2212) dndptdyproton[kk] = dndptdyproton[kk] + add_piece/2.0 * weight;
                        if (pid == 310) dndptdyk0s[kk] = dndptdyk0s[kk] + add_piece * weight;
                        if (abs(pid) == 3122) dndptdyLambda[kk] = dndptdyLambda[kk] + add_piece/2.0 * weight;
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
                            dndptdetacharge[kk] = dndptdetacharge[kk] + add_piece_eta * weight;
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
                        dNch_deta[ieta] = dNch_deta[ieta] + 1.0/eta_step * weight;
                        dET_deta[ieta]  = dET_deta[ieta]  + 1.0/eta_step * weight * ET;
			dE_deta[ieta]  = dE_deta[ieta]  + 1.0/eta_step * weight * energy;
                        if (iev == 0) dNch_deta_fir_ev[ieta] = dNch_deta_fir_ev[ieta] + 1.0/eta_step * weight;
                    }
                }
            }
        }
    }
    fclose(outbin);
    if (even_loop_flag == 1) {
        fscanf(infile,"%s %s %lf %s %lf %s %lf %s %lf\n",&stemp1, &stemp2,
                       &sigmaGen, &stemp1, &sigmaErr, &stemp1, &weight,
                       &stemp1, &pThat);
    } else {
        sigmaGen = sigmaGen_ev;
    }
    fclose(infile);

    //output the results to file
    char output_filename[128];
    sprintf(output_filename,"Hard_hadron_yield");
    ofstream output(output_filename);

    if( ! output.is_open() ) {
        cout << "cannot open output file:"<< endl
             << output_filename << endl;
        return -1;
    }
    output << " # " << "pT,  dN/2pidpTdy of " << " pion0 " << " " << " pionplus " << " " << " pionminus " << " " << " kaon_charge " << " " 
           << " p " << " " << " K0s " << " " << " Lambda   "
           << " Lambda_cms1,  Lambda_cms2, " << " " << " Omega_cms1,  Omega_cms2,  " << " " << " Xsi_cms1,  Xsi_cms2, charged;   "
           << "eta,  dNch/deta " << " dNch/deta of first event " << " dET/deta " << " sigmaGen " << " Wight_sum " << " Nevent " << " Ncoll " 
           << endl; 
    for (auto i = 0; i < length; ++i) {
        output << stepp*i*1.0+ stepp/2.0 << "  " << dndptdypion0[i]/2./M_PI << "  " << dndptdypionplus[i]/2./M_PI << "  " 
               << dndptdypionmius[i]/2./M_PI << "  " << dndptdykaon[i]/2./M_PI << "  " 
               << dndptdyproton[i]/2./M_PI << "  " << dndptdyk0s[i]/2./M_PI << "  "
               << dndptdyLambda[i]/2./M_PI << "  " 
               << dndptdyLambdacms1[i]/Nevent/2./M_PI << "  " << dndptdyLambdacms2[i]/Nevent/2./M_PI << "  "
               << dndptdyOmegacms1[i]/Nevent/2./M_PI << "  " << dndptdyOmegacms2[i]/Nevent/2./M_PI << "  "
               << dndptdyXsicms1[i]/Nevent/2./M_PI << "  " << dndptdyXsicms2[i]/Nevent/2./M_PI << "  "
               << dndptdetacharge[i]/2./M_PI << "  "
               << eta_step*i*1.0 + eta_low + eta_step/2.0 << "  " << dNch_deta[i] << "  " << dNch_deta_fir_ev[i] << "  " 
               << dET_deta[i] << "  " << sigmaGen
               << "  " << weight_sum << "  " << count_event_number << "  " << Ncoll 
               << endl;
    }
    output.close();

}

