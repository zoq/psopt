
//////////////////////////////////////////////////////////////////////////
//////////////////         chain.cxx               ///////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////           PSOPT  Example             ////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////// Title:           Hanging chain problem           ////////////////
//////// Last modified:   29 January 2009                 ////////////////
//////// Reference:                                       ////////////////
////////                                                  ////////////////
//////////////////////////////////////////////////////////////////////////
////////     Copyright (c) Victor M. Becerra, 2009        ////////////////
//////////////////////////////////////////////////////////////////////////
//////// This is part of the PSOPT software library, which ///////////////
//////// is distributed under the terms of the GNU Lesser ////////////////
//////// General Public License (LGPL)                    ////////////////
//////////////////////////////////////////////////////////////////////////

#include "psopt.h"

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the end point (Mayer) cost function //////////
//////////////////////////////////////////////////////////////////////////

adouble endpoint_cost(adouble* initial_states, adouble* final_states,
                      adouble* parameters,adouble& t0, adouble& tf,
                      adouble* xad, int iphase, Workspace* workspace)
{
    return 0.0;
}

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the integrand (Lagrange) cost function  //////
//////////////////////////////////////////////////////////////////////////

adouble integrand_cost(adouble* states, adouble* controls,
                       adouble* parameters, adouble& time, adouble* xad,
                       int iphase, Workspace* workspace)
{
    adouble x 		= states[   0 ];
    adouble dxdt 	= controls[ 0 ];

    adouble L = x*sqrt(1.0+ pow(dxdt,2.0));

    return  L;
}

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the DAE's ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void dae(adouble* derivatives, adouble* path, adouble* states,
         adouble* controls, adouble* parameters, adouble& time,
         adouble* xad, int iphase, Workspace* workspace)
{
   adouble xdot, ydot, vdot;

   adouble x = states[ 0 ];

   adouble dxdt = controls[ 0 ];

   derivatives[ 0 ] = dxdt;

}


////////////////////////////////////////////////////////////////////////////
///////////////////  Define the integrand of the integral constraint ///////
////////////////////////////////////////////////////////////////////////////

adouble integrand( adouble* states, adouble* controls, adouble* parameters,
                     adouble& time, adouble* xad, int iphase, Workspace* workspace)
{
   adouble G;
   adouble dxdt = controls[ 0 ];

   G =   sqrt( 1.0 + pow(dxdt,2.0));

   return G;

}


////////////////////////////////////////////////////////////////////////////
///////////////////  Define the events function ////////////////////////////
////////////////////////////////////////////////////////////////////////////

void events(adouble* e, adouble* initial_states, adouble* final_states,
            adouble* parameters,adouble& t0, adouble& tf, adouble* xad,
            int iphase, Workspace* workspace)
{
   adouble x0 = initial_states[ 0 ];
   adouble xf = final_states[   0 ];
   adouble Q;

   // Compute the integral to be constrained
   Q = integrate( integrand, xad, iphase, workspace );

   e[ 0 ] = x0;
   e[ 1 ] = xf;
   e[ 2 ] = Q;

}



///////////////////////////////////////////////////////////////////////////
///////////////////  Define the phase linkages function ///////////////////
///////////////////////////////////////////////////////////////////////////

void linkages( adouble* linkages, adouble* xad, Workspace* workspace)
{
  // No linkages as this is a single phase problem
}


////////////////////////////////////////////////////////////////////////////
///////////////////  Define the main routine ///////////////////////////////
////////////////////////////////////////////////////////////////////////////


