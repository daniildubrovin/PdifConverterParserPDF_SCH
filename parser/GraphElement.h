#ifndef GRAPHELEMENT_H
#define GRAPHELEMENT_H
#include <string>
#include <vector>
#include <cmath>
#include "PDIFLexer.h"
#include "Layer.h"

class Line; 
class Circle;
class Arc;
class Rect;
class Text;
class Wire;
class Rdl;
class Pnl;
class Sd;
class P;
class At;
class V;
class Nn;
class N;
class COMP_DEF;
class Rd;
class Pn;
class I;

class GraphElement
{
public:
	Layer layer;

	GraphElement() {}
	GraphElement(Layer layer) : layer(layer) {}

	virtual std::string getSvgCode(N* n = nullptr) = 0;
	virtual token_type getType() = 0;
};

class N : public GraphElement
{
public:
	std::string net_name;
	std::vector<GraphElement*> DG;
	std::string Ns;
	std::string Un;

	N() {}
	N(Layer layer) : GraphElement(layer) {}
	N(std::string net_name, std::string Ns, std::string Un) {
		this->net_name = net_name;
		this->Ns = Ns;
		this->Un = Un;
	}

	std::string getSvgCode(N* n = nullptr) { return std::string(); }
	token_type getType() { return token_type::n; }
};

class Line : public GraphElement
{
public:
	std::vector<std::pair<int, int>> coordinates;

	Line() {}
	Line(Layer layer) : GraphElement(layer) {} 

	std::string getSvgCode(N* n = nullptr) {		
		string svg = "<polyline ";
		svg += "class=\"element";
		if (n != nullptr) {
			string net_name = n->net_name;
			for (char& s : net_name) if (s == '\"') s = ' ';
			svg += " " + net_name;
		}
		svg += "\" "; 
		svg += "style=\"stroke-width:" + to_string(layer.Wd + 1) + ";fill:none;\"";
		svg += " points=\" ";
		for (pair<int, int> coord : coordinates) {
			svg += to_string(coord.first) + ',' + to_string(coord.second) + " ";
		}
		svg += "\"/>";
		svg += '\n';
		return svg;
	}

	token_type getType() { return token_type::l; }
};

class Circle : public GraphElement
{
public:
	int x, y, r;
	bool isFilled = false;

	Circle(Layer layer) : GraphElement(layer), x(0), y(0), r(0) {}
	Circle() : x(0), y(0), r(0) {}
	Circle(int x, int y, int r) : x(x), y(y), r(r) {}
	Circle(Layer layer, int x, int y, int r) : GraphElement(layer), x(x), y(y), r(r) {}

	std::string getSvgCode(N* n = nullptr) {
		string svg = "<circle ";	
		svg += "class=\"element";
		if (n != nullptr) {
			string net_name = n->net_name;
			for (char& s : net_name) if (s == '\"') s = ' ';
			svg += " " + net_name;
		}
		svg += "\" ";
		svg += "style=\"stroke-width:" + to_string(layer.Wd + 1) + ";";
		if(isFilled) svg += "fill: " + layer.color + ";";
		else svg += "fill: none;";
		svg += "\"";
		svg += " cx = \"" + to_string(x) + "\" ";
		svg += "cy = \"" + to_string(y) + "\" ";
		svg += "r = \"" + to_string(r) + "\"";
		svg += "/>";
		svg += '\n';
		return svg;
	}
	token_type getType() { return token_type::c; }
};

class Arc : public GraphElement
{
public:
	int x, y, rx, ry, ex, ey;

	Arc() {}
	Arc(Layer layer) : GraphElement(layer) {}

	double get_radius() {
		return sqrt(pow(x - rx, 2) + pow(y - ry, 2));
	}

	double get_dots_angle() {
		int x1 = rx - x, y1 = ry - y;
		int x2 = ex - x, y2 = ey - y;

		int cross_product = x1 * y2 - x2 * y1;
		int dot_product = x1 * x2 + y1 * y2;

		double pi = atan(1) * 4;

		return atan2(cross_product, dot_product) * 180 / pi	;
	}

