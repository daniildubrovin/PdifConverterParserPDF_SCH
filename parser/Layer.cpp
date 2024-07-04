#include "Layer.h"

Layer::Layer()
{
}

Layer::Layer(std::string Ly, std::string Ls, int Wd, int Ts, int Tr, std::string Tj, std::string Tm)
{ 
	this->Ly = Ly;
	this->Ls = Ls;
	this->Wd = Wd;
	this->Ts = Ts;
	this->Tr = Tr;
	this->Tj = Tj;
	this->Tm = Tm;
}

Layer::Layer(std::string Ly, std::string Ls, int Wd)
{
	this->Ly = Ly;
	this->Ls = Ls;
	this->Wd = Wd;
	this->Ts = -1;
	this->Tr = -1;
	this->Tj = "";
	this->Tm = "";
}
