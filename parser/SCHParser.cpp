#include "SCHParser.h"

SCHParser::SCHParser(string file_name, double scale)
{
	file.open(file_name, ios::binary);
	SCALE = scale;

	if (!file.is_open()) throw SCHParser::Exception(file, "error opening the file");
}

string SCHParser::to_svg(string svg_file_name)
{
	parse();

	convert_coords();
	search_begin_coordinates();
	set_border_coordinates();

	string svg;

	svg += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	//svg
	svg += get_svg_setting();

	
	svg += "<title>" + svg_file_name + "</title>\n";

	//defs
	svg += generate_comp_def_patterns();

	//background    
	svg += "<rect x=\"" + to_string(minX - borderOffset) + "\" y=\"" + to_string(minY - borderOffset) + "\" height=\"100%\" width=\"100%\" fill=\"black\"/>\n";
	svg += "<g style=\"fill: none; stroke: none; font-family:Consolas;\">\n";

	vector<string> layer_groups, dotted, dashed;
	for (pair<string, string> l : ENVIRONMENT_LYRSTR) {
		string visible = isHidden(l.first) ? "visibility=\"hidden\"" : " ";
		layer_groups.push_back("<g " + visible + " id=\"" + l.first + "\" style=\"stroke:" + l.second + ";fill:" + l.second + ";\">\n");
		dotted.push_back("<g " + visible + " style=\"stroke-dasharray: 1 3;\">\n");
		dashed.push_back("<g " + visible + " style=\"stroke-dasharray: 5 5;\">\n");
	}

	for (GraphElement* element : SYMBOL_PIN_DEF) insert_into_layer(layer_groups, dotted, dashed, element);
	for (GraphElement* element : SYMBOL_PIC) insert_into_layer(layer_groups, dotted, dashed, element);
	for (GraphElement* element : DETAIL_ANNOTATE) insert_into_layer(layer_groups, dotted, dashed, element);

	for (N* n : DETAIL_NET_DEF) {
		for (GraphElement* element : n->DG) insert_into_layer(layer_groups, dotted, dashed, element, n);
	}

	for (I* inst : DETAIL_I) insert_instance(layer_groups, dotted, dashed, inst);

	for (int i = 0; i < layer_groups.size(); i++) {
		layer_groups[i] += dashed[i] + "</g>\n";
		layer_groups[i] += dotted[i] + "</g>\n";
		layer_groups[i] += "</g>\n";
		svg += layer_groups[i];
	}

	svg += "</g>\n";
	svg += js_script();
	svg += "</svg>";


	ofstream ofs(svg_file_name);
	ofs << svg;

	return svg;
}

void SCHParser::parse()
{
	parse_counts();
	get_sectons_offset();
	parse_layers();
	parse_offsets();
	parse_layers_struct();
	parse_net_def();
	parse_symbol_pindef();
	parse_symbol_pic();
	parse_detail_annotate();
	parse_sub_comp();
}

void SCHParser::clear_memory()
{

}

void SCHParser::parse_counts()
{
	file.seekg(sections_offset.count_struct);
	const int counts_struct_size = 64;
	file.read((char*)&counts, /*sizeof(COUNTS_STRUCT)*/counts_struct_size);
	if (counts.version != 0x68) throw Exception(file, "unsupported file version");
}

void SCHParser::get_sectons_offset()
{
	const int sch_p_size = 30;
	const int counts_struct_size = 64;
	const int offset_struct_size = 20;
	const int offset_subcomp_struct_size = 20;
	const int layer_struct_size = 16;
	const int net_def_size = 26;

	sections_offset.count_struct = 0x140;
	sections_offset.info_section = sections_offset.count_struct + /*sizeof(COUNTS_STRUCT)*/counts_struct_size;
	sections_offset.layers_section = sections_offset.info_section + 145 * 2;
	sections_offset.offset_struct = 0x896;
	sections_offset.offset_sub_comp_struct = sections_offset.offset_struct + counts.offset_struct_count * /*sizeof(OFFSET_STRUCT)*/offset_struct_size;
	sections_offset.layer_struct = sections_offset.offset_sub_comp_struct + counts.offset_subcomp_struct_count * /*sizeof(OFFSET_SUBCOMP_STRUCT)*/offset_subcomp_struct_size;
	sections_offset.net_def_struct = sections_offset.layer_struct + counts.layer_struct_count * /*sizeof(LAYER_STRUCT)*/layer_struct_size;
	sections_offset.p_struct = sections_offset.net_def_struct + counts.net_struct_count * /*sizeof(NET_DEF)*/net_def_size;
	sections_offset.pic_section = sections_offset.p_struct + counts.p_struct_count * /*sizeof(SCH_P)*/sch_p_size;

	/*cout << hex << setw(10) << sections_offset.count_struct;
	cout << hex << setw(10) << sections_offset.info_section;
	cout << hex << setw(10) << sections_offset.layers_section;
	cout << hex << setw(10) << sections_offset.offset_struct;
	cout << hex << setw(10) << sections_offset.offset_sub_comp_struct;
	cout << hex << setw(10) << sections_offset.layer_struct;
	cout << hex << setw(10) << sections_offset.net_def_struct;
	cout << hex << setw(10) << sections_offset.p_struct;
	cout << hex << setw(10) << sections_offset.pic_section;*/

	file.seekg(0, ios_base::end);
	sections_offset.end_file = file.tellg();
	file.seekg(0, ios_base::beg);
}

