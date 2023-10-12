#include <iostream>
#include <fstream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;


int main()
{
    MyMesh mesh;
    OpenMesh::FPropHandleT<double> min_dihedral_angle;
    OpenMesh::FPropHandleT<double> max_dihedral_angle;
    mesh.add_property(min_dihedral_angle);
    mesh.add_property(max_dihedral_angle);
    if (!OpenMesh::IO::read_mesh(mesh, "./meshes/spot.obj")) {
        std::cerr << "Error: Cannot read mesh file." << std::endl;
        return 1;
    }
    std::ofstream out_min_file("./meshes/per_face_min_angle.txt");
    std::ofstream out_max_file("./meshes/per_face_max_angle.txt");

    // Iterate through all faces
    for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
        MyMesh::FaceHandle face = *f_it;
        double max_angle = std::numeric_limits<double>::min();
        double min_angle = std::numeric_limits<double>::max();
        // Calculate the dihedral angle for the face
        for (MyMesh::FaceHalfedgeIter fh_it = mesh.fh_iter(face); fh_it.is_valid(); ++fh_it) {
            MyMesh::HalfedgeHandle halfEdge = *fh_it;
            double angle = mesh.calc_dihedral_angle(halfEdge);
            angle = angle > 0 ? angle : -angle;
            if (angle > max_angle)
                max_angle = angle;
            if (angle < min_angle)
                min_angle = angle;
        }
        mesh.property(min_dihedral_angle, face) = min_angle;
        mesh.property(max_dihedral_angle, face) = max_angle;
        out_min_file << min_angle << std::endl;
        out_max_file << max_angle << std::endl;
    }
    out_min_file.close();
    out_max_file.close();
    return 0;
}