	std::string getSvgCode(N* n = nullptr) {
		string svg = "<path ";
		svg += "class=\"element";
		if (n != nullptr) {
			string net_name = n->net_name;
			for (char& s : net_name) if (s == '\"') s = ' ';
			svg += " " + net_name;
		}
		svg += "\" ";
		svg += "style=\"stroke-width:" + to_string(layer.Wd + 1) + ";fill:none;\"";
		svg += " d=\"M" + to_string(ex) + " " + to_string(ey);
		svg += " A " + to_string(get_radius()) + " " + to_string(get_radius()) + " 0 ";
		double angle = get_dots_angle();

		if (angle >= 0 && angle <= 180) svg += "1 1";
		else if (angle < 0 && angle >= -180) svg += "0 1";

		svg += to_string(rx) + " " + to_string(ry);
		svg += "\"/>";
		svg += '\n';
		return svg;
	}
	token_type getType() { return token_type::arc; }
};

class Rect : public GraphElement
{
public:
	bool isFilled;
	int x1, x2, y1, y2;

	Rect() {}
	Rect(Layer layer, bool isFilled = false) : GraphElement(layer), isFilled(isFilled) {}
	Rect(int x1, int x2, int y1, int y2): isFilled(false),x1(x1),x2(x2),y1(y1),y2(y2) {}

	std::string getSvgCode(N* n = nullptr) {
		string svg = "<rect ";
		svg += "class=\"element";
		if (n != nullptr) {
			string net_name = n->net_name;
			for (char& s : net_name) if (s == '\"') s = ' ';
			svg += " " + net_name;
		}
		svg += "\" ";
		svg += "style=\"stroke-width:" + to_string(layer.Wd + 1) + ";\"";
		int _x1 = (x1 <= x2) ? x1 : x2;
		int _x2 = (_x1 == x1) ? x2 : x1;
		int _y1 = (y1 <= y2) ? y1 : y2;
		int _y2 = (_y1 == y1) ? y2 : y1;
		svg += " x = \"" + to_string(_x1) + "\" ";
		svg += "y = \"" + to_string(_y1) + "\" ";
		svg += "width = \"" + to_string(abs(_x2 - _x1)) + "\" ";
		svg += "height = \"" + to_string(abs(_y2 - _y1)) + "\"";
		if (isFilled) svg += " fill=\""+layer.color+"\"";
		else svg += " fill=\"none\"";
		svg += "/>";
		svg += '\n';
		return svg;
	}
	token_type getType() { return token_type::r; }
};

class Text : public GraphElement
{
public:
	std::string text;
	int x, y;

	Text() {}
	Text(Layer layer) : GraphElement(layer) { }
	Text(Layer layer, int x, int y, string text) : GraphElement(layer) {
		this->x = x;
		this->y = y;
		this->text = text;	
	}

	double offset_Tj_x() {
		double offset_x = 0;
		if (layer.Tj == "") return offset_x;
		if (layer.Tj[0] == 'C') offset_x = (text.size() / 2.0) * (layer.Ts * 0.65);
		else if (layer.Tj[0] == 'R') offset_x = text.size() * (layer.Ts * 0.65);
		return offset_x;
	}

	double offset_Tj_y() {
		double offset_y = -1;
		if(layer.Tj == "") return offset_y;
		if (layer.Tj[1] == 'C') offset_y =  layer.Ts / 2.0 * 0.8;
		else if (layer.Tj[1] == 'T') offset_y = layer.Ts * 0.8;
		return offset_y;
	}

	void clear_reverse_slash() {
		for (int i = 0; i < text.size() - 1; i++)
		{
			if (text[i] == '\\' && (text[i + 1] >= 33 && text[i + 1] <= 126)) {
				text.erase(text.begin() + i);
				i--;
			}
		}
	}

	std::string getSvgCode(N* n = nullptr) {
		clear_reverse_slash();

		string svg = "<text ";
		string letter_space = to_string(int(layer.Ts/15.0));
		svg += "x = \"" + to_string(x) + "\" ";
		svg += "y = \"" + to_string(y) + "\"";
		svg += " class=\"element";
		if (n != nullptr) {
			string net_name = n->net_name;
			for (char& s : net_name) if (s == '\"') s = ' ';
			svg += " " + net_name;
		}
		svg += "\" ";
		svg += " font-size=\"" + to_string(layer.Ts) + "\"";
		svg += " letter-spacing=\""+ letter_space +"\"";
		svg += " stroke=\"none\"";
		svg += " transform=\"";

		int degrees = 0;
		if (layer.Tr == 1) degrees = -90;
		else if (layer.Tr == 2) degrees = 180;
		else if (layer.Tr == 3) degrees = 90;

		if(degrees != 0) svg += "rotate("+to_string(degrees)+","+ to_string(x)+','+ to_string(y)+")";
		if (layer.Tm == "Y") {
			if (degrees == -90) svg += " scale(-1,1) translate("+ to_string((x) * -2) + ",0)";
			else if (degrees == 90) svg += " scale(-1,1) translate("+ to_string((x) * -2) +",0)";
			else svg += " scale(-1,1) translate("+ to_string(x * -2) + ", 0)";	
		}
		svg += " translate(" + to_string(-offset_Tj_x()) + "," + to_string(offset_Tj_y()) + ")";
		svg += "\"> ";
		svg += text;
		svg += " </text>";
		svg += '\n';
		return svg;
	}