void SCHParser::parse_layers()
{
	file.seekg(sections_offset.layers_section);
	char ly[7]; int8_t color_index, is_hidden;

	USER_LV.push_back(0);

	do {	
		file.read((char*)&ly, 6);
		ly[6] = '\0';
		file.seekg(1, ios_base::cur);

		file.read((char*)&color_index, 1);
		file.seekg(1, ios_base::cur);

		file.read((char*)&is_hidden, 1);
		file.seekg(2, ios_base::cur);

		string sly = ly[0] == 0 ? "$$NULL" : string(ly);	
		pair<string, string> lyr(sly, colors[color_index]);
		
		ENVIRONMENT_LYRSTR.push_back(lyr);
		USER_LV.push_back(is_hidden);
	} while (ly[0] != 0);

	for (size_t i = 1; i < USER_LV.size(); i++) {
		if (USER_LV[i] > 0) USER_LV[0]++;
	}
}

void SCHParser::parse_offsets()
{
	file.seekg(sections_offset.offset_struct);	
	for (size_t i = 0; i < counts.offset_struct_count; i++)
	{
		OFFSET_STRUCT offset_struct;
		const int offset_struct_size = 20;
		file.read((char*)&offset_struct, /*sizeof(OFFSET_STRUCT)*/offset_struct_size);
		if (offset_struct.layer_struct_item_id >= 0) offsets.push_back(offset_struct);
	}
}

void SCHParser::parse_sub_comp()
{
	file.seekg(sections_offset.offset_sub_comp_struct);
	
	for (size_t i = 0; i < counts.offset_subcomp_struct_count; i++)
	{
		OFFSET_SUBCOMP_STRUCT off_sub_comp;
		const int offset_subcomp_struct_size = 20;
		file.read((char*)&off_sub_comp, /*sizeof(OFFSET_SUBCOMP_STRUCT)*/offset_subcomp_struct_size);
		if (off_sub_comp.id >= 0) offset_subcomps.push_back(off_sub_comp);
	}

	for (size_t i = 1; i < offset_subcomps.size(); i++)
	{
		for (size_t j = 0; j < layer_structs.size(); j++)
		{
			if (layer_structs[j].section_type == 0xc && layer_structs[j].item_id == offset_subcomps[i].id) {
				uint64_t offset = layer_structs[j].first_element_in_layer_offset + layer_structs[j].count_10000 * 0x10000;
				vector<GraphElement*> one_sub_comp_vec;				
				parse_section_into_pic(offset, get_next_layer_struct_offset(j), layer_structs[j].layer_index, one_sub_comp_vec);
				if (one_sub_comp_vec.empty() && one_sub_comp_vec.size() > 1) throw Exception(file, "comp_def not found, or several identical comp_def were found");
				DETAIL_COMP_DEFS.push_back((COMP_DEF*)one_sub_comp_vec[0]);
				break;
			}
		}

		OFFSET_STRUCT* offset_struct_for_subcomp = nullptr;

		for (OFFSET_STRUCT& offset_struct : offsets)
		{
			if (offset_struct.sub_comp_id == offset_subcomps[i].id) {
				offset_struct_for_subcomp = new OFFSET_STRUCT(offset_struct);
				break;
			}
		}

		if (offset_struct_for_subcomp == nullptr) throw Exception(file, "offset_struct not found for comp_def");

		OFFSET_SUBCOMP_STRUCT* subcomp_pointer = new OFFSET_SUBCOMP_STRUCT(offset_subcomps[i]);
		
		if (offset_struct_for_subcomp->offset_count_p >= 0) {
			int p_section_offset = sections_offset.p_struct;
			uint64_t offset = offset_struct_for_subcomp->offset_count_p;
			SCH_P sch_p;
			do {
				const int sch_p_size = 30;
				file.seekg(p_section_offset + offset * /*sizeof(SCH_P)*/sch_p_size);
				file.read((char*)&sch_p, /*sizeof(SCH_P)*/sch_p_size);
				change_coord_depending_on_offset(sch_p.x, sch_p.y, offset_struct_for_subcomp, subcomp_pointer);
				DETAIL_COMP_DEFS.back()->PIN_DEF.push_back(sch_p.to_P(ENVIRONMENT_LYRSTR));
				offset = sch_p.next_p_offset_count;
			} while ((short)offset > -1);
		}
		
		for (size_t j = 0; j < layer_structs.size(); j++)
		{
			if (layer_structs[j].section_type == 0x9 && layer_structs[j].item_id == offset_subcomps[i].id) {
				uint64_t offset = layer_structs[j].first_element_in_layer_offset + layer_structs[j].count_10000 * 0x10000;
				uint64_t next_of = get_next_layer_struct_offset(j);
				parse_section_into_pic(offset, next_of, layer_structs[j].layer_index, DETAIL_COMP_DEFS.back()->pic, offset_struct_for_subcomp, subcomp_pointer);
			}
		}

		delete subcomp_pointer;
		delete offset_struct_for_subcomp;

		for (OFFSET_STRUCT& offset_struct: offsets)
		{
			if (offset_struct.sub_comp_id == offset_subcomps[i].id) {
				vector<GraphElement*> one_i_vec;
				for (size_t j = 0; j < layer_structs.size(); j++)
				{
					if (layer_structs[j].section_type == 0x10 && layer_structs[j].item_id == offset_struct.layer_struct_item_id) {
						uint64_t offset = layer_structs[j].first_element_in_layer_offset + layer_structs[j].count_10000 * 0x10000;
						parse_section_into_pic(offset, get_next_layer_struct_offset(j), layer_structs[j].layer_index, one_i_vec);

						if(one_i_vec.empty() || one_i_vec.size() > 1) throw Exception(file, "struct I not found, or several identical I were found");

						((I*)one_i_vec[0])->comp_def = DETAIL_COMP_DEFS.back();
						/*if (((I*)one_i_vec[0])->inv_name == "UC300102") {
							((I*)one_i_vec[0])->comp_pin_def = DETAIL_COMP_DEFS.back()->PIN_DEF;
						}*/
						//((I*)one_i_vec[0])->comp_pin_def = DETAIL_COMP_DEFS.back()->PIN_DEF;
						((I*)one_i_vec[0])->file_name = DETAIL_COMP_DEFS.back()->file_name;
						((I*)one_i_vec[0])->Ro = offset_struct.Ro;

						OFFSET_STRUCT* offset_pointer = new OFFSET_STRUCT(offset_struct);
						set_right_coord(offset_subcomps[i].pl_x, offset_subcomps[i].pl_y, offset_pointer, ((I*)one_i_vec[0])->Pl.x, ((I*)one_i_vec[0])->Pl.y);
						delete offset_pointer;

						DETAIL_I.push_back((I*)one_i_vec[0]);					
						break;
					}
				}
			}
		}
	}
}

