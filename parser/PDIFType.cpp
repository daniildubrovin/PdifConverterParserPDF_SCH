#include "PDIFType.h"

void PDIFType::negativeY(GraphElement* element)
{
    if (element->getType() == token_type::t) ((Text*)element)->y *= -1;
    else if(element->getType() == token_type::c) ((Circle*)element)->y *= -1;
    else if (element->getType() == token_type::r || element->getType() == token_type::fr) {
        ((Rect*)element)->y1 *= -1;
        ((Rect*)element)->y2 *= -1;
    }
    else if (element->getType() == token_type::l) {
        for (pair<int, int>& coord : ((Line*)element)->coordinates) coord.second *= -1;
    }
    else if (element->getType() == token_type::w) {
        for (pair<int, int>& coord : ((Wire*)element)->coordinates) coord.second *= -1;
    }
    else if (element->getType() == token_type::v) {
        ((V*)element)->y *= -1;
    }
    /*else if (element->getType() == token_type::at) {
        ((At*)element)->y *= -1;
    }
    else if (element->getType() == token_type::pnl) {
        ((Pnl*)element)->y *= -1;
    }
    else if (element->getType() == token_type::rdl) {
        ((Rdl*)element)->y *= -1;
    }*/
    else if (element->getType() == token_type::arc) {
        ((Arc*)element)->y *= -1;
        ((Arc*)element)->ry *= -1;
        ((Arc*)element)->ey *= -1;
    }
    else if (element->getType() == token_type::p) ((P*)element)->y *= -1;
    else if (element->getType() == token_type::rd) ((Rd*)element)->y *= -1;
    else if (element->getType() == token_type::pn) ((Pn*)element)->y *= -1;
    else if (element->getType() == token_type::nn) {
        for (pair<int, int>& coord : ((Nn*)element)->coordinates) coord.second *= -1;
    }
}

void PDIFType::convert_coords()
{
    for (GraphElement* element : SYMBOL.PIN_DEF) negativeY(element);
    for (GraphElement* element : SYMBOL.pic) negativeY(element);
    for (GraphElement* element : DETAIL.Annotate) negativeY(element);
    for (N* n : DETAIL.NET_DEF) {
        for (GraphElement* element : n->DG) negativeY(element);
    }
    for (COMP_DEF* comp_def : DETAIL.COMP_DEFS) {
         for(GraphElement* element : comp_def->PIN_DEF) negativeY(element);
         for(GraphElement* element : comp_def->locations) negativeY(element);
         for(GraphElement* element : comp_def->pic) negativeY(element);
         //negativeY(comp_def->rdl);
    }
    for (I* i : DETAIL.instances) {
        i->Pl.y *= -1;
       // for (GraphElement* element : i->asg) negativeY(element);
    }
}

