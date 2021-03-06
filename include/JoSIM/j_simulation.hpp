// Copyright (c) 2018 Johannes Delport
// This code is licensed under MIT license (see LICENSE for details)
#ifndef J_SIMULATION_H
#define J_SIMULATION_H
#include "j_std_include.hpp"

#define TRANSIENT 0
#define DC 1
#define AC 2
#define PHASE 3
#define NONE_SPECIFIED 4

/* Transient analysis simulation object*/
class trans_sim
{
public:
  double prstep;
  double tstop;
  double tstart;
  double maxtstep;
  trans_sim()
  {
    tstart = 0.0;
    maxtstep = 1E-12;
  }
  double simsize() { return (tstop - tstart) / maxtstep; }
};
extern trans_sim tsim;

/* RCSJ simulation object*/
class rcsj_sim_object
{
public:
  int vPositive, vNegative, bPhase;
  std::string label, positiveNodeRow, negativeNodeRow;
  int jjRtype;
  bool superconducting;
  double jjIcrit, jjCap, jjVg, VB, VB_Prev, VB_dt, VB_dt_Prev, VB_Guess, Phase,
    Phase_Guess, Phase_Prev, Is;
  rcsj_sim_object()
  {
    superconducting = true;
    vPositive = -1;
    vNegative = -1;
    bPhase = -1;
    VB_Prev = 0.0;
    VB_dt = 0.0;
    VB_dt_Prev = 0.0;
    VB_Guess = 0.0;
    Phase = 0.0;
    Phase_Guess = 0.0;
    Phase_Prev = 0.0;
    Is = 0.0;
  }
};

extern std::vector<std::vector<double>> xVect;
extern std::vector<double> timeAxis;
extern std::unordered_map<std::string, std::vector<double>> junctionCurrents;
/*
  Identify simulation type. Be it transient, ac, dc or phase
*/
void
identify_simulation(InputFile& iFile);
/*
        Perform transient simulation
*/
void
transient_simulation(InputFile& iFile);
#endif