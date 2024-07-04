#ifndef LAYER_H
#define LAYER_H
#include <string>

/**
 * @brief Ыtores general information about the layer
 * 
 */
class Layer
{
public:
	std::string Ly; ///< title
	std::string Ls; ///< style
	int Wd; ///< width
	int Ts, Tr; ///< text style
	std::string Tj, Tm;  ///< text style
	std::string color; 

	Layer();
	Layer(std::string Ly, std::string Ls, int Wd, int Ts, int Tr, std::string Tj,std::string Tm);
	Layer(std::string Ly, std::string Ls, int Wd);
};
#endif