bool PDIFType::set_begin_coordinate(GraphElement* element)
{
    if (element->getType() == token_type::t) {
        minY = maxY = ((Text*)element)->y;
        minX = maxX = ((Text*)element)->x;
        return true;
    }
    else if (element->getType() == token_type::c) {
        minY = maxY = ((Circle*)element)->y;
        minX = maxX = ((Circle*)element)->x;
        return true;
    }
    else if (element->getType() == token_type::r || element->getType() == token_type::fr) {
        minY = maxY = ((Rect*)element)->y1;
        minX = maxX = ((Rect*)element)->x1;
        return true;
    }
    else if (element->getType() == token_type::l) {
        for (pair<int, int>& coord : ((Line*)element)->coordinates) {
            minY = maxY = coord.second;
            minX = maxX = coord.first;
            return true;
        }
    }
    else if (element->getType() == token_type::w) {
        for (pair<int, int>& coord : ((Wire*)element)->coordinates) {
            minY = maxY = coord.second;
            minX = maxX = coord.first;
            return true;
        }
    }
    else if (element->getType() == token_type::arc) {
        minY = maxY = ((Arc*)element)->y;
        minX = maxX = ((Arc*)element)->x;
        return true;
    }
    else if (element->getType() == token_type::p) {
        minY = maxY = ((P*)element)->y;
        minX = maxX = ((P*)element)->x;
        return true;
    }
    else if (element->getType() == token_type::rd) {
        minY = maxY = ((Rd*)element)->y;
        minX = maxX = ((Rd*)element)->x;
        return true;
    }
    else if (element->getType() == token_type::pn) {
        minY = maxY = ((Pn*)element)->y;
        minX = maxX = ((Pn*)element)->x;
        return true;
    }
    else if (element->getType() == token_type::nn) {
        for (pair<int, int>& coord : ((Nn*)element)->coordinates) {
            minY = maxY = coord.second;
            minX = maxX = coord.first;
            return true;
        }
    }
    else if (element->getType() == token_type::v) {
        minY = maxY = ((V*)element)->y;
        minX = maxX = ((V*)element)->x;
        return true;
    }
    /*else if (element->getType() == token_type::at) {
        minY = maxY = ((At*)element)->y;
        minX = maxX = ((At*)element)->x;
    }
    else if (element->getType() == token_type::pnl) {
        minY = maxY = ((Pnl*)element)->y;
        minX = maxX = ((Pnl*)element)->x;
    }
    else if (element->getType() == token_type::rdl) {
        minY = maxY = ((Rdl*)element)->y;
        minX = maxX = ((Rdl*)element)->x;
    }*/

    return false;
}

void PDIFType::search_begin_coordinates()
{ 
    for (GraphElement* element : SYMBOL.PIN_DEF) {
        if (set_begin_coordinate(element))  return;
    }
    for (GraphElement* element : SYMBOL.pic) {
        if (set_begin_coordinate(element))  return;
    }
    for (GraphElement* element : DETAIL.Annotate) {
        if (set_begin_coordinate(element))  return;
    }
    for (N* n : DETAIL.NET_DEF) {
        for (GraphElement* element : n->DG) {
            if (set_begin_coordinate(element))  return;
        }
    }
    for (I* i : DETAIL.instances) {
        for (GraphElement* element : i->comp_def->PIN_DEF) {
            if (set_begin_coordinate(element))  return;
        }
        for (GraphElement* element : i->comp_def->pic) {
            if (set_begin_coordinate(element))  return;
        }
        for (GraphElement* element : i->asg) {
            if (set_begin_coordinate(element))  return;
        }
    }
    throw runtime_error("graphic elements were not found, there is nothing to convert");
}

void PDIFType::set_border_coordinates()
{
    for (GraphElement* element : SYMBOL.PIN_DEF) set_border_coordinate(element);
    for (GraphElement* element : SYMBOL.pic) set_border_coordinate(element);
    for (GraphElement* element : DETAIL.Annotate) set_border_coordinate(element);
    for (N* n : DETAIL.NET_DEF) {
        for (GraphElement* element : n->DG) set_border_coordinate(element);
    }
    for (I* i : DETAIL.instances) {
        for (GraphElement* element : i->comp_def->PIN_DEF) set_border_coordinate(element,i);
        for (GraphElement* element : i->comp_def->pic) set_border_coordinate(element,i);
        for (GraphElement* element : i->asg) set_border_coordinate(element,i);
    }
}

