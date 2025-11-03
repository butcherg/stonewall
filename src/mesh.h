#ifndef _MESH_
#define _MESH_

#include <string>
#include <vector>

struct vec3f {
	union {
		struct {
			float x, y, z;
		};
		struct {
			float d[3];
		};
	};
};

struct vec3d {
	double x, y, z;
};

struct vec3i {
	union {
		struct {
			unsigned x, y, z;
		};
		struct {
			unsigned d[3];
		};
	};
};

class Mesh
{
public:
	Mesh();
	
	unsigned setPt(vec3f p);
	unsigned setTri(vec3i t);
	
	bool SaveMesh(const std::string &filename);
	bool SaveBinarySTL(const std::string &filename);
	bool Save3MF(const std::string &filename);

	std::vector<vec3f> points;
	std::vector<vec3i> triangles;
	
private:
	vec3f normalize(vec3f v);
	vec3f triangleNormal(vec3f p1, vec3f p2, vec3f p3);

};

#endif