	token_type getType() { return token_type::t; }
};

class Wire : public GraphElement
{
public:
	std::vector<std::pair<int, int>> coordinates;

	Wire() {}
	Wire(Layer layer) : GraphElement(layer) {}

	std::string getSvgCode(N* n = nullptr) {
		string svg = "<polyline ";
		svg += "class=\"element";
		if (n != nullptr) {
			string net_name = n->net_name;
			for (char& s : net_name) if (s == '\"') s = ' ';
			svg += " " + net_name;
		}
		svg += "\" ";
		svg += "style=\"stroke-width:" + to_string(layer.Wd + 1) + ";fill:none;\"";
		svg += " points=\" ";
		for (pair<int, int> coord : coordinates) {
			svg += to_string(coord.first) + ',' + to_string(coord.second) + " ";
		}
		svg += "\"/>";
		svg += '\n';
		return svg;
	}
	token_type getType() { return token_type::w; }
};

class Rdl : public GraphElement {
public:
	int x, y, r = 4;

	Rdl() {}
	Rdl(Layer layer) : GraphElement(layer) {}

	std::string getSvgCode(N* n = nullptr) {
		//string svg = "<circle ";
		//svg += "style=\"stroke-width:" + to_string(layer.Wd + 1) + ";";
		//svg += "fill: deeppink;";
		//svg += "stroke: deeppink;";
		//svg += "\"";
		//svg += " fill-opacity=\"0.65\"";
		//svg += " stroke-opacity=\"0.65\"";
		//svg += " cx = \"" + to_string(x) + "\" ";
		//svg += "cy = \"" + to_string(y) + "\" ";
		//svg += "r = \"" + to_string(r) + "\"";
		//svg += "/>";
		//svg += '\n';
		//return svg;
		return ""s;
	}
	token_type getType() { return token_type::rdl; }
};

class Pnl : public GraphElement {
public:
	int x, y, r = 4;

	Pnl() {}
	Pnl(Layer layer) : GraphElement(layer) {}

	std::string getSvgCode(N* n = nullptr) {
		/*string svg = "<circle ";
		svg += "style=\"stroke-width:" + to_string(layer.Wd + 1) + ";";
		svg += "fill: midnightblue;";
		svg += "stroke: midnightblue;";
		svg += "\"";
		svg += " fill-opacity=\"0.65\"";
		svg += " stroke-opacity=\"0.65\"";
		svg += " cx = \"" + to_string(x) + "\" ";
		svg += "cy = \"" + to_string(y) + "\" ";
		svg += "r = \"" + to_string(r) + "\"";
		svg += "/>";
		svg += '\n';
		return svg;*/
		return ""s;
	}
	token_type getType() { return token_type::pnl; }
};

class Sd : public GraphElement {
public:
	std::string section;
	std::vector<string> pin_numbers;

	Sd() {}
	Sd(Layer layer) : GraphElement(layer) {}

	std::string getSvgCode(N* n = nullptr) {
		return ""s;
	}
	token_type getType() { return token_type::sd; }
};

class P : public GraphElement {
public:
	std::string pin_name;
	std::string type;
	int lq;
	int x, y;
	int r = 4;
	bool inCOMP_DEF = false;
	int length_line_cross = 5;
	P() {}
	P(Layer layer) : GraphElement(layer) {}
	P(std::string pin_name, std::string type, int lq, int x, int y, Layer layer): GraphElement(layer){
		this->pin_name = pin_name;
		this->type = type;
		this->lq = lq;
		this->x = x;
		this->y = y;
	}

