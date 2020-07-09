%module vdwo

%{
#define SWIG_FILE_WITH_INIT
#include <openbabel/mol.h>
#include "vdwo.hpp"
%}

double vdwo(OpenBabel::OBMol receptor, OpenBabel::OBMol ligand,
            double grid_size);