void SCHParser::parse_layers_struct()
{
	file.seekg(sections_offset.layer_struct);
	LAYER_STRUCT layer_struct;
	for (size_t i = 0; i < counts.layer_struct_count; i++)
	{	
		const int layer_struct_size = 16;
		file.read((char*)&layer_struct, /*sizeof(LAYER_STRUCT)*/layer_struct_size);
		if (layer_struct.offset_struct_id >= 0) {
			layer_structs.push_back(layer_struct);
		}
	}
}

void SCHParser::parse_net_def()
{
	file.seekg(sections_offset.net_def_struct);
	NET_DEF net_def;
	for (size_t i = 0; i < counts.net_struct_count; i++)
	{	
		const int net_def_size = 26;
		file.read((char*)&net_def, /*sizeof(NET_DEF)*/net_def_size);
		net_def.print();
		if (net_def.layer_struct_id > 0) {
			DETAIL_NET_DEF.push_back(new N(net_def.pin_name, "1", "Y"));

			if (net_def.DG_flag != -1) {
				for (size_t j = 0; j < layer_structs.size(); j++)
				{
					if (layer_structs[j].section_type == 0xb && layer_structs[j].item_id == net_def.layer_struct_id) {
						uint64_t offset = layer_structs[j].first_element_in_layer_offset + 0x10000 * layer_structs[j].count_10000;
						uint64_t next_offset = get_next_layer_struct_offset(j);
						parse_section_into_pic(offset, next_offset, layer_structs[j].layer_index, ((N*)DETAIL_NET_DEF.back())->DG);
					}
				}
			}
		}		
	}
}

void SCHParser::parse_symbol_pindef()
{
	uint64_t p_section_offset = sections_offset.p_struct;
	int16_t offset = offsets[0].offset_count_p;
	SCH_P sch_p;
	if (offset < 0) return;

	do {
		const int sch_p_size = 30;
		file.seekg(p_section_offset + offset * /*sizeof(SCH_P)*/sch_p_size);
		file.read((char*)&sch_p, /*sizeof(SCH_P)*/sch_p_size);
		SYMBOL_PIN_DEF.push_back(sch_p.to_P(ENVIRONMENT_LYRSTR));
		offset = sch_p.next_p_offset_count;
	} while ((short)offset > -1);
}

void SCHParser::parse_symbol_pic()
{
	for (size_t i = 0; i < layer_structs.size(); i++)
	{
		if (layer_structs[i].section_type == 0x9 && layer_structs[i].item_id == 0) {
			int64_t offset = layer_structs[i].first_element_in_layer_offset + layer_structs[i].count_10000 * 0x10000;
			uint64_t next = get_next_layer_struct_offset(i);
			parse_section_into_pic(offset, next, layer_structs[i].layer_index, SYMBOL_PIC);
		}	
	}
}

void SCHParser::parse_detail_annotate()
{
	for (size_t i = 0; i < layer_structs.size(); i++)
	{
		if (layer_structs[i].section_type == 0xA) {
			uint64_t start_offset = layer_structs[i].first_element_in_layer_offset + layer_structs[i].count_10000 * 0x10000;
			uint64_t next_offset = get_next_layer_struct_offset(i);
			parse_section_into_pic(start_offset, get_next_layer_struct_offset(i), layer_structs[i].layer_index, DETAIL_ANNOTATE);
		}
	}
}

