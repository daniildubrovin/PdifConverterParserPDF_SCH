#ifndef PDIFTYPE_H
#define PDIFTYPE_H

#include <string>
#include <vector>
#include <cmath>
#include "GraphElement.h"


	
using namespace std; 

 class PDIFType
{
private:
	/**
	 * @brief Multiplies the y coordinate by -1 for graphic element
	 * @param element graphic element
	 */
	void negativeY(GraphElement* element);

	/**
	 * @brief Negative the y coordinate for all graphic elements
	 * 
	 */
	void convert_coords();

	/**
	 * @brief Set the begin coordinate for graphic element
	 * @param element graphic element
	 * @return true 
	 * @return false 
	 */
	bool set_begin_coordinate(GraphElement* element);

	/**
	 * @brief Set the begin coordinates
	 */
	void search_begin_coordinates();

	/**
	 * @brief Set the border coordinate for graphic element
	 * @param element graphic element
	 * @param i 
	 */
	void set_border_coordinate(GraphElement* element, I* i = nullptr);

	/**
	 * @brief Set the border coordinates
	 */
	void set_border_coordinates();	

	/**
	 * @brief Point rotate transformation 
	 * 
	 * @param x 
	 * @param y 
	 * @param cx 
	 * @param cy 
	 * @param angle 
	 */
	void rotatePoint(int& x, int& y, int cx, int cy, int angle);

	/**
	 * @brief Figure rotate transformation 
	 * 
	 * @param element 
	 * @param cx 
	 * @param cy 
	 * @param angle 
	 * @return GraphElement* 
	 */
	GraphElement* rotateFigure(GraphElement* element, int cx, int cy, int angle);

	/**
	 * @brief Insert a graphic element into layer
	 * 
	 * @param layer_groups 
	 * @param dotted 
	 * @param dashed 
	 * @param element 
	 * @param n 
	 */
	void insert_into_layer(vector<string>& layer_groups,vector<string>& dotted,vector<string>& dashed, GraphElement* element,N* n = nullptr);

	/**
	 * @brief Generate COMP_DEF patterns
	 * 
	 * @return string 
	 */
	string generate_comp_def_patterns();

	/**
	 * @brief Insert I 
	 * 
	 * @param layer_groups 
	 * @param dotted 
	 * @param dashed 
	 * @param instance 
	 */
	void insert_instance(vector<string>& layer_groups, vector<string>& dotted, vector<string>& dashed, I* instance);

	/**
	 * @brief Get the svg settings
	 * 
	 * @return string svg settings code
	 */
	string get_svg_setting();

	/**
	 * @brief Check layer hiding
	 * 
	 * @param ly layer name
	 * @return true 
	 * @return false 
	 */
	bool isHidden(string ly);

	/**
	 * @brief Return a js script
	 * 
	 * @return string 
	 */
	string js_script();

public:
	int WIDTH;
	int HEIGHT;
	double SCALE = 2.0;
	int minX, minY;
	int maxX, maxY;
	string outputfile;
	int borderOffset = 10;

	string component; ///< COMPONENT
	
	/**
	 * @brief ENVIRONMENT
	 */
	struct ENVIRONMENT {
		float PDIFvrev;
		string Program;
		string DBtype;
		float DBvrev;
		string DBtime;
		string DBunit;
		int DBgrid;
		vector<pair<string, string>> Lyrstr;
	} ENVIRONMENT;

	/**
	 * @brief USER
	 */
	struct USER {
		string mode;
		struct Gs { int x, y; } Gs;
		vector<int> Lv;
	} USER;

	/**
	 * @brief SYMBOL
	 * 
	 */
	struct SYMBOL {
		vector<P*> PIN_DEF;
	//PKG
		Rdl* rdl;
		std::string pid;
		std::vector<Pnl*> locations;
		std::vector<Sd*> sections;
	//PIC
		std::vector<GraphElement*> pic;
	//ATR
		//IN
		struct ORG {
			int x, y;
		} org;
		int ty;
		//EX
		vector<At*> ex;
	} SYMBOL;

	/**
	 * @brief ANNOTATE
	 * 
	 */
	struct DETAIL {
		std::vector<GraphElement*> Annotate;
		std::vector<N*> NET_DEF;
		std::vector<COMP_DEF*> COMP_DEFS;
		std::vector<I*> instances;
	} DETAIL;
	

	 PDIFType();

	/**
	 * @brief Generates an svg file
	 * 
	 * @param output_file name of generated svg file
	 * @return string svg document
	 */
	 string toSVG(string output_file, double scale = 2.0);
	 ~PDIFType();
};
#endif