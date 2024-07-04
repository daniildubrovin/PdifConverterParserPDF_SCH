#ifndef SCHPARSER_H
#define SCHPARSER_H

#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include "GraphElement.h"
#include "PDIFType.h"
#include "PDIFLexer.h"

//#define WINDOWS_IGNORE_PACKING_MISMATCH

using namespace std;

 class SCHParser
{
public:
	 SCHParser(string file_name, double scale);
	 string to_svg(string svg_file_name);
	 void clear_memory();

	struct Exception {
		uint64_t file_pos;
		string msg;
		Exception(ifstream& file, string msg) {
			this->msg = msg;
			file_pos = file.tellg();
		}
		string what() {
			return std::to_string(file_pos) + ": " + msg;
		}
	};

private:
	struct OFFSET_STRUCT;
	struct OFFSET_SUBCOMP_STRUCT;
	struct LAYER_STRUCT;
	struct NET_DEF;
	struct SCH_P;
	struct SECTIONS_OFFSET;
	struct COUNTS_STRUCT;

	ifstream file;
	vector<string> colors { "black","lime","red","yellow","blue","aqua","fuchsia","rgba(85,85,85,255)",
	"rgba(0,85,0,255)","rgba(255,170,0,255)","rgba(170,85,0,255)","rgba(0,170,255,255)","rgba(255,170,170,255)",
	"rgba(255,0,170,255)","rgba(170,170,170,255)","chartreuse" };	
	vector<OFFSET_STRUCT> offsets;
	vector<OFFSET_SUBCOMP_STRUCT> offset_subcomps;
	vector<LAYER_STRUCT> layer_structs;

	vector<pair<string, string>> ENVIRONMENT_LYRSTR;
	vector<int> USER_LV;
	vector<P*> SYMBOL_PIN_DEF;
	vector<GraphElement*> SYMBOL_PIC;
	std::vector<GraphElement*> DETAIL_ANNOTATE;
	std::vector<N*> DETAIL_NET_DEF;
	std::vector<COMP_DEF*> DETAIL_COMP_DEFS;
	std::vector<I*> DETAIL_I;
	int WIDTH, HEIGHT;
	double SCALE;
	int minX, minY;
	int maxX, maxY;
	int borderOffset = 10;

	void parse();
	void parse_counts();
	void get_sectons_offset();
	void parse_layers();
	void parse_offsets();
	void parse_sub_comp();
	void parse_layers_struct();
	void parse_net_def();
	void parse_symbol_pindef();
	void parse_symbol_pic();
	void parse_detail_annotate();

	void parse_line(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset = nullptr, OFFSET_SUBCOMP_STRUCT* offset_subcomp = nullptr);
	void parse_text(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset = nullptr, OFFSET_SUBCOMP_STRUCT* offset_subcomp = nullptr);
	void parse_circle(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset = nullptr, OFFSET_SUBCOMP_STRUCT* offset_subcomp = nullptr);
	void parse_rect(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, bool is_filled, OFFSET_STRUCT* offset = nullptr, OFFSET_SUBCOMP_STRUCT* offset_subcomp = nullptr);
	void parse_arc(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset = nullptr, OFFSET_SUBCOMP_STRUCT* offset_subcomp = nullptr);
	void parse_wire(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset = nullptr, OFFSET_SUBCOMP_STRUCT* offset_subcomp = nullptr);
	void parse_nn(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset = nullptr, OFFSET_SUBCOMP_STRUCT* offset_subcomp = nullptr);
	void parse_i(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset = nullptr, OFFSET_SUBCOMP_STRUCT* offset_subcomp = nullptr);
	void parse_v(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size, OFFSET_STRUCT* offset = nullptr, OFFSET_SUBCOMP_STRUCT* offset_subcomp = nullptr);
	void parse_compdef(Layer& layer, vector<GraphElement*>& dest_arr, int16_t size);
	void parse_section_into_pic(uint64_t begin_offset, uint64_t end_offset, int layer_index, vector<GraphElement*>& dest_arr, OFFSET_STRUCT* offset_struct_for_subcomp = nullptr, OFFSET_SUBCOMP_STRUCT* offset_subcomp = nullptr);
	
