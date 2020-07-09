%module vdwo

%{
#define SWIG_FILE_WITH_INIT
#include <openbabel/mol.h>
#include "vdwo.hpp"
%}

%feature("docstring") vdwo %{
    vdwo(openbabel.OBMol receptor, openbabel.OBMol ligand, float grid_size) -> float

    Calculates the Van-der-Waals overlap volume between two structures via numeric integration.
    :param receptor: first structure
    :param ligand: second structure
    :param grid_size: grid size for the numeric integration in \u212b
    :return: returns volume in \u212b\u00b3
%}
double vdwo(OpenBabel::OBMol receptor, OpenBabel::OBMol ligand,
            double grid_size);

