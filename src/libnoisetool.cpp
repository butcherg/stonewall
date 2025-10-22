

#include "libnoisetool.h"

LibNoiseTool::LibNoiseTool()
{
	lowerXbound = 10.0;  upperXbound = 15.0;  lowerYbound = 10.0;  upperYbound = 15.0;
	netparsed = netbuilt = netwrote = false;
}

std::string LibNoiseTool::getNetworkDigraph() 
{
	return dd;
}

void LibNoiseTool::addDDNode(std::string name, std::string caption, std::string note)
{
	dd.append("\t"+name+" [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"2\"><TR><TD>"+caption+"</TD></TR><TR><TD ALIGN=\"LEFT\"><FONT POINT-SIZE=\"6\">"+note+"</FONT></TD></TR></TABLE>>];\n");

}

void LibNoiseTool::parse_err(std::string msg)
{
	printf("Error: %s. (line %d)\n", msg.c_str(), lineno);
	exit(EXIT_FAILURE);
}


float LibNoiseTool::random_number(int lower, int upper)
{
	//srand(static_cast<unsigned int>(time(0)));
	return (float) ((rand() % upper) + lower);
}

void LibNoiseTool::setBounds_XYWH(float x, float y, float w, float h)
{
	lowerXbound = x;  upperXbound = x+w;  lowerYbound = y;  upperYbound = y+h;
}

void LibNoiseTool::setBounds_XXYY(float x1, float x2, float y1, float y2)
{
	lowerXbound = x1;  upperXbound = x2;  lowerYbound = y1;  upperYbound = y2;
}

void LibNoiseTool::setDestFile(std::string filename)
{
	destfile = filename;
}

void LibNoiseTool::setBounds(float x, float y, float w, float h)
{
	setBounds_XYWH(x, y, w, h);
}
void LibNoiseTool::setDestSize(int width, int height)
{
	destw=width; desth=height;
}

void LibNoiseTool::setBuilder(std::string bldr)
{
	if (bldr=="plane" | bldr=="sphere" | bldr=="brick")
		builder=bldr;
	else
		parse_err(string_format("Invalid builder: %s", bldr.c_str()));
}



void LibNoiseTool::parseCache(std::string parms)
{
	std::string name, ddnote="module::Cache\n";
	module::Cache *s = new module::Cache();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos) { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Cache", ddnote);
}


//class generator Checkerboard
void LibNoiseTool::parseCheckerboard(std::string parms)
{
	//printf("%s\n", parms.c_str());
	std::string name, ddnote="module::Checkerboard";
	module::Checkerboard *p = new module::Checkerboard();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = p;
	addDDNode(name, "Checkerboard", ddnote);
}