void SCHParser::parse_section_into_pic(uint64_t begin_offset, uint64_t end_offset, int layer_index, vector<GraphElement*>& dest_arr, OFFSET_STRUCT* offset_struct_for_subcomp, OFFSET_SUBCOMP_STRUCT* offset_subcomp)
{
	int16_t size, sep0, sep1, sep2, bin_el_type, sep3, sep4, x = 0, y = 0;
	int8_t line_type, wd;
	uint64_t old_pos = file.tellg();

	file.seekg(begin_offset);

	while (file.tellg() != end_offset) {
		if (file.tellg() >= end_offset) throw Exception(file, "going outside the section");

		//cout << hex << "pos: " << file.tellg() << " end_offs: " << end_offset << endl;

		file.read((char*)&sep0, 2); //8003
		file.read((char*)&size, 2);

		if(sep0 != (int16_t)0x8003) throw Exception(file, "the beginning of the section was expected, but it was received: " + to_string(sep0));

		file.read((char*)&sep1, 2);  //8006				
		file.read((char*)&sep2, 2);
			
		file.read((char*)&bin_el_type, 2); //type
		file.read((char*)&line_type, 1);
		file.read((char*)&wd, 1);
	
		Layer ly = Layer(get_ly(layer_index), get_type_of_line(line_type), wd);

		if (bin_el_type == (int16_t)0x80A0) parse_compdef(ly, dest_arr, size);
		else if (bin_el_type == (int16_t)0x80A1) parse_v(ly, dest_arr, size, offset_struct_for_subcomp, offset_subcomp);
		else if (bin_el_type == (int16_t)0x80A2) parse_text(ly, dest_arr, size, offset_struct_for_subcomp, offset_subcomp);
		else if (bin_el_type == (int16_t)0x80A5) parse_circle(ly, dest_arr, size, offset_struct_for_subcomp, offset_subcomp);
		else if (bin_el_type == (int16_t)0x80A6 || bin_el_type == (int16_t)0x80A7) parse_rect(ly, dest_arr, size, bin_el_type == (int16_t)0x80A6, offset_struct_for_subcomp, offset_subcomp);
		else if (bin_el_type == (int16_t)0x80A8) parse_line(ly, dest_arr, size, offset_struct_for_subcomp, offset_subcomp);
		else if (bin_el_type == (int16_t)0x807A) parse_arc(ly, dest_arr, size, offset_struct_for_subcomp, offset_subcomp);
		else if (bin_el_type == (int16_t)0x8098) parse_i(ly, dest_arr, size, offset_struct_for_subcomp, offset_subcomp);
		else if (bin_el_type == (int16_t)0x809E) parse_wire(ly, dest_arr, size, offset_struct_for_subcomp, offset_subcomp);
		else if (bin_el_type == (int16_t)0x809F) parse_nn(ly, dest_arr, size, offset_struct_for_subcomp, offset_subcomp);
		else {
			cout << "unknown element: " << hex << bin_el_type << ", pos: " << file.tellg() << endl;
			file.seekg(4 * size - 12, ios_base::cur);
			continue;
		}
	}

	file.seekg(old_pos);
}

void SCHParser::parse_line(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset, OFFSET_SUBCOMP_STRUCT* offset_subcomp) {
	dest_arr.push_back(new Line(layer));

	int16_t x, y, len = 0;
	int8_t ls, wd;

	while (true) {
		file.read((char*)&x, 2); file.read((char*)&y, 2);

		len += 4;

		if (x <= (int16_t)0x8300) {
			if (x == (int16_t)0x80A8) {
				file.seekg(-2, ios_base::cur);
				file.read((char*)&ls, 1);
				file.read((char*)&wd, 1);
				layer.Ls = get_type_of_line(ls);
				layer.Wd = wd;
				dest_arr.push_back(new Line(layer));
				continue;
			}
			break;
		}	

		if (len > size * 4 - 12) throw Exception(file, "going beyond the section boundary (line)");

		if (offset != nullptr) change_coord_depending_on_offset(x, y, offset, offset_subcomp);

		((Line*)dest_arr.back())->coordinates.push_back(pair<int, int>(x, y));
	}
}

void SCHParser::parse_text(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset, OFFSET_SUBCOMP_STRUCT* offset_subcomp)
{
	dest_arr.push_back(new Text(layer));

	int16_t x, y, len = 0;
	int8_t tr, ts, tm, tj, text_value[4];

	if (offset_subcomp != nullptr) {
		if(offset_subcomp->id == 0x1d)
		int a = 5;
	}

	while (true) {
		file.read((char*)&x, 2); file.read((char*)&y, 2);
		if (offset != nullptr) change_coord_depending_on_offset(x, y, offset, offset_subcomp);
		((Text*)dest_arr.back())->x = x;
		((Text*)dest_arr.back())->y = y;
		file.read((char*)&tr, 1);
		file.read((char*)&ts, 1);
		((Text*)dest_arr.back())->layer.Tr = (int)tr;
		((Text*)dest_arr.back())->layer.Ts = (int)ts;
		file.read((char*)&tm, 1);
		file.read((char*)&tj, 1);
		((Text*)dest_arr.back())->layer.Tm = get_tm(tm);
		((Text*)dest_arr.back())->layer.Tj = get_tj(tj);
		len += 8;

		while (true) {
			file.read((char*)&x, 2); file.read((char*)&y, 2);
			len += 4;

			if (x <= (int16_t)0x8300) {
				if (x == (int16_t)0x80A2) {
					dest_arr.push_back(new Text(layer));
					break;
				}
				return;
			}

			if (len > size * 4 - 12)  throw Exception(file, "going beyond the section boundary (text)");

			file.seekg(-4, ios_base::cur);
			file.read((char*)text_value, 4);
			if(is_ascii_code(text_value[1])) ((Text*)dest_arr.back())->text += (char)text_value[1];		
			if (is_ascii_code(text_value[0]))((Text*)dest_arr.back())->text += (char)text_value[0];
			if (is_ascii_code(text_value[3]))((Text*)dest_arr.back())->text += (char)text_value[3];
			if (is_ascii_code(text_value[2]))((Text*)dest_arr.back())->text += (char)text_value[2];
		}
	}	
}

void SCHParser::parse_circle(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset, OFFSET_SUBCOMP_STRUCT* offset_subcomp)
{
	dest_arr.push_back(new Circle(layer));
	int16_t x, y, len = 0;
	int8_t ls, wd;
	while (true) {
		file.read((char*)&x, 2); file.read((char*)&y, 2);
		if (offset != nullptr) change_coord_depending_on_offset(x, y, offset, offset_subcomp);
		((Circle*)dest_arr.back())->x = x;
		((Circle*)dest_arr.back())->y = y;
		file.read((char*)&x, 2); file.read((char*)&y, 2);
		((Circle*)dest_arr.back())->r = x;

		file.read((char*)&x, 2); file.read((char*)&y, 2);

		len += 12;

		if (x <= (int16_t)0x8300) {
			if (x == (int16_t)0x80A5) {
				file.seekg(-2, ios_base::cur);
				file.read((char*)&ls, 1);
				file.read((char*)&wd, 1);
				layer.Ls = get_type_of_line(ls);
				layer.Wd = wd;
				dest_arr.push_back(new Circle(layer));
				continue;
			}
			break;
		}

		if (len >= size * 4 - 12) throw Exception(file, "going beyond the section boundary (circle)");


		len -= 4;
		file.seekg(-4, ios_base::cur);
		dest_arr.push_back(new Circle(layer));
	}
}

