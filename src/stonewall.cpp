#include <opencv2/opencv.hpp>

#include <string>
#include <random>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <set>
#include <algorithm>
#include <filesystem>

#include "heightmap.h"

#include "manifold/manifold.h"
#include "manifold/cross_section.h"
#include "meshio.h"

#include "libnoisetool.h"

#include "assimp/Exporter.hpp"
#include "assimp/Importer.hpp"
#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "manifold/optional_assert.h"
#include "manifold/linalg.h"

#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

/*
struct vec3f {
	float x, y, z;
};

struct vec3d {
	double x, y, z;
};

struct vec3i {
	unsigned x, y, z;
};
*/

//extract parameter statements with:   grep "//parm" ../src/contours.cpp | awk -F'//parm' '{printf "cout << \"" $2 "\" << endl << endl;\n" }'


using namespace cv;
using namespace std;

/*
std::vector<std::string> split(std::string s, std::string delim)
{
	std::vector<std::string> v;
	if (s.find(delim) == std::string::npos) {
		v.push_back(s);
		return v;
	}
	size_t pos=0;
	size_t start;
	while (pos < s.length()) {
		start = pos;
		pos = s.find(delim,pos);
		if (pos == std::string::npos) {
			v.push_back(s.substr(start,s.length()-start));
			return v;
		}
		v.push_back(s.substr(start, pos-start));
		pos += delim.length();
	}
	return v;
}
*/

vector<Point> translateContour(vector<Point> c, int x, int y)
{
	//printf("translateContour...\n"); fflush(stdout);
	vector<Point> t;
	for (auto p : c) 
		t.push_back(Point(p.x+x,p.y+y));
	return t;
}

vector<vector<float>> readTexture(string filename)
{
	//printf("readTexture...\n"); fflush(stdout);
	vector<vector<float>> t;
	ifstream inputFile(filename);
	if (inputFile.is_open()) {
		string line;
		while (getline(inputFile, line)) {
			vector<string> l = split(line, " ");
			vector<float> lt;
			for (auto n : l) lt.push_back(atof(n.c_str()));
			t.push_back(lt);
		}
		inputFile.close();
	}
	else cout << "texture file open failed." << endl;
	return t;
}

bool writeTexture(vector<vector<float>> texture, string filename)
{
	//printf("writeTexture...\n"); fflush(stdout);
	std::ofstream outputFile(filename);
	if (!outputFile.is_open()) {
		return false;
	}
	else {
		for (auto l : texture) {
			for (auto t : l) 
				outputFile << t << " ";
			outputFile << endl;
		}
	}
	outputFile.close();
	return true;
}

void printTexture(vector<vector<float>> texture)
{
	//printf("printTexture...\n"); fflush(stdout);
	for (auto r : texture) {
		for (auto c : r) {
			//cout << c << " ";
			printf("%1.1f ", c); fflush(stdout);
		}
		cout << endl;
	}
}

float minTextureValue(vector<vector<float>> texture)
{
	float min = std::numeric_limits<float>::max();
	for (auto r : texture) 
		for (auto c : r) 
			if (c < min) min = c;
	return min;
}

void mapTexture(vector<vector<float>> &texture, vector<Point> contour)
{
	//printf("mapTexture...\n"); fflush(stdout);
	Rect r = boundingRect(contour);
	int cx = r.x;
	int cy = r.y;
	for (unsigned ty=0; ty<texture.size(); ty++) {
		for (unsigned tx=0; tx<texture[ty].size(); tx++) {
			Point pt; pt.x = cx+tx; pt.y = cy+ty;
			if (pointPolygonTest(contour, pt, false) <= 0.0)
				texture[ty][tx] = 0;
		}
	}
}

vector<vector<float>> initTexture(unsigned w, unsigned h, double v)
{
	//printf("initTexture...\n"); fflush(stdout);
	vector<vector<float>> t(h, vector<float>(w));
	for (unsigned ty=0; ty<t.size(); ty++) {
		for (unsigned tx=0; tx<t[ty].size(); tx++) {
			t[ty][tx] = v;
		}
	}
	return t;
}