void PDIFType::set_border_coordinate(GraphElement* element, I* i)
{
    int i_x = 0, i_y = 0, degrees = 0;
    GraphElement* t_element = nullptr;
    if (i != nullptr) {
        i_x = i->Pl.x;
        i_y = i->Pl.y;
        if (i->Ro == 1) degrees = -90;
        else if (i->Ro == 2) degrees = 180;
        else if (i->Ro == 3) degrees = 90;
    }

    if (degrees != 0) {
        t_element = rotateFigure(element, i_x, i_y, degrees);
        i_x = 0;
        i_y = 0;
    }
    else t_element = element;
  
    if (element->getType() == token_type::c) {  
        int x = ((Circle*)t_element)->x + i_x, y = ((Circle*)t_element)->y + i_y, r = ((Circle*)t_element)->r;

        if (x - r < minX) minX = x - r;
        if (x + r > maxX) maxX = x + r;
        if (y - r < minY) minY = y - r;
        if (y + r > maxY) maxY = y + r;
    }
    else if (element->getType() == token_type::p) {
        int x = ((P*)t_element)->x + i_x, y = ((P*)t_element)->y + i_y, r = ((P*)t_element)->r;

        if (x - r < minX) minX = x - r;
        if (x + r > maxX) maxX = x + r;
        if (y - r < minY) minY = y - r;
        if (y + r > maxY) maxY = y + r;
    }
    else if (element->getType() == token_type::v) {
        int x = ((V*)t_element)->x + i_x, y = ((V*)t_element)->y + i_y, r = ((V*)t_element)->r;

        if (x - r < minX) minX = x - r;
        if (x + r > maxX) maxX = x + r;
        if (y - r < minY) minY = y - r;
        if (y + r > maxY) maxY = y + r;
    }
    /*else if (element->getType() == token_type::at) {
        int x = ((At*)t_element)->x + i_x, y = ((At*)t_element)->y + i_y, r = ((At*)t_element)->r;

        if (x - r < minX) minX = x - r;
        if (x + r > maxX) maxX = x + r;
        if (y - r < minY) minY = y - r;
        if (y + r > maxY) maxY = y + r;
    }
    else if (element->getType() == token_type::pnl) {
        int x = ((Pnl*)t_element)->x + i_x, y = ((Pnl*)t_element)->y + i_y, r = ((Pnl*)t_element)->r;

        if (x - r < minX) minX = x - r;
        if (x + r > maxX) maxX = x + r;
        if (y - r < minY) minY = y - r;
        if (y + r > maxY) maxY = y + r;
    }
    else if (element->getType() == token_type::rdl) {
        int x = ((Rdl*)t_element)->x + i_x, y = ((Rdl*)t_element)->y + i_y, r = ((Rdl*)t_element)->r;

        if (x - r < minX) minX = x - r;
        if (x + r > maxX) maxX = x + r;
        if (y - r < minY) minY = y - r;
        if (y + r > maxY) maxY = y + r;
    }*/
    else if (element->getType() == token_type::arc) {
        int x = ((Arc*)t_element)->x + i_x, y = ((Arc*)t_element)->y + i_y, r = ((Arc*)t_element)->get_radius();

        if (x - r < minX) minX = x - r;
        if (x + r > maxX) maxX = x + r;
        if (y - r < minY) minY = y - r;
        if (y + r > maxY) maxY = y + r;
    }
    else if (element->getType() == token_type::t) {
        Text* text = ((Text*)t_element);
        int x = text->x - text->offset_Tj_x() + i_x, y = text->y + text->offset_Tj_y() + i_y;

        if (x < minX) minX = x;
        if (x + text->offset_Tj_x() > maxX) maxX = x + text->offset_Tj_x();
        if (y - text->offset_Tj_y() < minY) minY = y - text->offset_Tj_y();
        if (y > maxY) maxY = y;
    }
    else if (element->getType() == token_type::l) {
        for (pair<int, int> coord : ((Line*)t_element)->coordinates) {
            if (coord.first + i_x < minX) minX = coord.first + i_x;
            if (coord.first + i_x > maxX) maxX = coord.first + i_x;
            if (coord.second + i_y < minY) minY = coord.second + i_y;
            if (coord.second + i_y > maxY) maxY = coord.second + i_y;
        }
    }
    else if (element->getType() == token_type::w) {
        for (pair<int, int> coord : ((Wire*)t_element)->coordinates) {
            if (coord.first + i_x < minX) minX = coord.first + i_x;
            if (coord.first + i_x > maxX) maxX = coord.first + i_x;
            if (coord.second + i_y < minY) minY = coord.second + i_y;
            if (coord.second + i_y > maxY) maxY = coord.second + i_y;
        }
    }
    else if (element->getType() == token_type::r || element->getType() == token_type::fr) {
        int x1 = ((Rect*)t_element)->x1 + i_x, y1 = ((Rect*)t_element)->y1 + i_y;
        int x2 = ((Rect*)t_element)->x2 + i_x, y2 = ((Rect*)t_element)->y2 + i_y;

        if (x1 < minX) minX = x1;
        if (x1 > maxX) maxX = x1;
        if (x2 < minX) minX = x2;
        if (x2 > maxX) maxX = x2;

        if (y1 < minY) minY = y1;
        if (y1 > maxY) maxY = y1;
        if (y2 < minY) minY = y2;
        if (y2 > maxY) maxY = y2;
    }

    if(degrees != 0) delete t_element;
}