void SCHParser::parse_rect(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, bool is_filled, OFFSET_STRUCT* offset, OFFSET_SUBCOMP_STRUCT* offset_subcomp)
{
	dest_arr.push_back(new Rect(layer, is_filled));
	int16_t x, y, len = 0;
	int8_t ls, wd;
	while (true) {
		file.read((char*)&x, 2); file.read((char*)&y, 2); 
		if (offset != nullptr) change_coord_depending_on_offset(x, y, offset, offset_subcomp);
		((Rect*)dest_arr.back())->x1 = x;
		((Rect*)dest_arr.back())->y1 = y;
		file.read((char*)&x, 2); file.read((char*)&y, 2);
		if (offset != nullptr) change_coord_depending_on_offset(x, y, offset, offset_subcomp);
		((Rect*)dest_arr.back())->x2 = x;
		((Rect*)dest_arr.back())->y2 = y;

		file.read((char*)&x, 2); file.read((char*)&y, 2);

		len += 12;

		if (x <= (int16_t)8300) {
			if (x == (int16_t)0x80A6 || x == (int16_t)0x80A7) {
				file.seekg(-2, ios_base::cur);
				file.read((char*)&ls, 1);
				file.read((char*)&wd, 1);
				layer.Ls = get_type_of_line(ls);
				layer.Wd = wd;
				dest_arr.push_back(new Rect(layer, x == (int16_t)0x80A6));
				continue;
			}
			break;
		}

		if (len >= size * 4 - 12) throw Exception(file, "going beyond the section boundary (rect)");
	}
}

void SCHParser::parse_arc(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset, OFFSET_SUBCOMP_STRUCT* offset_subcomp)
{
	dest_arr.push_back(new Arc(layer));

	int16_t x, y, len = 0;
	int8_t ls, wd;
	while (true) {
		file.read((char*)&x, 2); file.read((char*)&y, 2);
		if (offset != nullptr) change_coord_depending_on_offset(x, y, offset, offset_subcomp);
		((Arc*)dest_arr.back())->x = x;
		((Arc*)dest_arr.back())->y = y;
		file.read((char*)&x, 2); file.read((char*)&y, 2);
		if (offset != nullptr) change_coord_depending_on_offset(x, y, offset, offset_subcomp);
		((Arc*)dest_arr.back())->rx = x;
		((Arc*)dest_arr.back())->ry = y;
		file.read((char*)&x, 2); file.read((char*)&y, 2);
		if (offset != nullptr) change_coord_depending_on_offset(x, y, offset, offset_subcomp);
		((Arc*)dest_arr.back())->ex = x;
		((Arc*)dest_arr.back())->ey = y;

		file.read((char*)&x, 2); file.read((char*)&y, 2);

		len += 8 * 2;

		if (x <= (int16_t)0x8300) {
			if (x == (int16_t)0x807A) {
				file.seekg(-2, ios_base::cur);
				file.read((char*)&ls, 1);
				file.read((char*)&wd, 1);
				layer.Ls = get_type_of_line(ls);
				layer.Wd = wd;
				dest_arr.push_back(new Arc(layer));
				continue;
			}
			break;
		}

		if (len >= size * 4 - 12)  throw Exception(file, "going beyond the section boundary (arc)");

		len -= 4;
		file.seekg(-4, ios_base::cur);
		dest_arr.push_back(new Arc(layer));
	}
}

void SCHParser::parse_wire(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset, OFFSET_SUBCOMP_STRUCT* offset_subcomp)
{
	dest_arr.push_back(new Wire(layer));
	int16_t x, y, len = 0;
	int8_t ls, wd;

	while (true) {
		file.read((char*)&x, 2); file.read((char*)&y, 2);
		len += 4;
		if (x <= (int16_t)0x8300) {
			if (x == (int16_t)0x809E) {
				file.seekg(-2, ios_base::cur);
				file.read((char*)&ls, 1);
				file.read((char*)&wd, 1);
				layer.Ls = get_type_of_line(ls);
				layer.Wd = wd;
				dest_arr.push_back(new Wire(layer));
				continue;
			}
			break;
		}

		if (len >= size * 4 - 12)  throw Exception(file, "going beyond the section boundary (wire)");

		if (offset != nullptr) change_coord_depending_on_offset(x, y, offset, offset_subcomp);
		((Wire*)dest_arr.back())->coordinates.push_back(pair<int, int>(x, y));
	}
}