	int64_t get_next_layer_struct_offset(int ly_i);
	string get_ly(int i);
	string get_tj(int i);
	string get_tm(int i);
	string get_type_of_line(int8_t type);
	void set_right_coord(int16_t s_x, int16_t s_y, OFFSET_STRUCT* offset, int16_t& x, int16_t& y);
	void set_right_coord2(int16_t& x, int16_t& y, int8_t Ro);
	void change_coord_depending_on_offset(int16_t& x, int16_t& y, OFFSET_STRUCT* offset, OFFSET_SUBCOMP_STRUCT* offset_subcomp);
	bool is_ascii_code(int8_t c);

	void convert_coords();
	void negativeY(GraphElement* element);
	bool set_begin_coordinate(GraphElement* element);
	void search_begin_coordinates();
	void set_border_coordinate(GraphElement* element, I* i = nullptr);
	void set_border_coordinates();

	void rotatePoint(int& x, int& y, int cx, int cy, int angle);
	GraphElement* rotateFigure(GraphElement* element, int cx, int cy, int angle);
	void insert_into_layer(vector<string>& layer_groups, vector<string>& dotted, vector<string>& dashed, GraphElement* element, N* n = nullptr);
	string generate_comp_def_patterns();
	void insert_instance(vector<string>& layer_groups, vector<string>& dotted, vector<string>& dashed, I* instance);
	string get_svg_setting();
	bool isHidden(string ly);
	string js_script();

	struct SECTIONS_OFFSET {
		uint64_t count_struct = 0x140;
		uint64_t info_section;
		uint64_t layers_section;
		uint64_t offset_struct = 0x896;
		uint64_t offset_sub_comp_struct;
		uint64_t layer_struct;
		uint64_t net_def_struct;
		uint64_t p_struct;
		uint64_t pic_section;
		uint64_t end_file;
	} sections_offset;

	/*#pragma pack(2)*/
	struct COUNTS_STRUCT {
		int16_t version;
		int16_t arr1[13];
		int16_t offset_struct_count;
		int16_t offset_subcomp_struct_count;
		int16_t layer_struct_count;
		int16_t var_1;
		int16_t net_struct_count;
		int16_t p_struct_count;
		int16_t arr2[12];
	} counts;

	/*#pragma pack(2)*/
	struct OFFSET_STRUCT {
		int16_t layer_struct_id; // 0
		int16_t sub_comp_id; // 2
		int16_t pl_x; //4
		int16_t pl_y; //6
		int16_t var4; //8
		int16_t var5; //10
		int8_t var6; //12
		int8_t Ro; //13
		int16_t offset_count_p; //14
		int16_t var8; //16
		int16_t layer_struct_item_id; //18

		void print() {
			cout.setf(ios_base::left);
			cout << hex << setw(7) << layer_struct_id << setw(7) << sub_comp_id << setw(7) << pl_x << setw(7) << pl_y << setw(7) << var4;
			cout << setw(7) << var5 << setw(7) << to_string(var6) << setw(7) << to_string(Ro) << setw(7) << offset_count_p << setw(7) << var8;
			cout << setw(7) << layer_struct_item_id << endl;

			cout.unsetf(ios_base::left);
		}
	};

	/*#pragma pack(2)*/
	struct OFFSET_SUBCOMP_STRUCT {
		int16_t var0; // 0
		int16_t var1; // 2
		int16_t var2; //4
		int8_t var3_0; //5
		int8_t var3_1; //6 // от нее зависит set_right_coord
		int16_t var4; //8
		int16_t var5; //10
		int16_t var6; //12
		int16_t pl_x; //14
		int16_t pl_y; //16
		int16_t id; //18

