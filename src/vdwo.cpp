#include <iostream>
#include <cmath>
#include <numeric>
#include <limits>
#include <omp.h>
#include <openbabel/mol.h>
#include <openbabel/data.h>
#include <openbabel/math/vector3.h>
#include "zip.hpp"
#include "vdwo.hpp"

static OpenBabel::OBElementTable obtable;
static const std::numeric_limits<double> double_limits;

OpenBabel::vector3 min(OpenBabel::vector3 a, OpenBabel::vector3 b) {
    auto output = OpenBabel::vector3();
    output.SetX((a.GetX() < b.GetX()) ? a.GetX() : b.GetX());
    output.SetY((a.GetY() < b.GetY()) ? a.GetY() : b.GetY());
    output.SetZ((a.GetZ() < b.GetZ()) ? a.GetZ() : b.GetZ());
    return output;
};

OpenBabel::vector3 max(OpenBabel::vector3 a, OpenBabel::vector3 b) {
    auto output = OpenBabel::vector3();
    output.SetX((a.GetX() > b.GetX()) ? a.GetX() : b.GetX());
    output.SetY((a.GetY() > b.GetY()) ? a.GetY() : b.GetY());
    output.SetZ((a.GetZ() > b.GetZ()) ? a.GetZ() : b.GetZ());
    return output;
};

void print_progress_bar(double progress, std::ostream& stream = std::cout) {
    unsigned percentage = 100 * progress;
    stream << "[" << std::string(percentage/2, '=') 
                  << std::string(50 - percentage / 2, ' ') << "] "
                  << percentage << "%\r";
    stream.flush();
};

class Sphere {
    public:
        Sphere(OpenBabel::vector3 position, double radius) :
               _position(position), _radius(radius) {
        };
        void SetPosition(OpenBabel::vector3& position) {
            _position = position;
        };
        OpenBabel::vector3& GetPosition() {
            return _position;
        };
        void SetRadius(double radius) {
            _radius = radius;
        };
        double GetRadius() {
            return _radius;
        };
    
    protected:
        OpenBabel::vector3 _position;
        double _radius;
};

class Cuboid {
    public:
        Cuboid(OpenBabel::vector3 position, OpenBabel::vector3 X,
               OpenBabel::vector3 Y, OpenBabel::vector3 Z) :
               _position(position), _vectors(OpenBabel::matrix3x3(X,Y,Z)) {
        };
        
        void SetPosition(OpenBabel::vector3 position) {
            _position = position;
        };
        OpenBabel::vector3& GetPosition() {
            return _position;
        };
        void SetVector(unsigned idx, OpenBabel::vector3 vector) {
            _vectors.SetRow(idx, vector);
        };
        OpenBabel::vector3 GetVector(unsigned idx) {
            return _vectors.GetRow(idx);
        };

    protected:
        OpenBabel::vector3 _position;
        OpenBabel::matrix3x3 _vectors;
};

class OverlapIntegrationBox {
    public:
        OverlapIntegrationBox(double grid_size) : _grid_size(grid_size) {
            
        };
        void add_sphere(unsigned group_id, Sphere& sphere) {
            _spheres[group_id].push_back(sphere);
        };
        void add_sphere(unsigned group_id, OpenBabel::vector3& position,
                        double radius) {
            Sphere sphere(position, radius);
            add_sphere(group_id, sphere);
        };
        void set_grid_size(double grid_size) {
            _grid_size = grid_size;
        };
        double& get_grid_size() {
            return _grid_size;
        };
        double compute() {
            _compute_boundary();

            unsigned total_overlap_counter = 0;
            unsigned n_x = std::ceil((_boundary.max.x() - _boundary.min.x()) / _grid_size);
            unsigned n_y = std::ceil((_boundary.max.y() - _boundary.min.y()) / _grid_size);
            unsigned n_z = std::ceil((_boundary.max.z() - _boundary.min.z()) / _grid_size);
            #pragma omp parallel for default(none) shared(total_overlap_counter, n_x, n_y, n_z)
            for (unsigned i_x = 0; i_x <= n_x; ++i_x) {
                int thread_overlap_counter = 0;
                double x = i_x * _grid_size + _boundary.min.x();
                for (unsigned i_y = 0; i_y <= n_y; ++i_y) {
                    double y = i_y * _grid_size + _boundary.min.y();
                    for (unsigned i_z = 0; i_z <= n_z; ++i_z) {
                        double z = i_z * _grid_size + _boundary.min.z();
                        OpenBabel::vector3 grid_pos(x, y, z);
                        std::array<bool,2> is_in_sphere_group{false, false};
                        for (auto&& [sphere_group, is_in_sphere] : Zip(_spheres, is_in_sphere_group)) {
                            for (auto& sphere : sphere_group) {
                                is_in_sphere |= (sphere.GetPosition().distSq(grid_pos) <=  std::pow(sphere.GetRadius(), 2));
                            };
                        };
                        // If cell belongs to both sphere groups
                        if (std::find_if_not(is_in_sphere_group.begin(), is_in_sphere_group.end(), [](bool i){return i;}) == is_in_sphere_group.end()) {
                            ++thread_overlap_counter;
                        };
                    };
                };
                #pragma omp atomic
                total_overlap_counter += thread_overlap_counter;
            };
            return total_overlap_counter * std::pow(_grid_size, 3);
        };

    protected:
        struct Boundary {
            OpenBabel::vector3 min;
            OpenBabel::vector3 max;
        };
        
        double _grid_size;
        std::array<std::vector<Sphere>, 2> _spheres;
        Boundary _boundary;
        
        inline void _compute_boundary() {
            std::array<Boundary, 2> tmp_boundaries;
            for (auto&& [sphere_group, tmp_boundary] : Zip(_spheres,
                 tmp_boundaries)) {
                tmp_boundary.min = double_limits.max();
                tmp_boundary.max = double_limits.min();
                for (auto& sphere : sphere_group) {
                    OpenBabel::vector3& position = sphere.GetPosition();
                    double radius = sphere.GetRadius();
                    OpenBabel::vector3 offset(radius, radius, radius);
                    tmp_boundary.min = min(tmp_boundary.min, position
                                           - offset);
                    tmp_boundary.max = max(tmp_boundary.max, position
                                           + offset);
                };
            };
            _boundary = {max(tmp_boundaries[0].min, tmp_boundaries[1].min),
                        min(tmp_boundaries[0].max, tmp_boundaries[1].max)};
        };
};

template<typename T_out, typename T_inp>
T_out vector3_cast(T_inp &vector_inp) {
    T_out vector_out;
    for (unsigned i = 0; i < 3; i++) {
        vector_out[i] = vector_inp[i];
    };
    return vector_out;
};

double vdwo(OpenBabel::OBMol receptor, OpenBabel::OBMol ligand,
                    double grid_size) {
    double overlap = 0;
    if (ligand.NumAtoms() <= 0 || receptor.NumAtoms() <= 0) {
        return overlap;
    };
    OverlapIntegrationBox overlap_integrator(grid_size);
    for (unsigned group_id = 0; group_id < 2; group_id++) {
        OpenBabel::OBMol molecule = (group_id == 0) ? receptor : ligand;
        for (unsigned i = 1; i <= molecule.NumAtoms(); i++) {
            OpenBabel::OBAtom *atom = molecule.GetAtom(i);
            unsigned element = atom->GetAtomicNum();
            OpenBabel::vector3 test = atom->GetVector();
            auto position = atom->GetVector();
            double radius = obtable.GetVdwRad(element);
            overlap_integrator.add_sphere(group_id, position, radius);
        };
    };
    return overlap_integrator.compute();
};