void SCHParser::parse_nn(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset, OFFSET_SUBCOMP_STRUCT* offset_subcomp)
{
	dest_arr.push_back(new Nn(layer));
	int16_t x, y, len = 0;
	int8_t tr, ts, tm, tj, ls, wd;
	while (true) {
		file.read((char*)&x, 2); file.read((char*)&y, 2);
		if (offset != nullptr) change_coord_depending_on_offset(x, y, offset, offset_subcomp);
		((Nn*)dest_arr.back())->coordinates.push_back(pair<int, int>(x, y));

		file.read((char*)&tr, 1);
		file.read((char*)&ts, 1);
		((Nn*)dest_arr.back())->layer.Tr = (int)tr;
		((Nn*)dest_arr.back())->layer.Ts = (int)ts;
		file.read((char*)&tm, 1);
		file.read((char*)&tj, 1);
		((Nn*)dest_arr.back())->layer.Tm = get_tm(tm);
		((Nn*)dest_arr.back())->layer.Tj = get_tj(tj);

		len += 8;

		file.read((char*)&x, 2); file.read((char*)&y, 2);

		if (x <= (int16_t)0x8300) {
			if (x == (int16_t)0x809F) {
				file.seekg(-2, ios_base::cur);
				file.read((char*)&ls, 1);
				file.read((char*)&wd, 1);
				layer.Ls = get_type_of_line(ls);
				layer.Wd = wd;
				dest_arr.push_back(new Nn(layer));
				continue;
			}
			break;
		}

		if (len >= size * 4 - 12)  throw Exception(file, "going beyond the section boundary (nn)");

		len -= 4;
		file.seekg(-4, ios_base::cur);
		dest_arr.push_back(new Nn(layer));
	}
}

void SCHParser::parse_i(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset, OFFSET_SUBCOMP_STRUCT* offset_subcomp)
{
	dest_arr.push_back(new I());

	int16_t x, y, len = 0;
	char text[4];

	while (true) {
		file.read((char*)&x, 2); file.read((char*)&y, 2);
		file.read((char*)&x, 2); file.read((char*)&y, 2);

		len += 8;

		while (true) {
			file.read((char*)&x, 2); file.read((char*)&y, 2); len += 4;
			if (x <= (int16_t)0x8300) {
				if (x == (int16_t)0x80A8) {
					throw Exception(file, "more than one struct I found");
				}
				return;
			}
			file.seekg(-4, ios_base::cur);
			file.read((char*)text, 4);
			if (is_ascii_code(text[1])) ((I*)dest_arr.back())->inv_name += (char)text[1];
			if (is_ascii_code(text[0])) ((I*)dest_arr.back())->inv_name += (char)text[0];
			if (is_ascii_code(text[3])) ((I*)dest_arr.back())->inv_name += (char)text[3];
			if (is_ascii_code(text[2])) ((I*)dest_arr.back())->inv_name += (char)text[2];
		}

		if (len >= size * 4 - 12)  throw Exception(file, "going beyond the section boundary (i)");
	}
}

void SCHParser::parse_v(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset, OFFSET_SUBCOMP_STRUCT* offset_subcomp)
{
	dest_arr.push_back(new V(layer));

	int16_t x, y, len = 0;
	int8_t  ls, wd;

	file.seekg(-2, ios_base::cur);
	file.read((char*)&ls, 1);
	file.read((char*)&wd, 1);

	while (true) {
		file.read((char*)&x, 2); file.read((char*)&y, 2);
		if (offset != nullptr) change_coord_depending_on_offset(x, y, offset, offset_subcomp);
		((V*)dest_arr.back())->x = x;
		((V*)dest_arr.back())->y = y;
		((V*)dest_arr.back())->via_type = ls;

		file.read((char*)&x, 2); file.read((char*)&y, 2);
		len += 8;

		if (x <= (int16_t)0x8300) {
			if (x == (int16_t)0x80A1) {
				file.seekg(-2, ios_base::cur);
				file.read((char*)&ls, 1);
				file.read((char*)&wd, 1);
				layer.Ls = get_type_of_line(ls);
				layer.Wd = wd;
				dest_arr.push_back(new V(layer));
				continue;
			}
			break;
		}

		if (len >= size * 4 - 12)  throw Exception(file, "going beyond the section boundary (v)");

		len -= 4;
		file.seekg(-4, ios_base::cur);
		dest_arr.push_back(new V(layer));
	}
}

void SCHParser::parse_compdef(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size)
{
	dest_arr.push_back(new COMP_DEF());
	string name; char arr4[4]; 
	int16_t x, y, len = 0;
	while (true) {
		file.read((char*)&x, 2); file.read((char*)&y, 2);
		len += 4;
		if (x <= (int16_t)0x8300) {
			if (len == size * 4 - 12) {
				((COMP_DEF*)dest_arr.back())->file_name = name;
				break;
			}
			cout << "warning: bad compdef: " << name << endl;
			continue;
		}
		file.seekg(-4, ios_base::cur);
		file.read((char*)arr4, 4);
		for (size_t i = 0; i < 4; i++) {
			if (is_ascii_code(arr4[i])) name += (char)arr4[i];
		}

		if (len >= size * 4 - 12)  throw Exception(file, "going beyond the section boundary (compdef)");
	}
}

int64_t SCHParser::get_next_layer_struct_offset(int ly_i)
{
	uint64_t offset = layer_structs[ly_i].first_element_in_layer_offset;
	uint64_t next_offset = sections_offset.end_file;
	uint64_t dist_offset = sections_offset.end_file;
	size_t tj = ly_i + 1;

	if (ly_i != layer_structs.size() - 1) {
		next_offset = layer_structs[ly_i + 1].first_element_in_layer_offset + layer_structs[ly_i + 1].count_10000 * 0x10000;
	}

	return next_offset;
}

string SCHParser::get_ly(int i)
{
	if (i < 0 || i > ENVIRONMENT_LYRSTR.size() - 1) return "";
	return ENVIRONMENT_LYRSTR[i].first;
}

string SCHParser::get_tj(int i)
{
	if (i == 0x22) return "LB";
	if (i == 0x19) return "CT";
	if (i == 0x2a) return "RB";
	if (i == 0x23) return "LC";
	return "-(" + to_string(i) + ")";
}

