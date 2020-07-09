#pragma once

template<typename T_out, typename T_inp>
T_out vector3_cast(T_inp &vector_inp);

double vdwo(OpenBabel::OBMol receptor, OpenBabel::OBMol ligand,
            double grid_size);

