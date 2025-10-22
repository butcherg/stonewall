#ifndef _LIBNOISETOOL_
#define _LIBNOISETOOL_

#include <noise.h>
#include "noiseutils.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <limits>

#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

#include <stdlib.h>
#include <stdarg.h>





class LibNoiseTool
{
public:
	LibNoiseTool();
	
	bool loadNetwork(std::string filename);
	std::string getNetworkDigraph();
	bool buildNetwork();
	void writeNetwork();
	std::vector<std::vector<float>> getHeightMap();

	
	void setBounds_XYWH(float x, float y, float w, float h);
	void setBounds_XXYY(float x1, float x2, float y1, float y2);
	void setDestFile(std::string filename);
	void setBounds(float x, float y, float w, float h);
	void setDestSize(int width, int height);
	void setBuilder(std::string builder);
	
	

private:
	void addDDNode(std::string name, std::string caption, std::string note);
	void parse_err(std::string msg);
	float random_number(int lower, int upper);
	//std::vector<std::string> split(std::string s, std::string delim);
	//std::string string_format(const std::string fmt, ...) ;
	
	void parseCache(std::string parms);
	void parseCheckerboard(std::string parms);
	void parseCylinders(std::string parms);
	void parsePerlin(std::string parms);
	void parseRidgedMulti(std::string parms);
	void parseTurbulence(std::string parms);
	void parseVoronoi(std::string parms);
	void parseBillow(std::string parms);
	void parseConst(std::string parms);
	void parseAdd(std::string parms);
	void parseAbs(std::string parms);
	void parseClamp(std::string parms);
	void parseCurve(std::string parms);
	void parseBlend(std::string parms);
	void parseMultiply(std::string parms);
	void parseSelect(std::string parms);
	void parseMax(std::string parms);
	void parseMin(std::string parms);
	void parsePower(std::string parms);
	void parseScaleBias(std::string parms);
	void parseRotate(std::string parms);
	void parseScalePoint(std::string parms);
	void parseDisplace(std::string parms);
	void parseInvert(std::string parms);
	void parseConnect(std::string parms);
	
	void parseOutput(std::string parms);
	
	std::vector<std::string> split(std::string s, std::string delim);
	std::string string_format(const std::string fmt, ...);

	

	std::map<std::string, module::Module*> modules;
	std::string dd;
	bool netparsed, netbuilt, netwrote;
	
	unsigned lineno;
	std::string outputmodule, outputname, builder;
	
	utils::NoiseMap heightMap;
	
	float lowerXbound;  float upperXbound;  float lowerYbound;  float upperYbound;
	int destw; int desth;
	std::string destfile;


};	


#endif