		void print() {
			cout << hex;
			cout.setf(ios_base::left);
			cout << setw(7) << var0 << setw(7) << var1 << setw(7) << var2;
			/*cout << setw(7) << (int)var3_0 << setw(7) << (int)var3_1 << setw(7) << to_string(var4) << setw(7) << to_string(var5);
			cout << setw(7) << to_string(var6) << setw(7) << to_string(pl_x) << setw(7) << to_string(pl_y);*/
			cout << setw(7) << (int)var3_0 << setw(7) << (int)var3_1 << setw(7) << var4 << setw(7) << var5;
			cout << setw(7) << var6 << setw(7) << pl_x << setw(7) << pl_y;
			cout << setw(7) << id << endl;
			cout.unsetf(ios_base::left);
		}
	};

	
	/*#pragma pack(2)*/
	struct LAYER_STRUCT {
		int16_t var1; // 0
		uint16_t first_element_in_layer_offset; //2
		int16_t count_10000; //4
		int8_t layer_index; //6
		int8_t section_type; //7  
		int16_t var5; //8
		int16_t item_id; //10
		int16_t var7; //12
		int16_t offset_struct_id; //14

		void print(vector<pair<string, string>>& ENVIRONMENT_LYRSTR) {
			cout << hex;
			cout.setf(ios_base::left);
			cout << setw(7) << var1;
			/*cout.unsetf(ios_base::hex);*/ cout << setw(7) << first_element_in_layer_offset << hex << setw(7) << count_10000;
			cout << setw(7);
			if (layer_index >= 0 && layer_index <= 18) cout << ENVIRONMENT_LYRSTR[layer_index].first;
			else cout << "(" + to_string((int)layer_index) + ")";


			cout << setw(7) << (int)section_type << setw(7) << (int)var5;

			cout << setw(7) << item_id << setw(7) << var7 << setw(7) << offset_struct_id << endl;

			cout.unsetf(ios_base::left);
		}
	};

	/*#pragma pack(2)*/
	struct NET_DEF {
		char pin_name[8];
		int16_t var1; //8
		int16_t DG_flag; //10
		int16_t has_pin; //12
		int16_t pin1_x; //14
		int16_t pin1_y; //16
		int16_t pin2_x; //18
		int16_t pin2_y; //20
		int16_t var7; //22
		int16_t layer_struct_id; //24

		void print() {
			cout.unsetf(ios_base::hex);
			cout.setf(ios_base::left);
			cout << setw(10) << string(pin_name);
			cout << hex << setw(7) << var1; cout.unsetf(ios_base::hex);
		    cout << setw(7) << DG_flag << setw(7) << has_pin;
			cout << setw(7) << pin1_x;
			cout << setw(7) << pin1_y << setw(7) << pin2_x;
			cout << setw(7) << pin2_y << setw(7) << hex << var7;
			cout << setw(7) << layer_struct_id;

			cout.unsetf(ios_base::left);
			cout << endl;
		}
	};

	/*#pragma pack(2)*/
	struct SCH_P {
		char pin_name[8];
		int16_t x; // 8
		int16_t y; // 10
		int16_t var5; // 12
		int8_t pt1; // 14;   14 байт - это Pt
		int8_t layer_index; // 14; 
		int8_t lq; // 16;  
		int8_t var0; // 16;
		int32_t var1; // 18;
		int16_t var2; // 22
		int16_t next_p_offset_count; // 24
		int16_t var3; // 26
		int16_t var28; // 28

		P* to_P(vector<pair<string, string>>& ENVIRONMENT_LYRSTR) {
			char r_name[9]; r_name[8] = '\0';
			memmove_s(r_name, 8, pin_name, 8);
			string name;
			for (char s : pin_name) {
				if (s > 32 && s <= 125 && s != 34 && s != 39 && s != 96 && s != 38) name += s;
			}

			return new P(name, get_type(), lq, x, y, Layer(get_layer_name(ENVIRONMENT_LYRSTR, layer_index), "SOLID", 1, 1, 1, "", ""));
		}

		string get_layer_name(vector<pair<string, string>>& ENVIRONMENT_LYRSTR, int i) {
			
			if (i < 0 || i > ENVIRONMENT_LYRSTR.size() - 1) return "";
			return ENVIRONMENT_LYRSTR[i].first;
		}

		string get_type() {
			switch (pt1) {
			case 0: return "INPUT";
			case 1: return  "OUTPUT";
			case 2: return  "I/O";
			case 3: return  "OC";
			case 4: return  "OE";
			case 5: return  "TRI";
			case 6: return  "AN";
			default: return std::to_string((int)pt1) + "";
			}
		}
	};
};

#endif