string SCHParser::get_tm(int i)
{
	if (i == 0) return "N";
	return "-";
}

string SCHParser::get_type_of_line(int8_t type)
{
	string arr[3] = { "SOLID" , "DOTTED" , "DASHED" };
	return type >= 0 && type <= 3 ? arr[type] : arr[0];
}

void SCHParser::set_right_coord(int16_t s_x, int16_t s_y, OFFSET_STRUCT* offset, int16_t& x, int16_t& y)
{
	int16_t Ro4 = offset->Ro & 4, Ro3 = offset->Ro & 3;
	int16_t pl_x = 0, pl_y = 0;

	if (Ro4 != 0) {
		if (Ro3 == 0) {
			pl_x = offset->var4 - s_x;
			pl_y = s_y + offset->pl_y;
		}
		else if (Ro3 == 1) {
			pl_y = offset->var4 - s_y;
			pl_x = offset->var5 - s_x;
		}
		else if (Ro3 == 2) {
			pl_x = s_x + offset->pl_x;
			pl_y = offset->var5 - s_y;
		}
		else if (Ro3 == 3) {
			pl_x = s_y + offset->pl_x;
			pl_y = s_x + offset->pl_y;
		}
	}
	else {
		if (Ro3 == 0) {
			pl_x = s_x + offset->pl_x;
			pl_y = s_y + offset->pl_y;
		}
		else if (Ro3 == 1) {
			pl_x = offset->var4 - s_y;
			pl_y = offset->pl_y + s_x;
		}
		else if (Ro3 == 2) {
			pl_x = offset->var4 - s_x;
			pl_y = offset->var5 - s_y;
		}
		else if (Ro3 == 3) {
			pl_x = s_y + offset->pl_x;
			pl_y = offset->var5 - s_x;
		}
	}

	x = pl_x;
	y = pl_y;
}

void SCHParser::set_right_coord2(int16_t& x, int16_t& y, int8_t Ro)
{
	int vx, vy;

	if (Ro > 0 && Ro <= 7) {
		vx = x, vy = y;

		if (Ro > 3) {
			vx *= -1; Ro -= 4;
		}

		if (Ro == 0) x = vx;
		else if (Ro == 1) {
			x = vy * -1;
			y = vx;
		}
		else if (Ro == 2) {
			x = vx * -1;
			y = vy * -1;
		}
		else if (Ro == 3) {
			x = vy;
			y = vx * -1;
		}
	}
}

void SCHParser::change_coord_depending_on_offset(int16_t& x, int16_t& y, OFFSET_STRUCT* offset, OFFSET_SUBCOMP_STRUCT* offset_subcomp)
{
	if (offset_subcomp == nullptr || offset == nullptr)  throw Exception(file, "offset_subcomp == nullptr || offset == nullptr");

	set_right_coord(x, y, offset, x, y);
	if ((offset->Ro & 3) != 0) set_right_coord2(x, y, 4 - (offset->Ro & 3));
	if ((offset->Ro & 4) != 0) set_right_coord2(x, y, 4);

	int16_t ax = offset_subcomp->pl_x, ay = offset_subcomp->pl_y;
	set_right_coord(ax, ay, offset, ax, ay);
	if ((offset->Ro & 3) != 0) set_right_coord2(ax, ay, 4 - (offset->Ro & 3));
	if ((offset->Ro & 4) != 0) set_right_coord2(ax, ay, 4);

	x -= ax;
	y -= ay;
}

bool SCHParser::is_ascii_code(int8_t c)
{
	return c >= 32 && c <= 125 && c != 34 && c !=39 && c!=96 && c != 38;
}

void SCHParser::convert_coords()
{
	for (GraphElement* element : SYMBOL_PIN_DEF) negativeY(element);
	for (GraphElement* element : SYMBOL_PIC) negativeY(element);
	for (GraphElement* element : DETAIL_ANNOTATE) negativeY(element);
	for (N* n : DETAIL_NET_DEF) {
		for (GraphElement* element : n->DG) negativeY(element);
	}
	for (COMP_DEF* comp_def : DETAIL_COMP_DEFS) {
		for (GraphElement* element : comp_def->PIN_DEF) negativeY(element);
		for (GraphElement* element : comp_def->locations) negativeY(element);
		for (GraphElement* element : comp_def->pic) negativeY(element);
		
	}
	for (I* i : DETAIL_I) {
		i->Pl.y *= -1;
	}
}

void SCHParser::negativeY(GraphElement* element)
{
	if (element->getType() == token_type::t) ((Text*)element)->y *= -1;
	else if (element->getType() == token_type::c) ((Circle*)element)->y *= -1;
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

bool SCHParser::set_begin_coordinate(GraphElement* element)
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

	return false;
}