void PDIFType::rotatePoint(int& x, int& y, int cx, int cy, int angle)
{
    double s = sin(angle);
    double c = cos(angle);

    // rotate point
    double xnew = x * c - y * s;
    double ynew = x * s + y * c;

    // translate point back:
    x = xnew + cx;
    y = ynew + cy;
}

GraphElement* PDIFType::rotateFigure(GraphElement* element, int cx, int cy, int angle)
{
    GraphElement* t_element = nullptr;

    if (element->getType() == token_type::c) {
        t_element = new Circle(*(Circle*)element);
        rotatePoint(((Circle*)t_element)->x, ((Circle*)t_element)->y,cx,cy,angle);
    }
    else if (element->getType() == token_type::arc) {
        t_element = new Arc(*(Arc*)element);
        rotatePoint(((Arc*)t_element)->x, ((Arc*)t_element)->y, cx, cy, angle);
        rotatePoint(((Arc*)t_element)->rx, ((Arc*)t_element)->ry, cx, cy, angle);
        rotatePoint(((Arc*)t_element)->ex, ((Arc*)t_element)->ey, cx, cy, angle);
    }
    else if (element->getType() == token_type::t) {
        t_element = new Text(*(Text*)element);
        if (angle == -90) t_element->layer.Tr = 1;
        else if (angle == 180) t_element->layer.Tr = 2;
        else if (angle == 90) t_element->layer.Tr = 3;
        rotatePoint(((Text*)t_element)->x, ((Text*)t_element)->y, cx, cy, angle);
    }
    else if (element->getType() == token_type::p) {
        t_element = new P(*(P*)element);
        rotatePoint(((P*)t_element)->x, ((P*)t_element)->y, cx, cy, angle);
    }
    else if (element->getType() == token_type::v) {
        t_element = new V(*(V*)element);
        rotatePoint(((V*)t_element)->x, ((V*)t_element)->y, cx, cy, angle);
    }
    /*else if (element->getType() == token_type::at) {
        t_element = new At(*(At*)element);
        rotatePoint(((At*)t_element)->x, ((At*)t_element)->y, cx, cy, angle);
    }
    else if (element->getType() == token_type::rdl) {
        t_element = new Rdl(*(Rdl*)element);
        rotatePoint(((Rdl*)t_element)->x, ((Rdl*)t_element)->y, cx, cy, angle);
    }
    else if (element->getType() == token_type::pnl) {
        t_element = new Pnl(*(Pnl*)element);
        rotatePoint(((Pnl*)t_element)->x, ((Pnl*)t_element)->y, cx, cy, angle);
    }*/
    else if (element->getType() == token_type::l) {
        t_element = new Line(*(Line*)element);
        for (pair<int, int>& coord : ((Line*)t_element)->coordinates) {
            rotatePoint(coord.first, coord.second, cx, cy, angle);
        }
    }
    else if (element->getType() == token_type::w) {
        t_element = new Wire(*(Wire*)element);
        for (pair<int, int>& coord : ((Wire*)t_element)->coordinates) {
            rotatePoint(coord.first, coord.second, cx, cy, angle);
        }
    }
    else if (element->getType() == token_type::r || element->getType() == token_type::fr) {
        t_element = new Rect(*(Rect*)element);
        rotatePoint(((Rect*)t_element)->x1, ((Rect*)t_element)->y1, cx, cy, angle);
        rotatePoint(((Rect*)t_element)->x2, ((Rect*)t_element)->y2, cx, cy, angle);
    }

    return t_element;
}

