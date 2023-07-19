# Hands-on session on hadronization

:exclamation: Please read and complete **all** of the instructions in **Part I-III** carefully **before** the hands-on session on Jul 27.

> Note: It might be a good idea to follow **Part I-III** after the RIVET sessions on Jul 26, as for the hadronization a large part of the framework will be modified (see :warning: below).

If you encounter any problems feel free to contact us on slack in the [# july27-hadronization](https://jetscape2023o-d166455.slack.com/archives/C05GBKS0CQ6) channel.

# Table of contents
- **Part I: Introduction**
    - i. Background
    - i. Goals
- **Part II: Get ready**
- **Part III: Run Simulations**
    - i. $e^+e^-$
    - ii. Brick
- **Part IV: Hands-on Session**


# Part I: Introduction

## i. Background

The hadronization of hard partons from jets in JETSCAPE / X-SCAPE can be performed with three different modules:

- Colorless hadronization (no color information used in string fragmentation)
- Colored hadronization (information on color is used in string fragmentation)
- Hybrid hadronization (quark recombination for the dense parts and string fragmentation for dilute parts)

In this hands-on session we will mainly use the hybrid hadronization module.

> :warning: **The hybrid hadronization version of the module we are going to use in the session (and also the $e^+e^-$ gun module) is currently under development and not released in any public version yet. We are preparing a public release of this module in the near future for JETSCAPE and X-SCAPE. Until then there might be changes in the implementation. Do not use this version for scientific work.**

## ii. Goals

- In this session we will understand how to hadronize partons from hard processes.
- We will simulate $e^+e^-$ collisions and we will investigate different observables in systems without a medium.
- Simulations with a jet in a brick hydro medium will be used to compare the different hydronization modules.

# Part II: Get Ready

As mentioned above, the hybrid hadronization module is currently under development, so it is not part of the X-SCAPE 1.0 release used for the summer school.
Please make sure to follow the next steps carefully, which will implement the current version of the hybrid hadronization module in the X-SCAPE 1.0 code.
For simplicity we will perform all of the following steps from inside the docker container.

Go to `~/X-SCAPE/external_packages/` and type `ls` to check if you have downloaded the LBT tables already (maybe for another hands-on session). There should be a corresponding directory called `LBT-tables`. If this does not exist, please download the tables using:

```
./get_lbtTab.sh
```

Go to `~/SummerSchool2023/Jul27_Hadronization/replace_files/` and execute the bash script:
```
./replace_files.sh
```
Then go back to `~/X-SCAPE/build/` and run `make -jN` with `N` the number of cpu cores you want to use for the process.

> Note: In case you want to go back to a *clean* version of the X-SCAPE code after the session, then you can run `./restore_code.sh` in the `~/SummerSchool2023/Jul27_Hadronization/replace_files/` directory and run `make` in the build directory.

For the hands-on session we will need an additional python package for the analysis of the data, which is called *sparkx*. You can install it in the docker container using:
```
pip3 install sparkx
```
> Note: The documentation (including many examples) of the sparkx package is available under this [link](https://smash-transport.github.io/sparkx/).

# Part III: Run Simulations

To prepare for the hands on session it would be great, if you can run some simulations before the session starts to generate datasets we can analyze.

Go to the `~/X-SCAPE/build/` directory and create a directory called `hadronization_results`.

## i. $e^+e^-$
Inside the `~/X-SCAPE/build/` directory run:
```
./runJetscape ../../SummerSchool2023/Jul27_Hadronization/xml_files/jetscape_user_epem.xml
```
The simulation of 10000 $e^+e^-$ events will run for up to $\approx 4$ minutes, depending on your system.

## ii. Brick
```
./runJetscape ../../SummerSchool2023/Jul27_Hadronization/xml_files/jetscape_user_brick_hybrid_2fm.xml
./runJetscape ../../SummerSchool2023/Jul27_Hadronization/xml_files/jetscape_user_brick_hybrid_4fm.xml
```
The simulation of 1000 jets in a 2 / 4 fm brick will run for up to $\approx 5 / 9$ minutes, depending on your system.

```
./runJetscape ../../SummerSchool2023/Jul27_Hadronization/xml_files/jetscape_user_brick_colorless_2fm.xml
```
The simulation of 1000 jets in colorless hadronization will run for up to $\approx 7$ minutes, depending on your system.

# Part IV: Hands-on Session

## Jupyter Notebooks

Information on the jupyter notebooks...