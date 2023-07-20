# preperations
import numpy as np
import matplotlib.pyplot as plt

############################################################################
# Set File Paths
# Please set the path for pp jet data file
file_pp = '../../data/jet_pp.dat'

# Please set the path for pbpb jet data file
file_pbpb = '../../data/jet_pbpb.dat'

# Please set the value of jet cone size you used in the jet reconstruction
jetR = 0.4

# Number of hard scattering events generated by JETSCAPE
n_ev_pp = 250
n_ev_pbpb = 250

############################################################################
# preperations
import numpy as np
import matplotlib.pyplot as plt

# pi and 2pi coppied from FastJet
twopi = 6.283185307179586476925286766559005768394
pi = 0.5*twopi

# define plot style
width = 0.05
plotMarkerSize = 8
labelfontsize = 15
import matplotlib as mpl
mpl.rcParams['figure.figsize'] = [6., 4.5]
mpl.rcParams['lines.linewidth'] = 2
mpl.rcParams['xtick.top'] = True
mpl.rcParams['xtick.labelsize'] = 15
mpl.rcParams['xtick.major.width'] = 1.0
mpl.rcParams['xtick.minor.width'] = 0.8
mpl.rcParams['xtick.minor.visible'] = True
mpl.rcParams['xtick.direction'] = "in"
mpl.rcParams['ytick.right'] = True
mpl.rcParams['ytick.labelsize'] = 15
mpl.rcParams['ytick.major.width'] = 1.0
mpl.rcParams['ytick.minor.width'] = 0.8
mpl.rcParams['ytick.minor.visible'] = True
mpl.rcParams['ytick.direction'] = "in"
mpl.rcParams['legend.fontsize'] = 15
mpl.rcParams['legend.numpoints'] = 1
mpl.rcParams['font.size'] = 15
mpl.rcParams['savefig.format'] = "pdf"
############################################################################
def ratio_error(v1,e1,v2,e2):
  #v1, e1: numerator value and error
  #v2, e2: denominator value and error  
  error1 = e1/v2
  error2 = (e2/v2)*(v1/v2)
  error = np.sqrt(error1*error1+error2*error2)
  return error
############################################################################
# Load files
data_pp = np.loadtxt(file_pp, delimiter=',')
data_pbpb = np.loadtxt(file_pbpb, delimiter=',')

# Indices of the data array
i_pp = data_pp[:,0] 
i_pbpb = data_pbpb[:,0] 

# Get Indices of jets in the data array
jet_id_pp = np.where(i_pp < 0.1)
jet_id_pbpb = np.where(i_pbpb < 0.1)

# Extract jets
jets_pp = data_pp[jet_id_pp]
jets_pbpb = data_pbpb[jet_id_pbpb]

# Extract associated charged particles for pp
assoc_pp= []
for i in range(len(jet_id_pp[0])-1):
  chunck = data_pp[jet_id_pp[0][i]+1:jet_id_pp[0][i+1]]
  assoc_pp.append(chunck)
chunck = data_pp[jet_id_pp[0][-1]+1:]
assoc_pp.append(chunck)

# Extract associated charged particles for pbpb
assoc_pbpb= []
for i in range(len(jet_id_pbpb[0])-1):
  chunck = data_pbpb[jet_id_pbpb[0][i]+1:jet_id_pbpb[0][i+1]]
  assoc_pbpb.append(chunck)
chunck = data_pbpb[jet_id_pbpb[0][-1]+1:]
assoc_pbpb.append(chunck)
############################################################################
## Jet Fragmentation Function
############################################################################
# Number of jets
n_jet_pp = len(jets_pp)
n_jet_pbpb = len(jets_pbpb)

# prepare arrays for z = pT_assoc/pTjet
z_pp_in = []
z_pbpb_in = []

# prepare arrays for associated's status
status_pp = []
status_pbpb = []

# z for pp
for i in range(n_jet_pp):
  pt_jet = jets_pp[i,1] #jet-pt in pp
  for assoc in assoc_pp[i]:
    pt_assoc = assoc[1]
    z = pt_assoc/pt_jet
    z_pp_in.append(z)
    status_pp.append(1.0)

# z for pbpb
for i in range(n_jet_pbpb):
  pt_jet = jets_pbpb[i,1] #jet-pt in PbPb
  for assoc in assoc_pbpb[i]:
    pt_assoc = assoc[1]
    z = pt_assoc/pt_jet
    z_pbpb_in.append(z)

    # For hole subtraction
    status = assoc[4]
    if not status < 0:
      status = 1
    status_pbpb.append(status)


# Set Histogram bins
ex = np.linspace(-2, 0, 8)
z_bins = np.power(10,ex)

# Fill Histograms
n_pp, z = np.histogram(z_pp_in, bins=z_bins, weights = status_pp )
n_pbpb, z = np.histogram(z_pbpb_in, bins=z_bins, weights = status_pbpb )


# Statistical Errors
err_n_pp = np.sqrt(n_pp)
err_n_pbpb = np.sqrt(n_pbpb)

# bin width
dz = (z[1:]-z[:-1])
# bin center
z = z[0:-1] + 0.5*dz

# z-distribution
dn_dz_pp = n_pp/dz/n_jet_pp
dn_dz_pbpb = n_pbpb/dz/n_jet_pbpb

# Errors 
err_dn_dz_pp = err_n_pp/dz/n_jet_pp
err_dn_dz_pbpb = err_n_pbpb/dz/n_jet_pbpb

# Generate Plots
fig = plt.figure()

plt.errorbar(z, dn_dz_pp, fmt='s', label="pp",
             xerr=0.5*dz, yerr=err_dn_dz_pp, color='black')
plt.errorbar(z, dn_dz_pbpb, fmt='o', label="PbPb(30-40%)",
             xerr=0.5*dz, yerr=err_dn_dz_pbpb, color='red')


#axes setting
plt.xscale('log')
plt.yscale('log')
plt.legend(loc=0)
plt.xlabel(r"$z$")
plt.ylabel(r"$D(z)=(1/N_{\mathrm{jet}})dN_{\mathrm{ch}}/dz}_{\mathrm{T}}$")
plt.xlim(z_bins[0],z_bins[-1])
y_min = 0.01
y_max = 1000
plt.ylim(y_min,y_max)
plt.text(0.012,y_min*1.2, '5.02 TeV')

# save plot to the Desktop
#plt.tight_layout()
print('Find jet_ff.pdf')
plt.savefig('jet_ff')
############################################################################
## Ratio
############################################################################
# Generate Plots
fig = plt.figure()

# Calculate RAA and error
raa =  dn_dz_pbpb/dn_dz_pp
error_raa = ratio_error(dn_dz_pbpb,err_dn_dz_pbpb,dn_dz_pp,err_dn_dz_pp)


plt.errorbar(z, raa, fmt='o', label="PbPb(30-40%)/pp",
             xerr=0.5*dz, yerr=error_raa, color='red')


#axes setting
plt.legend(loc=0)
plt.xlabel(r"$z$")
plt.ylabel(r"$D(z)_{\mathrm{PbPb}}/D(z)_{pp}}$")
plt.xlim(z_bins[0],z_bins[-1])
plt.ylim(0.0,2.0)
plt.legend(loc=0)
plt.text(0.012,0.1, '5.02 TeV')
plt.xscale('log')

plt.axhline(1, color = "black", linestyle="dashed", linewidth=0.8) 

# save plot to the Desktop
plt.tight_layout()
print('Find ratio_jet_ff.pdf')
plt.savefig('ratio_jet_ff')
############################################################################