void PDIFType::insert_into_layer(vector<string>& layer_groups, vector<string>& dotted, vector<string>& dashed, GraphElement* element, N* n)
{ 
    for (int i = 0; i < ENVIRONMENT.Lyrstr.size(); i++)
    {
        if (element->layer.Ly == ENVIRONMENT.Lyrstr[i].first) {
            if (n != nullptr && element->getType() == token_type::nn) {
                for (pair<int, int> coord : ((Nn*)element)->coordinates) {
                    Text text(element->layer, coord.first, coord.second, n->net_name);              
                    layer_groups[i] += text.getSvgCode(n);
                }
            }
            else if (element->getType() == token_type::t || element->getType() == token_type::p || element->layer.Ls == "SOLID") layer_groups[i] += element->getSvgCode(n);
            else if (element->layer.Ls == "DOTTED") dotted[i] += element->getSvgCode(n);
            else if (element->layer.Ls == "DASHED") dashed[i] += element->getSvgCode(n);
            break;
        }         
    }
}

string PDIFType::generate_comp_def_patterns()
{
    string defs = "<defs>\n";

    for (COMP_DEF* comp_def : DETAIL.COMP_DEFS) {
        string comp_def_svg;
        vector<string> layer_groups, dotted, dashed;

        for (pair<string, string> l : ENVIRONMENT.Lyrstr) {
            layer_groups.push_back("<g id=\"" + comp_def->file_name + "_" + l.first + "_SOLID\">\n");
            dotted.push_back("<g id=\"" + comp_def->file_name + "_" + l.first + "_DOTTED\">\n");
            dashed.push_back("<g id=\"" + comp_def->file_name + "_" + l.first + "_DASHED\">\n");
        }

       /* for (P* element : comp_def->PIN_DEF) {
            element->inCOMP_DEF = true;
            insert_into_layer(layer_groups, dotted, dashed, element);
        }*/
        //for (Pnl* element : comp_def->locations) insert_into_layer(layer_groups, dotted, dashed, element);
        for (GraphElement* element : comp_def->pic) insert_into_layer(layer_groups,dotted,dashed,element);
        //insert_into_layer(layer_groups, dotted, dashed, comp_def->rdl);

        for (int i = 0; i < layer_groups.size(); i++) {
            layer_groups[i] += "</g>\n";
            int empty_length = 18 + comp_def->file_name.size() + ENVIRONMENT.Lyrstr[i].first.size();
            if (dashed[i].size() != empty_length) {
                layer_groups[i] += dashed[i] + "</g>\n";
                comp_def->layers.push_back(ENVIRONMENT.Lyrstr[i].first+"_DASHED");
            }
            if (dotted[i].size() != empty_length) {
                layer_groups[i] += dotted[i] + "</g>\n";
                comp_def->layers.push_back(ENVIRONMENT.Lyrstr[i].first + "_DOTTED");
            }
            if (layer_groups[i].size() != empty_length + 4) {
                comp_def_svg += layer_groups[i];
                comp_def->layers.push_back(ENVIRONMENT.Lyrstr[i].first);
            }
        }
        defs += comp_def_svg;
    }

    defs += "</defs>\n";
    return defs;
}