	std::string getSvgCode(N* n = nullptr) {
		string svg;
		if (inCOMP_DEF) svg = "<path d=\"M" + to_string(x - length_line_cross) + " " + to_string(y - length_line_cross) + "," + to_string(x + length_line_cross) + " " + to_string(y + length_line_cross) + "\"/>" +
				"<path d=\"M" + to_string(x - length_line_cross) + " " + to_string(y + length_line_cross) + "," + to_string(x + length_line_cross) + " " + to_string(y - length_line_cross) + "\"/>\n";
		else {
			svg = "<circle ";
			svg += "style=\"stroke-width:" + to_string(layer.Wd + 1) + ";";
			//svg += "fill: " + layer.color + ";";
			svg += "\"";
			svg += " fill-opacity=\"0.65\"";
			svg += " stroke-opacity=\"0.65\"";
			svg += " cx = \"" + to_string(x) + "\" ";
			svg += "cy = \"" + to_string(y) + "\" ";
			svg += "r = \"" + to_string(r) + "\"";
			svg += "/>";
			svg += "\n";
		}
		return svg;
	}
	token_type getType() { return token_type::p; }
};

class At : public GraphElement {
public:
	std::string key;
	std::string value;
	int x, y, r = 4;

	At() {}
	At(Layer layer) : GraphElement(layer) {}

	std::string getSvgCode(N* n = nullptr) {
		/*string svg = "<circle ";
		svg += "style=\"stroke-width:" + to_string(layer.Wd + 1) + ";";
		svg += "fill: indigo;";
		svg += "stroke: indigo;";
		svg += "\"";
		svg += " fill-opacity=\"0.65\"";
		svg += " stroke-opacity=\"0.65\"";
		svg += " cx = \"" + to_string(x) + "\" ";
		svg += "cy = \"" + to_string(y) + "\" ";
		svg += "r = \"" + to_string(r) + "\"";
		svg += "/>";
		svg += '\n';
		return svg;*/
		return ""s;
	}
	token_type getType() { return token_type::at; }
};

class V : public GraphElement {
public:
	int x, y;
	int via_type;
	int r = 4;

	V() {}
	V(Layer layer) : GraphElement(layer) {
		this->layer.Ly = "SDOT";
	}

	std::string getSvgCode(N* n = nullptr) {
		string svg = "<circle ";
		svg += "style=\"stroke-width:" + to_string(layer.Wd + 1) + ";";
		svg += "fill: lime;";
		svg += "stroke: none;";
		svg += "\"";
		svg += " fill-opacity=\"0.65\"";
		svg += " stroke-opacity=\"0.65\"";
		svg += " cx = \"" + to_string(x) + "\" ";
		svg += "cy = \"" + to_string(y) + "\" ";
		svg += "r = \"" + to_string(r) + "\"";
		svg += "/>";
		svg += '\n';
		return svg;
	}
	token_type getType() { return token_type::v; }
};

class Nn : public GraphElement
{
public:
	std::vector<std::pair<int, int>> coordinates;

	Nn() {}
	Nn(Layer layer) : GraphElement(layer) {}

	std::string getSvgCode(N* n = nullptr) { return std::string(); }
	token_type getType() { return token_type::nn; }
};

class COMP_DEF : public GraphElement
{
public:
	std::string file_name;
	vector<P*> PIN_DEF;
	Rdl* rdl;
	std::string pid;
	std::vector<Pnl*> locations;
	std::vector<Sd*> sections;
	std::vector<GraphElement*> pic;
	struct ORG {
		int x, y;
	} org;
	int Ty;
	int Vr;
	vector<string> layers;

	COMP_DEF() {}

	std::string getSvgCode(N* n = nullptr) { return file_name + "\n"; }
	token_type getType() { return token_type::comp_def; }
};

class Rd : public GraphElement {
public:
	std::string rd;
	int x, y;

	Rd() {}
	Rd(Layer layer) : GraphElement(layer) {}

	std::string getSvgCode(N* n = nullptr) {
		Text text(layer,x,y,rd);
		return text.getSvgCode(n);
	}
	token_type getType() { return token_type::rd; }
};

class Pn : public GraphElement {
public:
	std::string n;
	int x, y;

	Pn() {}
	Pn(Layer layer) : GraphElement(layer) {}

	std::string getSvgCode(N* _n = nullptr) {
		Text text(layer, x, y, n);
		return text.getSvgCode(_n);
	}
	token_type getType() { return token_type::pn; }
};

class I : public GraphElement
{
public:
	COMP_DEF* comp_def;
	std::string inv_name, file_name;
	std::vector<N*> CN;
	std::vector<P*> comp_pin_def;

	std::string asg_name;
	std::vector<GraphElement*> asg;

	struct Pl { int16_t x, y; } Pl;
	struct Sc { int sx, sy; } Sc;
	int Ro = 0;
	std::string Mr;
	struct Nl { int x, y; } Nl;


	std::vector<At*> ex;

	std::string getSvgCode(N* n = nullptr) {
		return "";
	}

	token_type getType() { return token_type::i; }
};
#endif