#!/usr/bin/env bash

echo "replacing HybridHadronization.h"
cp -r HybridHadronization.h ../../../X-SCAPE/src/hadronization/
echo "replacing HybridHadronization.cc"
cp -r HybridHadronization.cc ../../../X-SCAPE/src/hadronization/

echo "replacing ColorlessHadronization.cc"
cp -r ColorlessHadronization.cc ../../../X-SCAPE/src/hadronization/

echo "replacing ColoredHadronization.cc"
cp -r ColoredHadronization.cc ../../../X-SCAPE/src/hadronization/

echo "replacing src/CMakeLists.txt"
cp -r CMakeLists.txt ../../../X-SCAPE/src/

echo "adding ThermPtnSampler.h"
cp -r ThermPtnSampler.h ../../../X-SCAPE/src/hadronization/
echo "adding ThermPtnSampler.cc"
cp -r ThermPtnSampler.cc ../../../X-SCAPE/src/hadronization/

echo "adding epemGun.h"
cp -r epemGun.h ../../../X-SCAPE/src/initialstate/
echo "adding epemGun.cc"
cp -r epemGun.cc ../../../X-SCAPE/src/initialstate/
echo "replacing JetScape.cc"
cp -r JetScape.cc ../../../X-SCAPE/src/framework/

echo "replacing Matter.h"
cp -r Matter.h ../../../X-SCAPE/src/jet/
echo "replacing Matter.cc"
cp -r Matter.cc ../../../X-SCAPE/src/jet/

echo "replacing Brick.h"
cp -r Brick.h ../../../X-SCAPE/src/hydro/
echo "replacing Brick.cc"
cp -r Brick.cc ../../../X-SCAPE/src/hydro/

echo "replacing PGun.h"
cp -r PGun.h ../../../X-SCAPE/src/initialstate/
echo "replacing PGun.cc"
cp -r PGun.cc ../../../X-SCAPE/src/initialstate/

echo "replacing PythiaGun.h"
cp -r PythiaGun.h ../../../X-SCAPE/src/initialstate/
echo "replacing PythiaGun.cc"
cp -r PythiaGun.cc ../../../X-SCAPE/src/initialstate/

echo "replacing jetscape_main.xml"
cp -r jetscape_main.xml ../../../X-SCAPE/config/