void PDIFType::insert_instance(vector<string>& layer_groups, vector<string>& dotted, vector<string>& dashed, I* instance)
{
    int degrees = instance->Ro == 1 ? -90 : instance->Ro == 2 ? 180 : instance->Ro == 3 ? 90 : 0;
   
    for (string comp_def_layer : instance->comp_def->layers) {
        for (int i = 0; i < ENVIRONMENT.Lyrstr.size(); i++)
        {
            string use, inst_inform, ls = ENVIRONMENT.Lyrstr[i].first;

            if (comp_def_layer == ls || comp_def_layer == ls + "_DASHED" || comp_def_layer == ls + "_DOTTED") {
                use = "<use class=\"element\" xlink:href=\"#" + instance->comp_def->file_name + "_" + ls + "_";
                inst_inform = " x=\"" + to_string(instance->Pl.x) + "\" y=\"" + to_string(instance->Pl.y) + "\"";
                if (degrees != 0) inst_inform += " transform=\"rotate(" + to_string(degrees) + "," + to_string(instance->Pl.x) + ',' + to_string(instance->Pl.y) + ")\"";
                inst_inform += "/>\n";           
            }

            if (comp_def_layer == ls) layer_groups[i] += use + "SOLID\"" + inst_inform;
            else if (comp_def_layer == ls + "_DASHED") dashed[i] += use + "DASHED\"" + inst_inform;
            else if (comp_def_layer == ls + "_DOTTED") dotted[i] += use + "DOTTED\"" + inst_inform;           
        }
    }

    for (P* p : instance->comp_pin_def) {
        p->inCOMP_DEF = true;

        for (int i = 0; i < ENVIRONMENT.Lyrstr.size(); i++)
        {
            if (p->layer.Ly == ENVIRONMENT.Lyrstr[i].first) {
                string svg = "<g class=\"element \" ";
                svg += " transform=\"";
                if(degrees != 0) svg += "rotate(" + to_string(degrees) + "," + to_string(instance->Pl.x) + ',' + to_string(instance->Pl.y) + ") ";
                svg += "translate(" + to_string(instance->Pl.x) + ',' + to_string(instance->Pl.y) + ")\"";
                svg += ">\n";
                svg += p->getSvgCode(nullptr);
                svg += "\n</g>\n";
                layer_groups[i] += svg;
            }
        }
    }
}

string PDIFType::get_svg_setting()
{
    WIDTH = abs((int)((maxX - minX + borderOffset*2) * SCALE));
    HEIGHT = abs((int)((maxY - minY + borderOffset*2) * SCALE));
    string scale_width = to_string(int(WIDTH * 1.0 / SCALE));
    string scale_height = to_string(int(HEIGHT * 1.0 / SCALE));
    string width = to_string(WIDTH);
    string height = to_string(HEIGHT);

    return "<svg width=\"" + width +"\" height=\"" + height +
        "\" viewBox=\"" + to_string(minX - borderOffset) + " " + to_string(minY - borderOffset) + " " + scale_width + " " + scale_height + "\"" +
        " preserveAspectRatio=\"xMinYMin meet\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n"; 
}

bool PDIFType::isHidden(string ly)
{
    for (int i = 0; i < ENVIRONMENT.Lyrstr.size()-1; i++)
    {
        if (ENVIRONMENT.Lyrstr[i].first == ly && USER.Lv[i+1] == 0) {
            return true;
        }
    }
    return false;
}

string PDIFType::js_script()
{
    return "<script>\n\
        <![CDATA[\n\
        var elements = document.getElementsByClassName(\"element\");\n\
        for (let element of elements) {\n\
            element.propertyStroke = element.style.stroke;\n\
            element.propertyWidth = parseInt(element.style[\"stroke-width\"].substring(0, element.style[\"stroke-width\"].length - 2), 10);\n\
            element.onmouseenter = function() {\n\
                if(!element.isClick){\n\
                    var classList = element.classList;\n\
                    if(classList.length > 1){\n\
                        for (let n_el of elements){\n\
                            if(n_el.classList[1] == classList[1] && n_el !== element){\n\
                                n_el.style.stroke = '#FF8C00';\n\
                                n_el.style[\"stroke-width\"] = (n_el.propertyWidth + 1).toString() + \"px\";\n\
                            }\n\
                        }\n\
                    }\n\
                    if(element.tagName != \"text\"){\n\
                        this.style.stroke = '#FF8C00';\n\
                        this.style[\"stroke-width\"] = (this.propertyWidth + 1).toString() + \"px\";\n\
                    };\n\
                };\n\
            };\n\
            element.onmouseleave = function() {\n\
                if(!element.isClick){\n\
                    var classList = element.classList;\n\
                    if(classList.length > 1){\n\
                        for (let n_el of elements){\n\
                            if(n_el.classList[1] == classList[1] && n_el !== element){\n\
                                 n_el.style.stroke = n_el.propertyStroke;\n\
                                 n_el.style[\"stroke-width\"] = (n_el.propertyWidth).toString() + \"px\";\n\
                            }\n\
                        }\n\
                    }\n\
                    this.style.stroke = this.propertyStroke;\n\
                    this.style[\"stroke-width\"] = (this.propertyWidth).toString() + \"px\";\n\
                };\n\
            };\n\
            element.onclick = function() {\n\
                if(element.classList.length > 1){\n\
                    var net_elements = document.getElementsByClassName(element.classList[1]);\n\
                    for (let n_el of net_elements){\n\
                        if(!n_el.hasOwnProperty('isClick')) n_el.isClick = false;\n\
                        if(!n_el.isClick){\n\
                            n_el.style.stroke = '#FFFFFF';\n\
                            n_el.isClick = true;\n\
                        }\n\
                        else {\n\
                            n_el.style.stroke = n_el.propertyStroke;\n\
                            n_el.isClick = false;\n\
                        }\n\
                    }\n\
                }\n\
            };\n\
        };\n\
        ]]>\n\
   </script>\n";
}