//class generator Cylinders
void LibNoiseTool::parseCylinders(std::string parms)
{
	//printf("%s\n", parms.c_str());
	std::string name, ddnote="module::Cylinders";
	module::Cylinders *p = new module::Cylinders();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else if (parm[0].find("freq") != std::string::npos) { p->SetFrequency(atof(parm[1].c_str())); ddnote.append("<BR/>frequency: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = p;
	addDDNode(name, "Cylinders", ddnote);
}

//class generator Perlin
void LibNoiseTool::parsePerlin(std::string parms)
{
	//printf("%s\n", parms.c_str());
	std::string name, ddnote="module::Perlin\n";
	module::Perlin *p = new module::Perlin();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else if (parm[0].find("freq") != std::string::npos) { p->SetFrequency(atof(parm[1].c_str())); ddnote.append("<BR/>frequency: "+parm[1]); }
		else if (parm[0].find("lac") != std::string::npos)  { p->SetLacunarity(atof(parm[1].c_str())); ddnote.append("<BR/>lacunarity: "+parm[1]); }
		else if (parm[0].find("per") != std::string::npos)  { p->SetPersistence(atof(parm[1].c_str())); ddnote.append("<BR/>persistence: "+parm[1]); }
		else if (parm[0].find("oct") != std::string::npos)  { p->SetOctaveCount(atoi(parm[1].c_str())); ddnote.append("<BR/>octaves: "+parm[1]); }
		else if (parm[0].find("seed") != std::string::npos) { p->SetSeed(atoi(parm[1].c_str())); ddnote.append("<BR/>seed: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = p;
	addDDNode(name, "Perlin", ddnote);
}

//class generator RidgedMulti
void LibNoiseTool::parseRidgedMulti(std::string parms)
{
	std::string name, ddnote="module::RidgedMulti\n";
	module::RidgedMulti *p = new module::RidgedMulti();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else if (parm[0].find("freq") != std::string::npos) { p->SetFrequency(atof(parm[1].c_str())); ddnote.append("<BR/>frequency: "+parm[1]); }
		else if (parm[0].find("lac") != std::string::npos)  { p->SetLacunarity(atof(parm[1].c_str())); ddnote.append("<BR/>lacunarity: "+parm[1]); }
		else if (parm[0].find("oct") != std::string::npos)  { p->SetOctaveCount(atoi(parm[1].c_str())); ddnote.append("<BR/>octaves: "+parm[1]); }
		else if (parm[0].find("seed") != std::string::npos) { p->SetSeed(atoi(parm[1].c_str())); ddnote.append("<BR/>seed: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = p;
	addDDNode(name, "RidgedMulti", ddnote);
}

//class generator Turbulence
void LibNoiseTool::parseTurbulence(std::string parms)
{
	std::string name, ddnote="module::Turbulence\n";
	module::Turbulence *t = new module::Turbulence();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else if (parm[0].find("freq") != std::string::npos) { t->SetFrequency(atof(parm[1].c_str())); ddnote.append("<BR/>frequency: "+parm[1]); }
		else if (parm[0].find("pow") != std::string::npos)  { t->SetPower(atof(parm[1].c_str())); ddnote.append("<BR/>power: "+parm[1]); }
		else if (parm[0].find("roug") != std::string::npos) { t->SetRoughness(atoi(parm[1].c_str()));  ddnote.append("<BR/>roughness: "+parm[1]); }
		else if (parm[0].find("seed") != std::string::npos) { t->SetSeed(atoi(parm[1].c_str())); ddnote.append("<BR/>seed: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = t;
	addDDNode(name, "Turbulence", ddnote);
}

//class generator Voronoi
void LibNoiseTool::parseVoronoi(std::string parms)
{
	std::string name, ddnote="module::Voronoi\n";
	module::Voronoi *v = new module::Voronoi();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else if (parm[0].find("dist") != std::string::npos) { v->EnableDistance(true); ddnote.append("<BR/>distance: true"); }
		else if (parm[0].find("disp") != std::string::npos) { v->SetDisplacement(atof(parm[1].c_str())); ddnote.append("<BR/>displacement: "+parm[1]); }
		else if (parm[0].find("freq") != std::string::npos) { v->SetFrequency(atof(parm[1].c_str())); ddnote.append("<BR/>frequency: "+parm[1]); }
		else if (parm[0].find("seed") != std::string::npos) { v->SetSeed(atoi(parm[1].c_str())); ddnote.append("<BR/>seed: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = v;
	addDDNode(name, "Voronoi", ddnote);
}

//class generator Billow
void LibNoiseTool::parseBillow(std::string parms)
{
	std::string name, ddnote="module::Billow\n";
	module::Billow *b = new module::Billow();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else if (parm[0].find("freq") != std::string::npos) { b->SetFrequency(atof(parm[1].c_str())); ddnote.append("<BR/>frequency: "+parm[1]); }
		else if (parm[0].find("lac") != std::string::npos)  { b->SetLacunarity(atof(parm[1].c_str())); ddnote.append("<BR/>lacunarity: "+parm[1]); }
		else if (parm[0].find("per") != std::string::npos)  { b->SetPersistence(atof(parm[1].c_str())); ddnote.append("<BR/>persistence: "+parm[1]); }
		else if (parm[0].find("oct") != std::string::npos)  { b->SetOctaveCount(atoi(parm[1].c_str())); ddnote.append("<BR/>octaves: "+parm[1]); }
		else if (parm[0].find("seed") != std::string::npos) { b->SetSeed(atoi(parm[1].c_str())); ddnote.append("<BR/>seed: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = b;
	addDDNode(name, "Billow", ddnote);
}

//class generator Const
void LibNoiseTool::parseConst(std::string parms)
{
	std::string name, ddnote="module::Const\n";
	module::Const *s = new module::Const();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); } 
		else if (parm[0].find("const") != std::string::npos) { s->SetConstValue(atoi(parm[1].c_str())); ddnote.append("<BR/>const: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Const", ddnote);
}

//class aggregator Add
void LibNoiseTool::parseAdd(std::string parms)
{
	std::string name, ddnote="module::Add\n";
	module::Add *s = new module::Add();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Add", ddnote);
}

//class modifier Abs
void LibNoiseTool::parseAbs(std::string parms)
{
	std::string name, ddnote="module::Abs\n";
	module::Abs *s = new module::Abs();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Abs", ddnote);
}

//class modifier Clamp
void LibNoiseTool::parseClamp(std::string parms)
{
	std::string name, ddnote="module::Clamp\n";
	module::Clamp *s = new module::Clamp();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else if (parm[0].find("bounds") != std::string::npos) {
			std::vector<std::string> bb =  split(std::string(parm[1]), ",");
			s->SetBounds(atof(bb[0].c_str()), atof(bb[1].c_str())); 
			ddnote.append("<BR/>bounds: "+parm[1]); 
		}
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Clamp", ddnote);
}

//class modifier Curve
void LibNoiseTool::parseCurve(std::string parms)
{
	std::string name, ddnote="module::Curve\n";
	module::Curve *s = new module::Curve();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else if (parm[0].find("ctrlpoint") != std::string::npos) {
			std::vector<std::string> bb =  split(std::string(parm[1]), ",");
			s->AddControlPoint(atof(bb[0].c_str()), atof(bb[1].c_str())); 
			ddnote.append("<BR/>ctrlpoint: "+parm[1]); 
		}
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Curve", ddnote);
}

//class aggregator Blend
void LibNoiseTool::parseBlend(std::string parms)
{
	std::string name, ddnote="module::Blend\n";
	module::Blend *s = new module::Blend();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Blend", ddnote);
}

//class aggregator Multiply
void LibNoiseTool::parseMultiply(std::string parms)
{
	std::string name, ddnote="module::Multiply\n";
	module::Multiply *s = new module::Multiply();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Multiply", ddnote);
}

//class aggregator Select
void LibNoiseTool::parseSelect(std::string parms)
{
	std::string name, ddnote="module::Select\n";
	module::Select *s = new module::Select();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else if (parm[0].find("bounds") != std::string::npos) {
			std::vector<std::string> bb =  split(std::string(parm[1]), ",");
			s->SetBounds(atof(bb[0].c_str()), atof(bb[1].c_str())); 
			ddnote.append("<BR/>bounds: "+parm[1]); 
		}
		else if (parm[0].find("edge") != std::string::npos) { s->SetEdgeFalloff(atof(parm[1].c_str())); ddnote.append("<BR/>edgefalloff: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Select", ddnote);
}

//class aggregator Max
void LibNoiseTool::parseMax(std::string parms)
{
	std::string name, ddnote="module::Max\n";
	module::Max *s = new module::Max();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos) { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Max", ddnote);
}

//class aggregator Min
void LibNoiseTool::parseMin(std::string parms)
{
	std::string name, ddnote="module::Min\n";
	module::Min *s = new module::Min();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Min", ddnote);
}

//class aggregator Power
void LibNoiseTool::parsePower(std::string parms)
{
	std::string name, ddnote="module::Power\n";
	module::Power *s = new module::Power();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos) { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Power", ddnote);
}

//class modifier ScaleBias
void LibNoiseTool::parseScaleBias(std::string parms)
{
	std::string name, ddnote="module::ScaleBias\n";
	module::ScaleBias *s = new module::ScaleBias();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else if (parm[0].find("scale") != std::string::npos) { s->SetScale(atof(parm[1].c_str())); ddnote.append("<BR/>scale: "+parm[1]); }
		else if (parm[0].find("bias") != std::string::npos)  { s->SetBias(atof(parm[1].c_str())); ddnote.append("<BR/>bias: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "ScaleBias", ddnote);
}

void LibNoiseTool::parseRotate(std::string parms)
{
	std::string name, ddnote="module::Rotate\n";
	module::RotatePoint  *s = new module::RotatePoint ();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else if (parm[0].find("X") != std::string::npos) { s->SetXAngle(atof(parm[1].c_str())); ddnote.append("<BR/>X: "+parm[1]); }
		else if (parm[0].find("Y") != std::string::npos)  { s->SetYAngle(atof(parm[1].c_str())); ddnote.append("<BR/>Y: "+parm[1]); }
		else if (parm[0].find("Z") != std::string::npos)  { s->SetZAngle(atof(parm[1].c_str())); ddnote.append("<BR/>Z: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Rotate", ddnote);
}

//class modifier ScalePoint
void LibNoiseTool::parseScalePoint(std::string parms)
{
	std::string name, ddnote="module::ScalePoint\n";
	module::ScalePoint *s = new module::ScalePoint();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else if (parm[0].find("scale") != std::string::npos) { 
			std::vector<std::string> ss =  split(std::string(parm[1]), ",");
			if (ss.size() == 1) 
				s->SetScale(atof(ss[0].c_str()));
			else if (ss.size() == 3) 
				s->SetScale(atof(ss[0].c_str()), atof(ss[1].c_str()), atof(ss[2].c_str()));
			else 
				parse_err(string_format("Malformed scale parameter: %s", parm[0].c_str()));
			ddnote.append("<BR/>scale: "+parm[1]); 
		}
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "ScalePoint", ddnote);
}

//class modifier Displace
void LibNoiseTool::parseDisplace(std::string parms)
{
	std::string name, ddnote="module::Displace\n";
	module::Displace *s = new module::Displace();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Displace", ddnote);
}

//class modifier Invert
void LibNoiseTool::parseInvert(std::string parms)
{
	std::string name, ddnote="module::Invert\n";
	module::Invert *s = new module::Invert();
	std::vector<std::string> pp =  split(std::string(parms), ";");
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0].find("name") != std::string::npos)  { name = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	modules[name] = s;
	addDDNode(name, "Invert", ddnote);
}


//class connector
void LibNoiseTool::parseConnect(std::string parms)
{
	std::string source, sink;
	int instance = 0;
	if (parms.find("=") != std::string::npos) {
		std::vector<std::string> pp =  split(std::string(parms), ";");
		for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
			std::vector<std::string> parm =  split(*it, "=");
			if (parm[0].find("source") != std::string::npos) source = parm[1]; 
			else if (parm[0].find("sink") != std::string::npos) sink = parm[1]; 
			else if (parm[0].find("inst") != std::string::npos) instance = atoi(parm[1].c_str()); 
			else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
		}
	}
	else {
		std::vector<std::string> p =  split(std::string(parms), ",");
		if (p.size() < 3) parse_err("Not enough paramters for connect (source,sink,instance)");
		source = p[0];
		sink = p[1];
		instance = atoi(p[3].c_str());
		if (modules[sink]->GetSourceModuleCount() < instance) parse_err("This connect overloads the sink module connect count");
	}
	if (sink == outputname) 
		outputmodule = source;
	else
		modules[sink]->SetSourceModule(instance, *(modules[source]));
	dd.append("\t"+source+" -> "+sink+";\n");
}

void LibNoiseTool::parseOutput(std::string parms)
{
	std::string ddnote="module::Output\n";
	std::vector<std::string> pp =  split(std::string(parms), ";");
	//printvec("paramlist:", pp);  // needed????
	for (std::vector<std::string>::iterator it = pp.begin(); it != pp.end(); ++it) {
		std::vector<std::string> parm =  split(*it, "=");
		if (parm[0] == "module")  outputmodule = parm[1];
		else if (parm[0] == "name")  { outputname = parm[1]; ddnote.append("<BR/>id: "+parm[1]); }
		//else if (parm[0] == "boundsxy") {
		//	std::vector<std::string> bb =  split(std::string(parm[1]), ",");
		//	if (bb.size() < 4) err("Bounds doesn't contain 4 numbers (x1,y1,x2,y2)");
		//	setBounds_XXYY(atof(bb[0].c_str()), atof(bb[1].c_str()), atof(bb[2].c_str()), atof(bb[3].c_str()));
		//}
		//if (parm[0].find("boundswh") != std::string::npos) {
		else if (parm[0] == "builder")  { builder = parm[1]; ddnote.append("builder: "+parm[1]+"<BR/>"); }
		else if (parm[0] == "bounds") {
			/*
			std::vector<std::string> bb =  split(std::string(parm[1]), ",");
			if (bb.size() < 4) err("Bounds doesn't contain 4 numbers (x,y,w,h)");
			setBounds_XYWH(atof(bb[0].c_str()), atof(bb[1].c_str()), atof(bb[2].c_str()), atof(bb[3].c_str()));
			ddnote.append("bounds: "+parm[1]+"<BR/>");
			*/
			float bx, by, bw, bh;
			//std::vector<std::string> bb =  split(std::string(parm[1]), "=");
			//if (bb.size() < 2) err("Malformed bounds (1)");
			//std::cout << "parm[1]:" << parm[1] << std::endl;
			std::vector<std::string> b = split(std::string(parm[1]), ",");
			//std::cout << "b[0]:" << b[0] << "  b[1]:" << b[1] << std::endl;
			if (b.size() < 4) parse_err("Output: Bounds doesn't contain 4 numbers (x,y,w,h)");
			if (b[0].find("rnd") != std::string::npos) {
				std::vector<std::string> br = split(b[0], "-");
				if (br.size() != 3) parse_err("Output: random number specification invalid.");
				bx = random_number(atoi(br[1].c_str()), atoi(br[2].c_str()));
			}
			else {
				bx = atof(b[0].c_str());
			}
			if (b[1].find("rnd") != std::string::npos) {
				std::vector<std::string> br = split(b[1], "-");
				if (br.size() != 3) parse_err("Output: mrandom number specification invalid.");
				by = random_number(atoi(br[1].c_str()), atoi(br[2].c_str()));
			}
			else {
				by = atof(b[1].c_str());
			}
			bw = atof(b[2].c_str());
			bh = atof(b[3].c_str());
			setBounds_XYWH(bx, by, bw, bh);
		}
		else if (parm[0] == "destfile") { destfile = parm[1]; ddnote.append("destfile: "+parm[1]+"<BR/>"); }
		else if (parm[0] == "destsize") {
			std::vector<std::string> dd =  split(std::string(parm[1]), ",");
			if (dd.size() < 2) parse_err("Output: Destsize doesn't contain 2 numbers (w,h)");
			destw = atoi(dd[0].c_str()); desth = atoi(dd[1].c_str());
			ddnote.append("destsize: "+parm[1]+"<BR/>"); 
		}
		else parse_err(string_format("Unrecognized keyword: %s", parm[0].c_str()));
	}
	//if (outputmodule.size() == 0) parse_err("outputmodule parameter not defined");
	if (outputmodule.size() > 0) {
		dd.append("\t"+outputmodule+" -> output\n");
		addDDNode("output", "Output", ddnote);
	}
	else {
		outputmodule = outputname;
		addDDNode(outputname, "Output", ddnote);
	}
	
}

//void LibNoiseTool::parseFile(std::string filename)
bool LibNoiseTool::loadNetwork(std::string filename)
{
	char line[256];
	std::ifstream netfile;
	netfile.open(filename, std::ifstream::in);
	if (!netfile.good()) return false;
	
	dd.append("digraph noisetool {\n\trankdir=LR;\n\tnode [shape=plaintext];\n"); 
	for( std::string line; getline( netfile, line ); ) {
		lineno++;
		
		if (line[line.length()-1] == '\n') line.erase(line.length()-1);  //chomp line ending
		std::vector<std::string> ll =  split(std::string(line), "#");  //segregate comment (#)
		if (ll[0].empty()) continue;
		std::vector<std::string> l =  split(std::string(ll[0]), ":");
		
		//generators
		if (l[0] == "Billow") parseBillow(l[1]);
		else if (l[0] == "Cache") parseCache(l[1]);
		else if (l[0] == "Checkerboard") parseCheckerboard(l[1]);
		else if (l[0] == "Cylinders") parseCylinders(l[1]);
		else if (l[0] == "Perlin") parsePerlin(l[1]);
		else if (l[0] == "RidgedMulti") parseRidgedMulti(l[1]);
		else if (l[0] == "Turbulence") parseTurbulence(l[1]);
		else if (l[0] == "Voronoi") parseVoronoi(l[1]);
		
		//aggregators:
		else if (l[0] == "Add") parseAdd(l[1]);
		else if (l[0] == "Abs") parseAbs(l[1]);
		else if (l[0] == "Blend") parseBlend(l[1]);
		else if (l[0] == "Max") parseMax(l[1]);
		else if (l[0] == "Min") parseMin(l[1]);
		else if (l[0] == "Multiply") parseMultiply(l[1]);
		else if (l[0] == "Power") parsePower(l[1]);
		else if (l[0] == "Select") parseSelect(l[1]);
		
		//modifiers:
		else if (l[0] == "Clamp") parseClamp(l[1]);
		else if (l[0] == "Const") parseConst(l[1]);
		else if (l[0] == "Curve") parseCurve(l[1]);
		else if (l[0] == "Displace") parseDisplace(l[1]);
		else if (l[0] == "Invert") parseInvert(l[1]);
		else if (l[0] == "ScaleBias") parseScaleBias(l[1]);
		else if (l[0] == "ScalePoint") parseScalePoint(l[1]);
		else if (l[0] == "Rotate") parseRotate(l[1]);
		
		//network:
		else if (l[0] == "Connect") parseConnect(l[1]);
		else if (l[0] == "Output") parseOutput(l[1]);
		else parse_err(string_format("Unrecognized keyword: %s",l[0].c_str()));
	}
	dd.append("}\n");
	netfile.close();
	return netparsed = true;
}

bool LibNoiseTool::buildNetwork()
{
	if (!netparsed)  parse_err("No parsed network to build.");
	
	//seed random number generator:
	//srand(static_cast<unsigned int>(time(0)));
	
	if (builder == "cylinder") {
		//printf("cylinder...\n"); fflush(stdout);
		parse_err("Not implemented yet");
	}
	else if (builder == "plane") {
		//printf("plane...\n"); fflush(stdout);
		utils::NoiseMapBuilderPlane heightMapBuilder;
		heightMapBuilder.SetSourceModule (*(modules[outputmodule]));
		heightMapBuilder.SetDestNoiseMap (heightMap);
		//printf("\tdestination size: %d,%d\n", destw, desth); fflush(stdout);
		heightMapBuilder.SetDestSize (destw, desth);
		//printf("\tbounds: %0.2f,%0.2f,%0.2f,%0.2f\n", lowerXbound, upperXbound, lowerYbound, upperYbound); fflush(stdout);
		heightMapBuilder.SetBounds (lowerXbound, upperXbound, lowerYbound, upperYbound);
		heightMapBuilder.Build ();
	}
	else if (builder == "sphere") {
		//printf("sphere...\n"); fflush(stdout);
		parse_err("Not implemented yet");
	}
	else if (builder == "brick") {
		//printf("brick...\n"); fflush(stdout);
		utils::NoiseMapBuilderBrick heightMapBuilder;
		heightMapBuilder.SetSourceModule (*(modules[outputmodule]));
		heightMapBuilder.SetDestNoiseMap (heightMap);
		//printf("\tdestination size: %d,%d\n", destw, desth); fflush(stdout);
		heightMapBuilder.SetDestSize (destw, desth);
		//printf("\tbounds: %0.2f,%0.2f,%0.2f,%0.2f\n", lowerXbound, upperXbound, lowerYbound, upperYbound); fflush(stdout);
		heightMapBuilder.SetBounds (lowerXbound, upperXbound, lowerYbound, upperYbound);
		heightMapBuilder.Build ();
		heightMapBuilder.ApplyPositiveBias ();
	}
	else parse_err("Invalid builder.");
	
	return netbuilt = true;
}

void LibNoiseTool::writeNetwork()
{
	if (!netparsed)  parse_err("No parsed/built network to write.");
	if (!netbuilt) parse_err("Network not built for writing.");
	
	if (destfile.size() == 0) parse_err("No destfile parameter");
	
	std::vector<std::string> f =  split(destfile, ".");
	if (f.size() < 2) parse_err("Destination filename doesn't contain a file extension");
	std::string ext = f[1];
	
	if (ext == "bmp") {  //source: image
		//printf("\trender image...\n"); fflush(stdout);
		utils::RendererImage renderer;
		utils::Image image;
		renderer.SetSourceNoiseMap (heightMap);
		renderer.SetDestImage (image);
		renderer.Render ();
		
		//printf("\twrite BMP image: %s...\n", destfile.c_str()); fflush(stdout);
		utils::WriterBMP writer;
		writer.SetSourceImage (image);
		writer.SetDestFilename (destfile);
		writer.WriteDestFile ();
	}
	else if (ext == "txt") {  //source: image, change to noisemap
		//printf("\twrite TXT height image: %s...\n",destfile.c_str()); fflush(stdout);
		utils::WriterOpenSCADSurface writer;
		writer.SetSourceNoiseMap (heightMap);
		writer.SetDestFilename (destfile);
		writer.WriteDestFile ();
	}
	else if (ext == "ter") {  //source: noisemap
		//printf("\twrite TER heightmap: %s...\n", destfile.c_str()); fflush(stdout);
		utils::WriterTER writer;
		writer.SetSourceNoiseMap (heightMap);
		writer.SetDestFilename (destfile);
		writer.WriteDestFile ();
	}
}

std::vector<std::vector<float>> LibNoiseTool::getHeightMap()
{
	if (!netparsed)  parse_err("No parsed/built network with which to make a heightmap.");
	if (!netbuilt) parse_err("Network not built making a heightmap.");
	
	//printf("\treturning a heightmap vector matrix...\n");
	utils::WriterOpenSCADSurface writer;
	writer.SetSourceNoiseMap (heightMap);
	return writer.getHeightMap();
}

std::vector<std::string> LibNoiseTool::split(std::string s, std::string delim)
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

std::string LibNoiseTool::string_format(const std::string fmt, ...) 
{
    int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
    std::string str;
    va_list ap;
    while (1) {     // Maximum two passes on a POSIX system...
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {  // Everything worked
            str.resize(n);
            return str;
        }
        if (n > -1)  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 2;      // Guess at a larger size (OS specific)
    }
    return str;
}