void bevelTextureOrig(vector<vector<float>> &texture, vector<Point> contour)
{	
	//printf("bevelTexture...\n"); fflush(stdout);
	for (int i=0; i<texture.size(); i++) {
	//for (int i=texture.size()-1; i>=0; i--) {
		for (int j=0; j<texture[i].size(); j++) {
		//for (int j=texture[0].size()-1; j>=0; j--) {   //reverse order of y column
			Point pt(j,i);
			double b = pointPolygonTest(contour, pt, false);
			if (b == 0.0)
				texture[i][j] = 0.5;
			else if (b == -1)
				texture[i][j] = 0.0;
			else
				texture[i][j] = 1.0;
		}
	}
}

vector<Point> scaleContour(vector<Point> contour, float xscale, float yscale)
{
	vector<Point> c;
	Rect r = boundingRect(contour);
	Moments M = moments(contour);
	int cx = int(M.m10 / M.m00);
	int cy = int(M.m01 / M.m00);
	c = translateContour(contour, -cx, -cy);
	for (auto &p : c) {
		p.x *= xscale;
		p.y *= yscale;
	}
	c = translateContour(c, cx, cy);
	return c;
}

void bevelTexture(vector<vector<float>> &texture, vector<Point> contour, int bevelevels)
{
	Rect r = boundingRect(contour);
	//cout << "  width: " << r.width << "  height: " << r.height << endl;

	for (int x=0; x<texture.size(); x++) {
		for (int y=0; y<texture[x].size(); y++) {
			Point pt(y,x);
			double b = pointPolygonTest(contour, pt, false);
			if (b <= 0.0)
				texture[x][y] = 0.0;
			else
				texture[x][y] = 1.0;
		}
	}
	
	float inc = 1.0 / (float) bevelevels;
	float in = inc;
	
	for(int i=1; i<bevelevels; i++) {
		float xscale = ((float) r.width-(float) i) / (float) r.width;
		float yscale = ((float) r.height-(float) i) / (float) r.height;
		vector<Point> cont =  scaleContour(contour, xscale, yscale);
		Rect rc = boundingRect(cont);
		//cout << setprecision(2)<< "  i: " << i << "  xscale: " << xscale << "  yscale: " << yscale << "  width: " << rc.width << "  height: " << rc.height << " bevel inc: " << in << endl;
		for (int x=0; x<texture.size(); x++) {
			for (int y=0; y<texture[x].size(); y++) {
				Point pt(y,x);
				double bc = pointPolygonTest(cont, pt, false);
				if (bc == 0.0) texture[x][y] = in;
			}
		}
		in += inc;
	}
}


vector<vector<float>> flipTexture(vector<vector<float>> texture, bool fx, bool fy)
{
	vector<vector<float>> f = texture;
	for (int x=0; x<texture.size(); x++) {
		int rx;
		if (fx) rx = (texture.size()-1) - x; else rx = x;
		for (int y=0; y<texture[x].size(); y++) {
			int ry;
			if (fy) ry = (texture[x].size()-1) - y; else ry = y;
			f[x][y] = texture[rx][ry];
		}
	}
	return f;
}

void multiplyTexture(vector<vector<float>> &texture, vector<vector<float>> t)
{
	//printf("multiplyTexture...\n"); fflush(stdout);
	if (texture.size() != t.size()) return;
	if (texture[0].size() != t[0].size()) return;
	for (unsigned i=0; i<texture.size(); i++) 
		for (unsigned j=0; j<texture[0].size(); j++) 
			texture[i][j] *= t[i][j];
}

void subtractValTexture(vector<vector<float>> &texture, float s)
{
	//printf("subtractValTexture...\n"); fflush(stdout);
	for (unsigned i=0; i<texture.size(); i++) 
		for (unsigned j=0; j<texture[0].size(); j++) 
			texture[i][j] -= s;
}

