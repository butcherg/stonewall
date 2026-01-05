

#include "mesh.h"
#include "miniz.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <ios>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <cctype>

Mesh::Mesh() {
	points.clear();
	triangles.clear();
}

unsigned Mesh::setPt(vec3f p)
{
	points.push_back(p);
	return points.size()-1;
}
	
unsigned Mesh::setTri(vec3i t)
{
	triangles.push_back(t);
	return triangles.size()-1;
}

vec3f Mesh::normalize(vec3f v)
{
	vec3f n;
	double l = sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
	n.x = v.x/l;
	n.y = v.y/l;
	n.z = v.z/l;
	return n;
}

vec3f  Mesh::triangleNormal(vec3f p1, vec3f p2, vec3f p3)
{
	vec3f n, u, v;
	
	u.x = p2.x - p1.x;
	u.y = p2.y - p1.y;
	u.z = p2.z - p1.z;
	
	v.x = p3.x - p1.x;
	v.y = p3.y - p1.y;
	v.z = p3.z - p1.z;
	
	n.x = u.y*v.z - u.z*v.y;
	n.y = u.z*v.x - u.x*v.z;
	n.z = u.x*v.y - u.y*v.x;
	
	n = normalize(n);
	return n;
}

bool Mesh::SaveMesh(const std::string &filename)
{
	std::filesystem::path filePath = filename;
	std::string ext = filePath.extension().u8string();
	std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
    if (ext == ".stl") 
		return SaveBinarySTL(filename);
    else if (ext == ".3mf") 
		return Save3MF(filename);
	else return false;
}

//bool Mesh::SaveBinarySTL(const std::string &path, const std::vector<vec3f> &points, const std::vector<vec3i> &triangles)
bool Mesh::SaveBinarySTL(const std::string &filename)
{
    const uint64_t numBytes = uint64_t(triangles.size()) * 50 + 84;
    char *dst = (char *)calloc(numBytes, 1);

    const uint32_t count = triangles.size();
    memcpy(dst + 80, &count, 4);

    for (uint32_t i = 0; i < triangles.size(); i++) {
        const vec3i t = triangles[i];
        const vec3f p0 = points[t.x];
        const vec3f p1 = points[t.y];
        const vec3f p2 = points[t.z];
        const vec3f normal = triangleNormal(p0, p1, p2);
        const uint64_t idx = 84 + i * 50;
        memcpy(dst + idx, &normal, 12);
        memcpy(dst + idx + 12, &p0, 12);
        memcpy(dst + idx + 24, &p1, 12);
        memcpy(dst + idx + 36, &p2, 12);
    }

    std::fstream file(filename, std::ios::out | std::ios::binary);
    file.write(dst, numBytes);
    file.close();

    free(dst);
	return true;
}


//bool Mesh::Save3MF(const std::string &filename, const std::vector<vec3f> &points, const std::vector<vec3i> &triangles)
bool Mesh::Save3MF(const std::string &filename)
{
	std::string comment = "";
	remove(filename.c_str());
	
	if(!mz_zip_add_mem_to_archive_file_in_place(filename.c_str(), "_rels/", NULL, 0, comment.c_str(), comment.size(), MZ_BEST_COMPRESSION)) return false;
	
	if(!mz_zip_add_mem_to_archive_file_in_place(filename.c_str(), "3D/", NULL, 0, comment.c_str(), comment.size(), MZ_BEST_COMPRESSION)) return false;
	
	std::ostringstream buffer;
	
	buffer << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	buffer << "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">" << std::endl;
	buffer << "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\" />" << std::endl;
	buffer << "<Default Extension=\"model\" ContentType=\"application/vnd.ms-package.3dmanufacturing-3dmodel+xml\" />" << std::endl;
	buffer << "</Types>" << std::endl;
	if (!mz_zip_add_mem_to_archive_file_in_place(filename.c_str(), "[Content_Types].xml", buffer.str().c_str(), buffer.str().size(), comment.c_str(), comment.size(), MZ_BEST_COMPRESSION)) return false;
	
	buffer.str("");
	buffer.clear();

	buffer << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	buffer << "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">" << std::endl;
	buffer << "<Relationship Target=\"/3D/3dmodel.model\" Id=\"rel0\" Type=\"http://schemas.microsoft.com/3dmanufacturing/2013/01/3dmodel\" />" << std::endl;
	buffer << "</Relationships>" << std::endl;
	if(!mz_zip_add_mem_to_archive_file_in_place(filename.c_str(), "_rels/.rels", buffer.str().c_str(), buffer.str().size(), comment.c_str(), comment.size(), MZ_BEST_COMPRESSION)) return false;
	
	buffer.str("");
	buffer.clear();
	
	buffer << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	buffer << "  <model unit=\"inch\" xmlns=\"http://schemas.microsoft.com/3dmanufacturing/2013/01\">" << std::endl;
	buffer << "    <resources>" << std::endl;
	buffer << "      <object id=\"0\" type=\"model\">" << std::endl;
	buffer << "        <mesh>" << std::endl;
	buffer << "          <vertices>" << std::endl;

	buffer << std::setiosflags(std::ios_base::fixed);
	buffer << std::setprecision(18);

	for (std::vector<vec3f>::const_iterator i = points.begin(); i != points.end(); i++)
		buffer << "            <vertex x=\"" << (*i).x << "\" y=\"" << (*i).y << "\" z=\"" << (*i).z << "\" />" << std::endl;

	buffer << "          </vertices>" << std::endl;
	buffer << "          <triangles>" << std::endl;

	for (std::vector<vec3i>::const_iterator i = triangles.begin(); i != triangles.end(); i++)
		buffer << "            <triangle v1=\"" << (*i).x << "\" v2=\"" << (*i).y << "\" v3=\"" << (*i).z << "\" />" << std::endl;

	buffer << "          </triangles>" << std::endl;
	buffer << "        </mesh>" << std::endl;
	buffer << "      </object>" << std::endl;
	buffer << "    </resources>" << std::endl;
	buffer << "    <build>" << std::endl;
	buffer << "      <item objectid=\"0\" />" << std::endl;
	buffer << "    </build>" << std::endl;
	buffer << "  </model>" << std::endl;
	
	if (!mz_zip_add_mem_to_archive_file_in_place(filename.c_str(), "3D/3dmodel.model", buffer.str().c_str(), buffer.str().size(), comment.c_str(), comment.size(), MZ_BEST_COMPRESSION)) return false;
	
	return true;
	
}