int main(void)
{

////////////////////////////////////////////////////////////////////////////
///////////////////  Declare key structures ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    Alg  algorithm;
    Sol  solution;
    Prob problem;

////////////////////////////////////////////////////////////////////////////
///////////////////  Register problem name  ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    problem.name        		= "Hanging chain problem";

    problem.outfilename                 = "chain.txt";

////////////////////////////////////////////////////////////////////////////
////////////  Define problem level constants & do level 1 setup ////////////
////////////////////////////////////////////////////////////////////////////

    problem.nphases   			= 1;
    problem.nlinkages                   = 0;

    psopt_level1_setup(problem);


/////////////////////////////////////////////////////////////////////////////
/////////   Define phase related information & do level 2 setup /////////////
/////////////////////////////////////////////////////////////////////////////



    problem.phases(1).nstates   		= 1;
    problem.phases(1).ncontrols 		= 1;
    problem.phases(1).nevents   		= 3;
    problem.phases(1).npath     		= 0;
    problem.phases(1).nodes         =  (RowVectorXi(2) << 20, 50).finished();

    psopt_level2_setup(problem, algorithm);



////////////////////////////////////////////////////////////////////////////
///////////////////  Enter problem bounds information //////////////////////
////////////////////////////////////////////////////////////////////////////

    problem.phases(1).bounds.lower.states(0) =  -10.0;
    problem.phases(1).bounds.upper.states(0) =   10.0;


    problem.phases(1).bounds.lower.controls(0) = -20.0;
    problem.phases(1).bounds.upper.controls(0) =  20.0;

    problem.phases(1).bounds.lower.events(0) =  1.0;
    problem.phases(1).bounds.lower.events(1) =  3.0;
    problem.phases(1).bounds.lower.events(2) =  4.0;


    problem.phases(1).bounds.upper.events(0) = 1.0;
    problem.phases(1).bounds.upper.events(1) = 3.0;
    problem.phases(1).bounds.upper.events(2) = 4.0;



    problem.phases(1).bounds.lower.StartTime    = 0.0;
    problem.phases(1).bounds.upper.StartTime    = 0.0;

    problem.phases(1).bounds.lower.EndTime      = 1.0;
    problem.phases(1).bounds.upper.EndTime      = 1.0;


////////////////////////////////////////////////////////////////////////////
///////////////////  Register problem functions  ///////////////////////////
////////////////////////////////////////////////////////////////////////////


    problem.integrand_cost 	= &integrand_cost;
    problem.endpoint_cost 	= &endpoint_cost;
    problem.dae 		= &dae;
    problem.events 		= &events;
    problem.linkages		= &linkages;




////////////////////////////////////////////////////////////////////////////
///////////////////  Define & register initial guess ///////////////////////
////////////////////////////////////////////////////////////////////////////

    problem.phases(1).guess.controls       = 2.0*ones(1,30);
    problem.phases(1).guess.states         = linspace(1.0,3.0, 30);
    problem.phases(1).guess.time           = linspace(0.0,1.0, 30);

////////////////////////////////////////////////////////////////////////////
///////////////////  Enter algorithm options  //////////////////////////////
////////////////////////////////////////////////////////////////////////////


    algorithm.nlp_method                  = "IPOPT";
    algorithm.scaling                     = "automatic";
    algorithm.derivatives                 = "automatic";
    algorithm.nlp_iter_max                = 1000;
    algorithm.nlp_tolerance               = 1.e-6;



////////////////////////////////////////////////////////////////////////////
///////////////////  Now call PSOPT to solve the problem   /////////////////
////////////////////////////////////////////////////////////////////////////

    psopt(solution, problem, algorithm);

    if (solution.error_flag) exit(0);

////////////////////////////////////////////////////////////////////////////
///////////  Extract relevant variables from solution structure   //////////
////////////////////////////////////////////////////////////////////////////

    MatrixXd x, u, t;
    x 		= solution.get_states_in_phase(1);
    u 		= solution.get_controls_in_phase(1);
    t           = solution.get_time_in_phase(1);

////////////////////////////////////////////////////////////////////////////
///////////  Save solution data to files if desired ////////////////////////
////////////////////////////////////////////////////////////////////////////

    Save(x,"x.dat");
    Save(u,"u.dat");
    Save(t,"t.dat");


////////////////////////////////////////////////////////////////////////////
///////////  Plot some results if desired (requires gnuplot) ///////////////
////////////////////////////////////////////////////////////////////////////

    plot(t,x,problem.name + ": state", "time (s)", "x", "x");

    plot(t,u,problem.name + ": control", "time (s)", "u", "u");

    plot(t,x,problem.name + ": state", "time (s)", "x", "x",
                            "pdf", "chain_state.pdf");

    plot(t,u,problem.name + ": control", "time (s)", "u", "u",
                            "pdf", "chain_control.pdf");



}

////////////////////////////////////////////////////////////////////////////
///////////////////////      END OF FILE     ///////////////////////////////
////////////////////////////////////////////////////////////////////////////