vector<vector<float>> invertTexture(vector<vector<float>> texture, bool ud, bool lr)
{
	unsigned w = texture.size();
	unsigned h = texture[0].size();
	vector<vector<float>> t(w, vector<float>(h));
	
	if (ud) {
		for (unsigned i=0; i<texture.size(); i++) 
			for (unsigned j=0; j<texture[0].size(); j++) 
				t[i][(h-1)-j] = texture[i][j];
	}
	
	if (lr) {
		for (unsigned i=0; i<texture.size(); i++) 
			for (unsigned j=0; j<texture[0].size(); j++) 
				t[(w-1)-i][j] = texture[i][j];
	}
	
	return t;
}

/*
std::vector<std::vector<float>> flipTexture(std::vector<std::vector<float>> texture, bool fx, bool fy)
{
        std::vector<std::vector<float>> f = texture;
        for (int x=0; x<texture.size(); x++) {
                int rx;
                if (fx) rx = (texture.size()-1) - x; else rx = x;
                for (int y=0; y<texture[x].size(); y++) {
                        int ry;
                        if (fy) ry = (texture[x].size()-1) - y; else ry = y;
                        f[x][y] = texture[rx][ry];
                }
        }
        return f;
}
*/

void err(string msg)
{
	cout << msg << endl;
	exit(EXIT_FAILURE);
}

string val(string nameval)
{
	vector<string> nv = split(nameval, "=");
	if (nv.size() >= 2) return nv[1];
	return "";
}

float random_number(int lower, int upper)
{
	return (float) ((rand() % upper) + lower);
}