void SCHParser::search_begin_coordinates()
{
	for (GraphElement* element : SYMBOL_PIN_DEF) {
		if (set_begin_coordinate(element))  return;
	}
	for (GraphElement* element : SYMBOL_PIC) {
		if (set_begin_coordinate(element))  return;
	}
	for (GraphElement* element : DETAIL_ANNOTATE) {
		if (set_begin_coordinate(element))  return;
	}
	for (N* n : DETAIL_NET_DEF) {
		for (GraphElement* element : n->DG) {
			if (set_begin_coordinate(element))  return;
		}
	}
	for (I* i : DETAIL_I) {
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

void SCHParser::set_border_coordinate(GraphElement* element, I* i)
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

	if (degrees != 0) delete t_element;
}

void SCHParser::set_border_coordinates()
{
	for (GraphElement* element : SYMBOL_PIN_DEF) set_border_coordinate(element);
	for (GraphElement* element : SYMBOL_PIC) set_border_coordinate(element);
	for (GraphElement* element : DETAIL_ANNOTATE) set_border_coordinate(element);
	for (N* n : DETAIL_NET_DEF) {
		for (GraphElement* element : n->DG) set_border_coordinate(element);
	}
	for (I* i : DETAIL_I) {
		for (GraphElement* element : i->comp_def->PIN_DEF) set_border_coordinate(element, i);
		for (GraphElement* element : i->comp_def->pic) set_border_coordinate(element, i);
		for (GraphElement* element : i->asg) set_border_coordinate(element, i);
	}
}

void SCHParser::rotatePoint(int& x, int& y, int cx, int cy, int angle)
{
	double s = sin(angle);
	double c = cos(angle);

	double xnew = x * c - y * s;
	double ynew = x * s + y * c;

	x = xnew + cx;
	y = ynew + cy;
}

GraphElement* SCHParser::rotateFigure(GraphElement* element, int cx, int cy, int angle)
{
	GraphElement* t_element = nullptr;

	if (element->getType() == token_type::c) {
		t_element = new Circle(*(Circle*)element);
		rotatePoint(((Circle*)t_element)->x, ((Circle*)t_element)->y, cx, cy, angle);
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

void SCHParser::insert_into_layer(vector<string>& layer_groups, vector<string>& dotted, vector<string>& dashed, GraphElement* element, N* n)
{
	for (int i = 0; i < ENVIRONMENT_LYRSTR.size(); i++)
	{
		if (element->layer.Ly == ENVIRONMENT_LYRSTR[i].first) {
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

string SCHParser::generate_comp_def_patterns()
{
	string defs = "<defs>\n";

	for (COMP_DEF* comp_def : DETAIL_COMP_DEFS) {
		string comp_def_svg;
		vector<string> layer_groups, dotted, dashed;

		for (pair<string, string> l : ENVIRONMENT_LYRSTR) {
			layer_groups.push_back("<g id=\"" + comp_def->file_name + "_" + l.first + "_SOLID\">\n");
			dotted.push_back("<g id=\"" + comp_def->file_name + "_" + l.first + "_DOTTED\">\n");
			dashed.push_back("<g id=\"" + comp_def->file_name + "_" + l.first + "_DASHED\">\n");
		}

		for (GraphElement* element : comp_def->pic) insert_into_layer(layer_groups, dotted, dashed, element);

		for (int i = 0; i < layer_groups.size(); i++) {
			layer_groups[i] += "</g>\n";
			int empty_length = 18 + comp_def->file_name.size() + ENVIRONMENT_LYRSTR[i].first.size();
			if (dashed[i].size() != empty_length) {
				layer_groups[i] += dashed[i] + "</g>\n";
				comp_def->layers.push_back(ENVIRONMENT_LYRSTR[i].first + "_DASHED");
			}
			if (dotted[i].size() != empty_length) {
				layer_groups[i] += dotted[i] + "</g>\n";
				comp_def->layers.push_back(ENVIRONMENT_LYRSTR[i].first + "_DOTTED");
			}
			if (layer_groups[i].size() != empty_length + 4) {
				comp_def_svg += layer_groups[i];
				comp_def->layers.push_back(ENVIRONMENT_LYRSTR[i].first);
			}
		}
		defs += comp_def_svg;
	}

	defs += "</defs>\n";
	return defs;
}

void SCHParser::insert_instance(vector<string>& layer_groups, vector<string>& dotted, vector<string>& dashed, I* instance)
{
	int degrees = instance->Ro == 1 ? -90 : instance->Ro == 2 ? 180 : instance->Ro == 3 ? 90 : 0;

	for (string comp_def_layer : instance->comp_def->layers) {
		for (int i = 0; i < ENVIRONMENT_LYRSTR.size(); i++)
		{
			string use, inst_inform, ls = ENVIRONMENT_LYRSTR[i].first;

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

		for (int i = 0; i < ENVIRONMENT_LYRSTR.size(); i++)
		{
			if (p->layer.Ly == ENVIRONMENT_LYRSTR[i].first) {
				string svg = "<g class=\"element \" ";
				svg += " transform=\"";
				if (degrees != 0) svg += "rotate(" + to_string(degrees) + "," + to_string(instance->Pl.x) + ',' + to_string(instance->Pl.y) + ") ";
				svg += "translate(" + to_string(instance->Pl.x) + ',' + to_string(instance->Pl.y) + ")\"";
				svg += ">\n";
				svg += p->getSvgCode(nullptr);
				svg += "\n</g>\n";
				layer_groups[i] += svg;
			}
		}
	}
}

string SCHParser::get_svg_setting()
{
	WIDTH = abs((int)((maxX - minX + borderOffset * 2) * SCALE));
	HEIGHT = abs((int)((maxY - minY + borderOffset * 2) * SCALE));
	string scale_width = to_string(int(WIDTH * 1.0 / SCALE));
	string scale_height = to_string(int(HEIGHT * 1.0 / SCALE));

	return "<svg width=\"" + to_string(WIDTH) + "\" height=\"" + to_string(HEIGHT) +
		"\" viewBox=\"" + to_string(minX - borderOffset) + " " + to_string(minY - borderOffset) + " " + scale_width + " " + scale_height + "\"" +
		" preserveAspectRatio=\"xMinYMin meet\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n";
}

bool SCHParser::isHidden(string ly)
{
	for (int i = 0; i < ENVIRONMENT_LYRSTR.size() - 1; i++)
	{
		if (ENVIRONMENT_LYRSTR[i].first == ly && USER_LV[i + 1] == 0) {
			return true;
		}
	}
	return false;
}

string SCHParser::js_script()
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