PDIFType::PDIFType() 
{
}

PDIFType::~PDIFType()
{
}

string PDIFType::toSVG(string outputfile, double scale)
{    
    SCALE = scale;
    convert_coords();
    search_begin_coordinates();
    set_border_coordinates();

    string svg;

    svg += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    //svg
    svg += get_svg_setting();

    //title
    this->outputfile = outputfile;
    svg += "<title>" + this->outputfile + "</title>\n";

    //defs
    svg += generate_comp_def_patterns();

    //background    
    svg += "<rect x=\"" + to_string(minX - borderOffset) + "\" y=\"" + to_string(minY - borderOffset) + "\" height=\"100%\" width=\"100%\" fill=\"black\"/>\n";
    svg += "<g style=\"fill: none; stroke: none; font-family:Consolas;\">\n";
    
    vector<string> layer_groups, dotted, dashed;
    for (pair<string, string> l : ENVIRONMENT.Lyrstr) {
        string visible = isHidden(l.first) ? "visibility=\"hidden\"" : " ";
        layer_groups.push_back("<g " + visible + " id=\"" + l.first + "\" style=\"stroke:" + l.second + ";fill:" + l.second + ";\">\n");
        dotted.push_back("<g " + visible + " style=\"stroke-dasharray: 1 3;\">\n");
        dashed.push_back("<g " + visible + " style=\"stroke-dasharray: 5 5;\">\n");
    }

    for (GraphElement* element : SYMBOL.PIN_DEF) insert_into_layer(layer_groups, dotted, dashed, element);
    for (GraphElement* element : SYMBOL.pic) insert_into_layer(layer_groups,dotted,dashed,element);
    for (GraphElement* element : DETAIL.Annotate) insert_into_layer(layer_groups, dotted, dashed, element);  

    for (N* n : DETAIL.NET_DEF) {
        for (GraphElement* element : n->DG) insert_into_layer(layer_groups, dotted, dashed, element, n);
    }
    /*for (COMP_DEF* comp_def : DETAIL.COMP_DEFS) {
        for (Pnl* element : comp_def->locations) insert_into_layer(layer_groups, dotted, dashed, element);
        insert_into_layer(layer_groups, dotted, dashed, comp_def->rdl);
    }   */

    for (I* inst : DETAIL.instances) insert_instance(layer_groups, dotted, dashed, inst);

    for (int i = 0; i < layer_groups.size(); i++) {
        layer_groups[i] += dashed[i] + "</g>\n";
        layer_groups[i] += dotted[i] + "</g>\n";
        layer_groups[i] += "</g>\n";
        svg += layer_groups[i];
    }

    svg += "</g>\n";
    svg += js_script();
    svg += "</svg>";

    
    ofstream ofs(outputfile);
    ofs << svg;
    
    return svg;
} 