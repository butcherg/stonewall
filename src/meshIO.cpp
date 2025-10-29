
#include "manifold/meshIO.h"
#include "mesh.h"

#include <iostream>

namespace manifold {

MeshGL ImportMesh(const std::string& filename, bool forceCleanup) {
	MeshGL meshout;
	
	//To-Do: implement import.
	
	return meshout;
}


void ExportMesh(const std::string& filename, const MeshGL& mesh,
                const ExportOptions& options) {
	Mesh mmesh;
	
	for (size_t i = 0; i < mesh.NumVert(); ++i) {
		vec3 v;
		for (int j : {0, 1, 2}) v[j] = mesh.vertProperties[i * mesh.numProp + j];
		mmesh.setPt({(float) v.x, (float) v.y, (float) v.z});
	}

	for (size_t i = 0; i < mesh.NumTri(); ++i) {
		linalg::vec<unsigned, 3> t;
		for (int j : {0, 1, 2}) t[j] = mesh.triVerts[3 * i + j];
		mmesh.setTri({t[0], t[1], t[2]});
	 }
	 
	 mmesh.SaveMesh(filename);
}

}  // namespace manifold