int main(int argc, char **argv) {
	
	if (argc < 2) {	
		err("No input image specified");
		//exit(EXIT_SUCCESS);
	}
	
	//set up the random number generator
	srand(static_cast<unsigned int>(time(0)));
	
    Mat image = imread(argv[1]);

    if (image.empty()) err("Could not open or find the image."); 
	
	unsigned thresh = 128;
	float epsilon = 0.0;
	bool border = false, resize_image = false, boundingbox = false, bashdims = false, cmddims = false, noisetexture=false, doscale=false, openscadarrays=false, debug = false;
	int bw = 1;  // border width, default = 1
	unsigned minarea = 0, minpoints=4;
	unsigned rw, rh;
	string noisefile, fileextension="3mf";
	float baseheight = 1.0;
	float scale = 1.0;
	int bevelevels = 1;
	
	bool foundbounds=false;
	
	int texturetest = -1;
	int onestone = -1;
	
	string destimage = "";
	
	//set up the random number generator
	//random_device seed;
	//mt19937 gen{seed()};
	//uniform_int_distribution<> dist{1, 150};
	
	for (unsigned i=1; i<argc; i++) {
		 if(string(argv[i]).find("destimage") != string::npos) {  //parm destimage: if defined, outputs a copy of the original image wtih the contours drawn in red and the contour numbers in the center.  Does not generate stones.
			destimage = val(argv[i]);
		}
		else if (string(argv[i]).find("openscadarrays") != string::npos) { //parm openscadarrays: spits out OpenSCAD arrays for contour width/heights, centers, translation, and a set of [0,0,0] arrays for user-specified translation (usually different height to make individual stones "stand proud".  Does not generate stones.
			openscadarrays=true;
		}
		else if(string(argv[i]).find("threshold") != string::npos) {  //parm threshold: rubicon between black and white for the gray->binary translation, betwee 0 and 255.  Default: 128
			string t = val(argv[i]);
			if (t.size() > 0) thresh = atoi(t.c_str());
		}
		if(string(argv[i]).find("resize") != string::npos) {  //parm resize: if defined, resizes the input image using WxH, e.g., 'resize:640x480'.  If either width or height is not specified, the resize of that dimension is calculated with the proportion of the defined dimension to the original image dimension, e.g., 'resize:640', or 'resize:x480'
			string r = val(argv[i]);
			vector<string> dim = split(r, "x");
			if (dim[0].size() == 0 & dim[1].size() == 0) err("invalid dimension");
			if (dim.size() == 1) {
				rw = atoi(r.c_str());
				rh = int((float) image.rows * ((float) rw / (float) image.cols));
				resize_image = true;
			}
			else if (dim.size() == 2) {
				rh = atoi(dim[1].c_str());
				if (dim[0].size() == 0) 
					rw = int((float) image.cols * ((float) rh / (float) image.rows));
				else 
					rw = atoi(dim[0].c_str());
				resize_image = true;
			}
		}
		else if(string(argv[i]).find("epsilon") != string::npos) {  //parm epsilon: The value used to specify the degree of simplification of contours, larger is simpler.  Set to 0.0 to disable.  Default: 0.0
			string e = val(argv[i]);
			if (e.size() > 0) epsilon = atof(e.c_str());
		}
		else if(string(argv[i]).find("border") != string::npos) { //parm border: if defined, draws a white border around the image, useful for isolating contours that butt up against the image edge.  Default width: 1
			border = true;
			string b = val(argv[i]);
			if (b.size() > 0) bw = atoi(b.c_str());
		}
		else if(string(argv[i]).find("minarea") != string::npos) { //parm minarea: minimum area of a valid contour.  Default: 0
			string m = val(argv[i]);
			if (m.size() > 0) minarea = atoi(m.c_str());
		}
		else if(string(argv[i]).find("minpoints") != string::npos) {  //parm minpoints: culls polygons with number of points less than this number.  Default: 4
			string m = val(argv[i]);
			if (m.size() > 0) minpoints = atoi(m.c_str());
		}
		else if (string(argv[i]).find("boundingbox") != string::npos) { //parm boundingbox: if defined, the polygons are four-point polygons describing the contours' bounding boxes
			boundingbox = true;
		}
		else if (string(argv[i]).find("noisefile") != string::npos) { //parm noisefile: the noise network to pass to noisetool to get the texture
			noisefile = val(argv[i]);
			noisetexture = true;
		}
		else if (string(argv[i]).find("baseheight") != string::npos) { //parm baseheight: thickness of the base munged to the bottom of the texture
			baseheight = atof(val(argv[i]).c_str());
		}
		else if (string(argv[i]).find("bevelevels") != string::npos) { //parm bevelevels: number of increments to bevel stone edges.  Default=1
			string e = val(argv[i]);
			if (e.size() > 0) bevelevels = atoi(e.c_str());
		}
		else if (string(argv[i]).find("scale") != string::npos) { //parm scale: thickness of the base munged to the bottom of the texture
			scale = atof(val(argv[i]).c_str());
			doscale=true;
		}
		else if (string(argv[i]).find("fileextension") != string::npos) { //parm fileextension: file type to save stones.  Default: 3mf
			fileextension = val(argv[i]);
		}
		else if (string(argv[i]).find("debug") != string::npos) { 
			debug = true;
		}
		else if (string(argv[i]).find("onestone") != string::npos) { 
			onestone = atoi(val(argv[i]).c_str());
		}
	}
	
	if (debug)
		if (!std::filesystem::is_directory(std::filesystem::path("stonetest")))
			 std::filesystem::create_directory(std::filesystem::path("stonetest"));
	
	if (resize_image) {
		resize(image, image, Size(rw, rh), 0, 0, INTER_LANCZOS4);
	}
	
	if (border) {
		copyMakeBorder( image, image, bw, bw, bw, bw, BORDER_CONSTANT, Scalar(0,0,0) );
	}

    // Convert to grayscale
    Mat gray;
    cvtColor(image, gray, COLOR_BGR2GRAY);

    // Threshold to create a binary image
    Mat binary;
    threshold(gray, binary, thresh, 255, THRESH_BINARY);

    // Find contours
    vector<vector<Point>> contours, culledcontours;
	vector<Vec4i> hierarchy;
    //findContours(binary, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
	findContours(binary, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE);
	
	//cull contours and points
	for (const auto& contour : contours) {
		if (contour.size() <= minpoints) continue;  //invalid contour
		if (contourArea(contour) < minarea) continue;  //too small
		vector<Point> pts;
		if (boundingbox) {
			fprintf(stderr, "using bounding box..\n"); fflush(stderr);
			Rect bb = boundingRect(contour);
			vector<Point> pts;
			pts.push_back(Point(bb.x, bb.y));
			pts.push_back(Point(bb.x+bb.width, bb.y));
			pts.push_back(Point(bb.x+bb.width, bb.y+bb.height));
			pts.push_back(Point(bb.x, bb.y+bb.height));
			culledcontours.push_back(pts);
		}	
		else if (epsilon > 0.0) { 
			vector<Point> pts;
			approxPolyDP(contour, pts, epsilon, true);
			culledcontours.push_back(pts);
		}
		else {
			culledcontours.push_back(contour);
		}
	}

	if (destimage.size() != 0) { //generate contour outline/index number image:
		unsigned count=0;
		for (const auto& contour : culledcontours) {
			drawContours(image, vector<vector<Point>>{contour}, 0, Scalar(0, 0, 255), 1);
			Moments M = moments(contour);
			int cx = int(M.m10 / M.m00);
			int cy = int(M.m01 / M.m00);
			int baseline=0;
			float fontScale = 0.5;
			Size s = getTextSize(std::to_string(count), FONT_HERSHEY_COMPLEX_SMALL, fontScale, 1, &baseline);
			cx -= s.width/2;
			cy += s.height/2;
			putText(image, std::to_string(count), Point(cx,cy), FONT_HERSHEY_COMPLEX_SMALL, fontScale, Scalar(0,0,255), 1, LINE_AA);
			count++;
		}
		cv::imwrite(destimage, image);
	}
	else if (openscadarrays) { //spit OpenSCAD arrays to stdout
		//center points of each contour:
		cout << endl << "//center points" << endl;
		cout << "pc = [" << endl;
		for (const auto& contour : culledcontours) {
			Moments M = moments(contour);
			int cx = int(M.m10 / M.m00);
			int cy = int(M.m01 / M.m00);
			cout << "  [" << cx << "," << cy << "]";
			if (contour != culledcontours[culledcontours.size()-1]) cout << "," << endl; else cout << endl;
		}
		cout << "];" << endl << endl;
		
		//widths/heights of each contour:
		cout << endl << "//widths/heights" << endl;
		cout << "pw = [" << endl;
		for (const auto& contour : culledcontours) {
			Rect r = boundingRect(contour);
			int wx = r.width-1;
			int wy = r.height-1;
			cout << "  [" << wx << "," << wy << "]";
			if (contour != culledcontours[culledcontours.size()-1]) cout << "," << endl; else cout << endl;
		}
		cout << "];" << endl;
		
		cout << endl << "//translate coordinates" << endl;
		cout << "pt = [" << endl;
		for (const auto& contour : culledcontours) {
			Rect r = boundingRect(contour);
			int x = r.x;
			int y = r.y;
			cout << "  [" << x << "," << y << "," << "0" << "]";
			if (contour != culledcontours[culledcontours.size()-1]) cout << "," << endl; else cout << endl;
		}
		cout << "];" << endl;
		
		cout << endl << "//user translate coordinates" << endl;
		cout << "pr = [" << endl;
		int ccount = 0;
		for (const auto& contour : culledcontours) {
			cout << "  [" << 0 << "," << 0 << "," << 0 << "]";
			if (contour != culledcontours[culledcontours.size()-1]) cout << ","; 
			cout << "  // " << ccount << endl;
			ccount++;
		}
		cout << "];" << endl;
	}
	else {  //do the stone wall thing
		unsigned c = 0;
		if (onestone > -1) {
			culledcontours = { culledcontours[onestone] };
			c = onestone;
		}
		for (const auto& contour : culledcontours) {
			
			Rect r = boundingRect(contour);
			int bxi = random_number(0, 1000);
			int byi = random_number(0, 1000);
			int bwi = r.width;
			int bhi = r.height;
			int dwi = bwi;
			int dhi = bhi;
			
			//cout << "bounds: " << bxi << "," << byi << "," << bwi << "," << bhi << endl;
			
			string bx = to_string(bxi);
			string by = to_string(byi);
			string bw = to_string(bwi);
			string bh = to_string(bhi);
			string dw = bw;
			string dh = bh;
			
			vector<vector<float>> tex;
			if (noisetexture) {
				
				LibNoiseTool nt;
				nt.loadNetwork(noisefile);
				nt.setBounds(bxi, byi, bwi, bhi);
				nt.setDestSize(dwi, dhi);
				nt.buildNetwork();
				tex = nt.getHeightMap();
			}
			else {
				tex = initTexture(r.width, r.height, 1.0);
				
			}

			//shape the texture to the contour:
			vector<Point> tc = translateContour(contour, -r.x, -r.y);		//get a copy of the contour translated to 0,0
			vector<vector<float>> mu = initTexture(r.width, r.height, 1);	//multiplier texture
			bevelTexture(mu, tc, bevelevels);								//apply bevel multipliers to multiplier texture
			multiplyTexture(tex, mu);										//multiply texture by the multiplier texture
			tex = flipTexture(tex, false, true);							//should probably call it mirrorTexture, doing this because my notions of xy in texture are AFU
			
			if (debug) writeTexture(tex, "stonetest/tex"+to_string(c)+".txt");
			if (debug) writeTexture(mu, "stonetest/mu"+to_string(c)+".txt");			
			
			//put bottom of texture at zero:
			float texmin = minTextureValue(tex);
			subtractValTexture(tex, texmin);
			
			int w = tex.size();
			int h = tex[0].size();
			
			//make manifold mesh from the heightmap texture:
			std::pair<std::vector<vec3f>, std::vector<vec3i>> msh = heightmap2Mesh(tex, 1);
			auto points = msh.first;
			auto triangles = msh.second;
			
			//load texture mesh into a Manifold object:
			manifold::MeshGL mesh;
			for (auto p : points) 
				mesh.vertProperties.insert(mesh.vertProperties.end(), {p.x, p.y, p.z});
			for (auto t : triangles)
				mesh.triVerts.insert(mesh.triVerts.end(), { (unsigned) t.x, (unsigned) t.y, (unsigned) t.z});
			mesh.Merge();
			manifold::Manifold stonetext(mesh);
			stonetext = stonetext.Rotate(0,0,90.0).Translate({(double) (h-1),0,0});  //because heightmap lays out heightmaps differently than OpenSCAD
			
			//translate texture mesh to the contour location:
			stonetext = stonetext.Translate({(double) r.x, (double) r.y, 0.0});
			
			if (debug) manifold::ExportMesh("stonetest/tex"+to_string(c)+".stl", stonetext.GetMeshGL(), {});
			
			//make a Manifold of the contour polygon:
			manifold::SimplePolygon p;
			for (const auto& point : contour) 
				p.push_back({(double) point.x, (double) point.y});
			if (contourArea(contour, true) < 0.0) std::reverse(p.begin(), p.end());  //determines winding of the polygon, reverses order of points if not CCW
			manifold::Polygons xc;
			xc.push_back(p);
			//xc = manifold::CrossSection(xc).ToPolygons();
			manifold::Manifold stonecont = manifold::Manifold::Extrude(xc,20);

			//translate contour down to encompass texture:
			stonecont = stonecont.Translate({0.0, 0.0, -5.0});
			
			if (debug) manifold::ExportMesh("stonetest/cont"+to_string(c)+".stl", stonecont.GetMeshGL(), {});
			
			//intersect the contour and texture
			manifold::Manifold stone = stonecont.Boolean(stonetext, manifold::OpType::Intersect);
			
			//scale the stone assembly, if specified
			
			if (doscale) {
				cout << "scaling stone by " << scale << endl;
				stone = stone.Scale({scale,scale,scale});
			}
			
			//save the stone mesh:
			manifold::ExportMesh(to_string(c)+"."+fileextension, stone.GetMeshGL(), {});
			
			//cout << "stone: " << c << "." << fileextension << " at " << r.x << "," << r.y << endl;

			//increment the stone number:
			c++;
		}

	}

    return 0;
}
