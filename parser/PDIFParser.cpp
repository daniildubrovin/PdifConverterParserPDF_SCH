#include "PDIFParser.h"
#include "PDIFException.h"
#include "GraphElement.h"

PDIFParser::PDIFParser(std::string ifile) : last_token(token_type::uninitialized),state(states::START)
{
	lexer = new PDIFLexer(ifile);
} 

token_type PDIFParser::next_token()
{
	return last_token = lexer->scan();
}

bool PDIFParser::is_skiping_token()
{
	return read_flags.UnsupportedToken != token_type::uninitialized;
}

void PDIFParser::print_unsupported_message(string file)
{
	string msg = "Warning: Unsupported element " + lexer->get_string_value() +
		"; position: " + std::to_string(lexer->get_position().line_number) +
		"," + std::to_string(lexer->get_position().symbol_number) + "\n";

	if (file != "") {
		ofstream ofs(file,ios::app);
		ofs << msg;
	}

	cout << msg;
}

void PDIFParser::update_flags(token_type token = token_type::uninitialized)
{
	bool flag = token != token_type::uninitialized;
	if (!flag) token = stack.top();

	if (token == token_type::component) read_flags.Component = flag;
	else if (token == token_type::user_literal) read_flags.USER = flag;
	else if (token == token_type::mode) read_flags.Mode = flag;
	else if (token == token_type::lv) read_flags.Lv = flag;
	else if (token == token_type::gs) read_flags.Gs = flag;
	else if (token == token_type::user_literal) read_flags.USER = flag;
	else if (token == token_type::environment) read_flags.ENVIRONMENT = flag;
	else if (token == token_type::pdifrev) read_flags.PDIFvrev = flag;
	else if (token == token_type::program) read_flags.Program = flag;
	else if (token == token_type::dbtype) read_flags.DBtype = flag;
	else if (token == token_type::dbvrev) read_flags.DBvrev = flag;
	else if (token == token_type::dbtime) read_flags.DBtime = flag;
	else if (token == token_type::dbunit) read_flags.DBunit = flag;
	else if (token == token_type::dbgrid) read_flags.DBgrid = flag;
	else if (token == token_type::lyrstr) read_flags.Lyrstr = flag;
	else if (token == token_type::l) read_flags.Line = flag;
	else if (token == token_type::arc) read_flags.Arc = flag;
	else if (token == token_type::c) read_flags.Circle = flag;
	else if (token == token_type::r) read_flags.Rect = flag;
	else if (token == token_type::fr) read_flags.Frect = flag;
	else if (token == token_type::t) read_flags.Text = flag;
	else if (token == token_type::w) read_flags.Wire = flag;
	else if (token == token_type::ly) read_flags.Ly = flag;
	else if (token == token_type::ls) read_flags.Ls = flag;
	else if (token == token_type::wd) read_flags.Wd = flag;
	else if (token == token_type::ts) read_flags.Ts = flag;
	else if (token == token_type::tj) read_flags.Tj = flag;
	else if (token == token_type::tr) read_flags.Tr = flag;
	else if (token == token_type::tm) read_flags.Tm = flag;
	else if (token == token_type::symbol) read_flags.Symbol = flag;
	else if (token == token_type::pin_def) read_flags.PIN_DEF = flag;
	else if (token == token_type::p) read_flags.P = flag;
	else if (token == token_type::pid) read_flags.pid = flag;
	else if (token == token_type::pt) read_flags.Pt = flag;
	else if (token == token_type::lq) read_flags.Lq = flag;
	else if (token == token_type::ploc) read_flags.Ploc = flag;
	else if (token == token_type::pic) read_flags.PIC = flag;
	else if (token == token_type::pkg) read_flags.PKG = flag;
	else if (token == token_type::atr) read_flags.ATR = flag;
	else if (token == token_type::in) read_flags.IN = flag;
	else if (token == token_type::ex) read_flags.EX = flag;
	else if (token == token_type::rdl) read_flags.Rdl = flag;
	else if (token == token_type::pnl) read_flags.Pnl = flag;
	else if (token == token_type::sd) read_flags.Sd = flag;
	else if (token == token_type::org) read_flags.Org = flag;
	else if (token == token_type::ty) read_flags.Ty = flag;
	else if (token == token_type::at) read_flags.At = flag;
	else if (token == token_type::annotate) read_flags.ANNOTATE = flag;
	else if (token == token_type::net_def) read_flags.NET_DEF = flag;
	else if (token == token_type::dg) read_flags.DG = flag;
	else if (token == token_type::n) { read_flags.N = flag; read_flags.N_value = flag; }
	else if (token == token_type::v) read_flags.V = flag;
	else if (token == token_type::nn) read_flags.Nn = flag;
	else if (token == token_type::ns) read_flags.Ns = flag;
	else if (token == token_type::un) read_flags.Un = flag;
	else if (token == token_type::subcomp) read_flags.SUBCOMP = flag;
	else if (token == token_type::comp_def) { read_flags.COMP_DEF = flag; read_flags.COMP_DEF_value = flag; }
	else if (token == token_type::vr) read_flags.Vr = flag;
	else if (token == token_type::i) { read_flags.I = flag; read_flags.I_new = flag; }
	else if (token == token_type::cn) read_flags.CN = flag;
	else if (token == token_type::rd) read_flags.Rd = flag;
	else if (token == token_type::pn) read_flags.Pn = flag;
	else if (token == token_type::pl) read_flags.Pl = flag;
	else if (token == token_type::asg) read_flags.ASG = flag;
	else if (token == token_type::sc) read_flags.Sc = flag;
	else if (token == token_type::ro) read_flags.Ro = flag;
	else if (token == token_type::mr) read_flags.Mr = flag;
	else if (token == token_type::nl) read_flags.Nl = flag;
}

void PDIFParser::parse_user(PDIFType& pdif, string mode)
{
	if (is_skiping_token()) return;

	if (mode == "literal") {
		if (read_flags.USER) {
			if (read_flags.Mode) read_flags.countParametrs = 1;
			else if (read_flags.Gs) read_flags.countParametrs = 2;
			else if (read_flags.Lv) read_flags.countParametrs = pdif.ENVIRONMENT.Lyrstr.size();
		}
	}
	else if (mode == "value") {
		if (read_flags.USER && (read_flags.Mode || read_flags.Gs || read_flags.Lv)) {
			read_flags.countParametrs--;
		}

		if (read_flags.Mode) pdif.USER.mode = lexer->get_string_value();
		else if (read_flags.Gs) {
			if (!read_flags.first_pos) {
				pdif.USER.Gs.x = lexer->get_integer_value();
				read_flags.first_pos = true;
				read_flags.second_pos = false;
			}
			else if (!read_flags.second_pos) {
				pdif.USER.Gs.y = lexer->get_integer_value();
				read_flags.second_pos = true;
				read_flags.first_pos = false;
			}
		}
		else if (read_flags.Lv) {
			pdif.USER.Lv.push_back(lexer->get_integer_value());
		}
	}
	else if (mode == "end") {
		if (read_flags.USER && (read_flags.Mode || read_flags.Gs || read_flags.Lv)) {
			if (read_flags.countParametrs != 0) throw PDIFException(lexer);
		}
	}
	else throw invalid_argument("mode must be either value | literal | end");
}

void PDIFParser::parse_environment(PDIFType& pdif, string mode)
{
	if (is_skiping_token()) return;

	if (mode == "literal") {
		if (read_flags.Component || read_flags.PDIFvrev || read_flags.Program || read_flags.DBtype || read_flags.DBvrev || read_flags.DBtime || read_flags.DBunit || read_flags.DBgrid) read_flags.countParametrs = 1;
		else if (read_flags.Lyrstr) read_flags.countParametrs = 0; 
	}
	else if (mode == "value") {
		if (read_flags.Component || read_flags.PDIFvrev || read_flags.Program || read_flags.DBtype || read_flags.DBvrev || read_flags.DBtime || read_flags.DBunit || read_flags.DBgrid) {
			read_flags.countParametrs--;
		}

		if (read_flags.Component) pdif.component = lexer->get_string_value();
		//if (readFlags.ENVIRONMENT) throw PDIFException(lexer, "no values are expected after environment");
		else if (read_flags.PDIFvrev) pdif.ENVIRONMENT.PDIFvrev = lexer->get_float_value();
		else if (read_flags.Program) pdif.ENVIRONMENT.Program = lexer->get_string_value();
		else if (read_flags.DBtype) pdif.ENVIRONMENT.DBtype = lexer->get_string_value();
		else if (read_flags.DBvrev) pdif.ENVIRONMENT.DBvrev = lexer->get_float_value();
		else if (read_flags.DBtime) pdif.ENVIRONMENT.DBtime = lexer->get_string_value();
		else if (read_flags.DBunit) pdif.ENVIRONMENT.DBunit = lexer->get_string_value();
		else if (read_flags.DBgrid) pdif.ENVIRONMENT.DBgrid = lexer->get_integer_value();
		else if (read_flags.Lyrstr) {
			if (!read_flags.first_pos) {
				string ly = lexer->get_string_value();
				if (ly == "$$NULL") ly = "";
				pdif.ENVIRONMENT.Lyrstr.push_back(pair<string, string>(ly, ""));
				read_flags.countParametrs++;
				read_flags.first_pos = true;
				read_flags.second_pos = false;
			}
			else if (!read_flags.second_pos) {
				pdif.ENVIRONMENT.Lyrstr.back().second = colors[lexer->get_integer_value()];
				read_flags.countParametrs++;
				read_flags.second_pos = true;
				read_flags.first_pos = false;
			}
		}
	}
	else if (mode == "end") {
		if (read_flags.Component || read_flags.PDIFvrev || read_flags.Program || 
			read_flags.DBtype || read_flags.DBvrev || read_flags.DBtime || read_flags.DBunit || read_flags.DBgrid) {
			if (read_flags.countParametrs != 0) throw PDIFException(lexer, "only 1 parameter is possible");
		}
		else if (read_flags.Lyrstr) {
			if (read_flags.countParametrs % 2 != 0) throw PDIFException(lexer);
			read_flags.Lyrstr = false;
		}

		if (read_flags.Component) read_flags.Component = false;
	}
	else throw invalid_argument("mode must be either value | literal | end");
}

void PDIFParser::parse_graphical_elements(PDIFType& pdif, string mode)
{
	if (is_skiping_token()) return;

	if (mode == "literal") {

		read_flags.second_pos = true;
		read_flags.first_pos = false;

		if (read_flags.Line) {
			Line* line = new Line(read_flags.current_layer);
			if(read_flags.Symbol) pdif.SYMBOL.pic.push_back(line);
			else if(read_flags.ANNOTATE) pdif.DETAIL.Annotate.push_back(line);
			else if(read_flags.COMP_DEF) pdif.DETAIL.COMP_DEFS.back()->pic.push_back(line);
			read_flags.countParametrs = 0;
		}
		else if (read_flags.Arc) {
			Arc* arc = new Arc(read_flags.current_layer);
			if (read_flags.Symbol) pdif.SYMBOL.pic.push_back(arc);
			else if (read_flags.ANNOTATE) pdif.DETAIL.Annotate.push_back(arc);
			else if (read_flags.COMP_DEF) pdif.DETAIL.COMP_DEFS.back()->pic.push_back(arc);
			read_flags.countParametrs = 6;
		}
		else if (read_flags.Circle) {
			Circle* circle = new Circle(read_flags.current_layer);
			if (read_flags.Symbol) pdif.SYMBOL.pic.push_back(circle);
			else if (read_flags.ANNOTATE) pdif.DETAIL.Annotate.push_back(circle);
			else if (read_flags.COMP_DEF) pdif.DETAIL.COMP_DEFS.back()->pic.push_back(circle);
			read_flags.countParametrs = 3; 
		}
		else if (read_flags.Rect) {
			Rect* rect = new Rect(read_flags.current_layer);
			if (read_flags.Symbol) pdif.SYMBOL.pic.push_back(rect);
			else if (read_flags.ANNOTATE) pdif.DETAIL.Annotate.push_back(rect);
			else if (read_flags.COMP_DEF) pdif.DETAIL.COMP_DEFS.back()->pic.push_back(rect);
			read_flags.countParametrs = 4; 
		}
		else if (read_flags.Frect) {
			Rect* frect = new Rect(read_flags.current_layer,true);
			if (read_flags.Symbol) pdif.SYMBOL.pic.push_back(frect);
			else if (read_flags.ANNOTATE) pdif.DETAIL.Annotate.push_back(frect);
			else if (read_flags.COMP_DEF) pdif.DETAIL.COMP_DEFS.back()->pic.push_back(frect);
			read_flags.countParametrs = 4; 
		}
		else if (read_flags.Text) {
			Text* text = new Text(read_flags.current_layer);
			if (read_flags.Symbol) pdif.SYMBOL.pic.push_back(text);
			else if (read_flags.ANNOTATE) pdif.DETAIL.Annotate.push_back(text);
			else if (read_flags.COMP_DEF) pdif.DETAIL.COMP_DEFS.back()->pic.push_back(text);
			read_flags.countParametrs = 3; 
		}
		else if (read_flags.Wire) {
			Wire* wire = new Wire(read_flags.current_layer);
			if (read_flags.Symbol) pdif.SYMBOL.pic.push_back(wire);
			else if (read_flags.ANNOTATE) pdif.DETAIL.Annotate.push_back(wire);
			else if (read_flags.N) pdif.DETAIL.NET_DEF.back()->DG.push_back(wire);
			else if (read_flags.COMP_DEF) pdif.DETAIL.COMP_DEFS.back()->pic.push_back(wire);
			read_flags.countParametrs = 0;
		}
		else if (read_flags.Rdl) {
			Rdl* rdl = new Rdl(read_flags.current_layer);
			if (read_flags.PKG) {
				if (read_flags.Symbol) pdif.SYMBOL.rdl = rdl;
				else if (read_flags.COMP_DEF) pdif.DETAIL.COMP_DEFS.back()->rdl = rdl;
			}		
			read_flags.countParametrs = 2;
		}
		else if (read_flags.Pnl) {
			Pnl* pnl = new Pnl(read_flags.current_layer);
			if (read_flags.PKG) {
				if (read_flags.Symbol) pdif.SYMBOL.locations.push_back(pnl);
				else if (read_flags.COMP_DEF) pdif.DETAIL.COMP_DEFS.back()->locations.push_back(pnl);
			}
			read_flags.countParametrs = 2;
		}
		else if (read_flags.Sd) {
			Sd* sd = new Sd(read_flags.current_layer);
			if (read_flags.Symbol) pdif.SYMBOL.sections.push_back(sd);
			else if (read_flags.COMP_DEF) pdif.DETAIL.COMP_DEFS.back()->sections.push_back(sd);
			read_flags.countParametrs = 0;
		}
		else if (read_flags.At) {
			At* at = new At(read_flags.current_layer);
			if (read_flags.ATR) {
				if (read_flags.Symbol) pdif.SYMBOL.ex.push_back(at);
				else if (read_flags.I) pdif.DETAIL.instances.back()->ex.push_back(at);
			}	
			read_flags.countParametrs = 4;
		}
		else if (read_flags.V) {
			V* v = new V(read_flags.current_layer);
			if (read_flags.N) pdif.DETAIL.NET_DEF.back()->DG.push_back(v);
			read_flags.countParametrs = 3;
		}
		else if (read_flags.Nn) {
			Nn* nn = new Nn(read_flags.current_layer);
			if (read_flags.N) pdif.DETAIL.NET_DEF.back()->DG.push_back(nn);
			read_flags.countParametrs = 0;
		}
		else if (read_flags.Rd) {
			Rd* rd = new Rd(read_flags.current_layer);
			if (read_flags.I) pdif.DETAIL.instances.back()->asg.push_back(rd);
			read_flags.countParametrs = 3;
		}
		else if (read_flags.Pn) {
			Pn* pn = new Pn(read_flags.current_layer);
			if (read_flags.I) pdif.DETAIL.instances.back()->asg.push_back(pn);
			read_flags.countParametrs = 3;
		}
	}
	else if (mode == "value") {
		GraphElement* last = nullptr;

		if (read_flags.Line || read_flags.Wire || read_flags.Arc || read_flags.Circle || read_flags.Rect || read_flags.Frect ||
			read_flags.Text || read_flags.V || read_flags.At || read_flags.Nn ||
			read_flags.Rd || read_flags.Pn || read_flags.Pnl || read_flags.Rdl || read_flags.Sd) {

			if (read_flags.Symbol) {
				if (read_flags.PIC) last = pdif.SYMBOL.pic.back();
				else if (read_flags.PIN_DEF) last = pdif.SYMBOL.PIN_DEF.back();
				else if (read_flags.PKG && read_flags.Pnl) last = pdif.SYMBOL.locations.back();
				else if (read_flags.PKG && read_flags.Sd) last = pdif.SYMBOL.sections.back();
			}
			else if (read_flags.ANNOTATE) last = pdif.DETAIL.Annotate.back();
			else if (read_flags.N) last = pdif.DETAIL.NET_DEF.back()->DG.back();
			else if (read_flags.SUBCOMP) {
				if (read_flags.COMP_DEF) {
					if (read_flags.PIC) last = pdif.DETAIL.COMP_DEFS.back()->pic.back();
					if (read_flags.Pnl) last = pdif.DETAIL.COMP_DEFS.back()->locations.back();
					else if (read_flags.Sd) last = pdif.DETAIL.COMP_DEFS.back()->sections.back();
				}			
				else if (read_flags.I) {
					if (read_flags.ASG) last = pdif.DETAIL.instances.back()->asg.back();
					else if (read_flags.ATR) last = pdif.DETAIL.instances.back()->ex.back();
				}
			}
		}

		if (read_flags.Line) { 
			Line* line = (Line*)last;
			if (!read_flags.first_pos) {
				line->coordinates.push_back(pair<int,int>(lexer->get_integer_value(),0));
				read_flags.first_pos = true;
				read_flags.second_pos = false;
			}
			else if (!read_flags.second_pos) {
				line->coordinates.back().second = lexer->get_integer_value();
				read_flags.second_pos = true;
				read_flags.first_pos = false;
			}
			read_flags.countParametrs++;
		}
		else if (read_flags.Arc) { 
			Arc* arc = (Arc*)last;
			if (read_flags.countParametrs == 6) arc->x = lexer->get_integer_value();
			else if (read_flags.countParametrs == 5) arc->y = lexer->get_integer_value();
			else if (read_flags.countParametrs == 4) arc->rx = lexer->get_integer_value();
			else if (read_flags.countParametrs == 3) arc->ry = lexer->get_integer_value();
			else if (read_flags.countParametrs == 2) arc->ex = lexer->get_integer_value();
			else if (read_flags.countParametrs == 1) arc->ey = lexer->get_integer_value();
			read_flags.countParametrs--;
		}
		else if (read_flags.Circle) { 
			Circle* circle = (Circle*)last;
			if (read_flags.countParametrs == 3) circle->x = lexer->get_integer_value();
			else if (read_flags.countParametrs == 2) circle->y = lexer->get_integer_value();
			else if (read_flags.countParametrs == 1) circle->r = lexer->get_integer_value();
			read_flags.countParametrs--;
		}
		else if (read_flags.Rect || read_flags.Frect) { 
			Rect* rect = (Rect*)last;
			if (read_flags.countParametrs == 4) rect->x1 = lexer->get_integer_value();
			else if (read_flags.countParametrs == 3) rect->y1 = lexer->get_integer_value();
			else if (read_flags.countParametrs == 2) rect->x2 = lexer->get_integer_value();
			else if (read_flags.countParametrs == 1) rect->y2 = lexer->get_integer_value();
			read_flags.countParametrs--;
		}
		else if (read_flags.Text) {
			Text* text = (Text*)last;
			if (read_flags.countParametrs == 3) text->text = lexer->get_string_value();
			else if (read_flags.countParametrs == 2) text->x = lexer->get_integer_value();
			else if (read_flags.countParametrs == 1) text->y = lexer->get_integer_value();
			read_flags.countParametrs--;
		}
		else if (read_flags.Wire) {
			Wire* wire = (Wire*)last;
			if (!read_flags.first_pos) {
				wire->coordinates.push_back(pair<int, int>(lexer->get_integer_value(), 0));
				read_flags.countParametrs++;
				read_flags.first_pos = true;
				read_flags.second_pos = false;
			}
			else if (!read_flags.second_pos) {
				wire->coordinates.back().second = lexer->get_integer_value();
				read_flags.countParametrs++;
				read_flags.second_pos = true;
				read_flags.first_pos = false;
			}
		}
		else if (read_flags.Rdl) {
			if (read_flags.countParametrs == 2) {
				if (read_flags.Symbol && read_flags.PKG) pdif.SYMBOL.rdl->x = lexer->get_integer_value();
				else if (read_flags.COMP_DEF && read_flags.PKG) pdif.DETAIL.COMP_DEFS.back()->rdl->x = lexer->get_integer_value();
			}
			else if (read_flags.countParametrs == 1) {
				if (read_flags.Symbol && read_flags.PKG) pdif.SYMBOL.rdl->y = lexer->get_integer_value();
				else if (read_flags.COMP_DEF && read_flags.PKG) pdif.DETAIL.COMP_DEFS.back()->rdl->y = lexer->get_integer_value();
			}
			read_flags.countParametrs--;
		}
		else if (read_flags.Pnl) {
			Pnl* pnl = (Pnl*)last;
			if (read_flags.countParametrs == 2) pnl->x = lexer->get_integer_value();
			else if (read_flags.countParametrs == 1) pnl->y = lexer->get_integer_value();
			read_flags.countParametrs--;
		}
		/*else if (read_flags.Sd) {
			Sd* sd = (Sd*)last;
			if (read_flags.countParametrs == 2) {
				sd->section = lexer->get_string_value();
			}
			else {
				if (!read_flags.first_pos) {
					sd->pin_numbers.push_back(lexer->get_integer_value());
					read_flags.first_pos = true;
					read_flags.second_pos = false;
				}
				else if (!read_flags.second_pos) {
					sd->pin_numbers.push_back(lexer->get_integer_value());
					read_flags.first_pos = false;
					read_flags.second_pos = true;
				}
			}	
			read_flags.countParametrs--;
		}*/
		else if (read_flags.Sd) {
			Sd* sd = (Sd*)last;
			if (read_flags.countParametrs == 0) {
				sd->section = lexer->get_string_value();
			}
			else {
				sd->pin_numbers.push_back(lexer->get_string_value());
			}
			read_flags.countParametrs++;
		}
		else if (read_flags.At) {
			At* at = (At*)last;
			if (read_flags.countParametrs == 4) at->key = lexer->get_string_value();
			else if (read_flags.countParametrs == 3) at->value = lexer->get_string_value();
			else if (read_flags.countParametrs == 2) at->x = lexer->get_integer_value();
			else if (read_flags.countParametrs == 1) at->y = lexer->get_integer_value();
			read_flags.countParametrs--;
		}
		else if (read_flags.V) {
			V* v = (V*)last;
			if (read_flags.countParametrs == 3) v->x = lexer->get_integer_value();
			else if (read_flags.countParametrs == 2) v->y = lexer->get_integer_value();
			else if (read_flags.countParametrs == 1) v->via_type = lexer->get_integer_value();
			read_flags.countParametrs--;
		}
		else if (read_flags.Nn) {
			Nn* nn = (Nn*)last;
			if (!read_flags.first_pos) {
				nn->coordinates.push_back(pair<int, int>(lexer->get_integer_value(), 0));
				read_flags.countParametrs++;
				read_flags.first_pos = true;
				read_flags.second_pos = false;
			}
			else if (!read_flags.second_pos) {
				nn->coordinates.back().second = lexer->get_integer_value();
				read_flags.countParametrs++;
				read_flags.second_pos = true;
				read_flags.first_pos = false;
			}
		}
		else if (read_flags.Rd) {
			Rd* rd = (Rd*)last;
			if (read_flags.countParametrs == 3) rd->rd = lexer->get_string_value();
			else if (read_flags.countParametrs == 2) rd->x = lexer->get_integer_value();
			else if (read_flags.countParametrs == 1) rd->y = lexer->get_integer_value();
			read_flags.countParametrs--;
		}
		else if (read_flags.Pn) {
			Pn* pn = (Pn*)last;
			if (read_flags.countParametrs == 3) pn->n = lexer->get_string_value();
			else if (read_flags.countParametrs == 2) pn->x = lexer->get_integer_value();
			else if (read_flags.countParametrs == 1) pn->y = lexer->get_integer_value();
			read_flags.countParametrs--;
		}
	}
	else if (mode == "end") {
		if (read_flags.Arc || read_flags.Circle || read_flags.Rect || read_flags.Frect || read_flags.Text || read_flags.V || read_flags.At || 
			read_flags.Rd || read_flags.Pn || read_flags.Pnl || read_flags.Rdl) {
			if (read_flags.countParametrs != 0) throw PDIFException(lexer);
		}
		else if (read_flags.Sd) {
			if (read_flags.countParametrs < 2) throw PDIFException(lexer);
		}
		else if (read_flags.Line || read_flags.Wire || read_flags.Nn) {
			if (read_flags.countParametrs % 2 != 0) throw PDIFException(lexer);
		}
	}
	else throw invalid_argument("mode must be either value | literal | end");
}

void PDIFParser::parse_layer(PDIFType& pdif, string mode)
{
	if (is_skiping_token()) return;

	if (mode == "literal") {
		if (read_flags.Ly || read_flags.Ls || read_flags.Wd || read_flags.Ts || read_flags.Tj || read_flags.Tr || read_flags.Tm) {
			read_flags.countParametrs = 1;
		}
	}
	else if (mode == "value"){
		if (read_flags.Ly || read_flags.Ls || read_flags.Wd || read_flags.Ts || read_flags.Tj || read_flags.Tr || read_flags.Tm) {
			read_flags.countParametrs--;
		}
		if (read_flags.Ly) {  
			bool flag_ly = false;
			for (pair<string, string> p : pdif.ENVIRONMENT.Lyrstr) {
				if (p.first == lexer->get_string_value()) {
					read_flags.current_layer.Ly = lexer->get_string_value();
					read_flags.current_layer.color = p.second;
					flag_ly = true;
					break;
				}
			}
			if (!flag_ly) throw PDIFException(lexer,"unexpected layer : " + lexer->get_string_value());
		}
		else if (read_flags.Ls) read_flags.current_layer.Ls = lexer->get_string_value();
		else if (read_flags.Wd) read_flags.current_layer.Wd = lexer->get_integer_value();
		else if (read_flags.Ts) read_flags.current_layer.Ts = lexer->get_integer_value();
		else if (read_flags.Tj) read_flags.current_layer.Tj = lexer->get_string_value();
		else if (read_flags.Tr) read_flags.current_layer.Tr = lexer->get_integer_value();
		else if (read_flags.Tm) read_flags.current_layer.Tm = lexer->get_string_value();
	}
	else if (mode == "end") {
		if (read_flags.Ly || read_flags.Ls || read_flags.Wd || read_flags.Ts || read_flags.Tj || read_flags.Tr || read_flags.Tm) {
			if (read_flags.countParametrs != 0) throw PDIFException(lexer);
		}
	}
	else throw invalid_argument("mode must be either value | literal | end");
}

void PDIFParser::parse_pin_def(PDIFType& pdif, string mode)
{
	if (is_skiping_token() || !read_flags.PIN_DEF) return;

	if (mode == "literal") {
		if (read_flags.Pt) read_flags.countParametrs = 1;
		else if (read_flags.Lq) read_flags.countParametrs = 1;
		else if (read_flags.Ploc) read_flags.countParametrs = 2;
		else if (read_flags.P) {
			P* p = new P(read_flags.current_layer);
			if (read_flags.Symbol) pdif.SYMBOL.PIN_DEF.push_back(p);
			else if (read_flags.COMP_DEF) pdif.DETAIL.COMP_DEFS.back()->PIN_DEF.push_back(p);
			read_flags.countParametrs = 1;
		}
	}
	else if (mode == "value") {
		P* p = nullptr;
		if (read_flags.Symbol) p = pdif.SYMBOL.PIN_DEF.back();
		else if (read_flags.COMP_DEF) p = pdif.DETAIL.COMP_DEFS.back()->PIN_DEF.back();

		if (read_flags.Pt || read_flags.Lq) read_flags.countParametrs--;

		if (read_flags.Pt) p->type = lexer->get_string_value();
		else if (read_flags.Lq) p->lq = lexer->get_integer_value();
		else if (read_flags.Ploc) {
			if (read_flags.countParametrs == 2) p->x = lexer->get_integer_value();
			else if (read_flags.countParametrs == 1) p->y = lexer->get_integer_value();
			read_flags.countParametrs--;
		}
		else if (read_flags.P) { p->pin_name = lexer->get_string_value(); read_flags.countParametrs--; }
	}
	else if (mode == "end") {
		if (read_flags.Pt || read_flags.Lq || read_flags.Ploc) {
			if (read_flags.countParametrs != 0) throw PDIFException(lexer);
		}
		else if (read_flags.P) {
			if (read_flags.countParametrs != 0) throw PDIFException(lexer);
		}
	}
	else throw invalid_argument("mode must be either value | literal | end");
}

void PDIFParser::parse_symbol(PDIFType& pdif, string mode)
{
	if (is_skiping_token() || !read_flags.Symbol) return;

	if (mode == "literal") {
		if (read_flags.Org) read_flags.countParametrs = 2;
		else if (read_flags.Ty) read_flags.countParametrs = 1;
		else if (read_flags.pid) read_flags.countParametrs = 1;
		parse_pin_def(pdif, mode);
	}
	else if (mode == "value") {
		if (read_flags.Org) {
			if (read_flags.countParametrs == 2) pdif.SYMBOL.org.x = lexer->get_integer_value();
			else if (read_flags.countParametrs == 1) pdif.SYMBOL.org.y = lexer->get_integer_value();
			read_flags.countParametrs--;
		}
		else if (read_flags.Ty) {
			if (read_flags.countParametrs == 1) pdif.SYMBOL.ty = lexer->get_integer_value();
			read_flags.countParametrs--;
		}
		else if (read_flags.pid) {
			if (read_flags.countParametrs == 1) pdif.SYMBOL.pid = lexer->get_string_value();
			read_flags.countParametrs--;
		}
		parse_pin_def(pdif, mode);
	}
	else if (mode == "end") {
		if (read_flags.Org || read_flags.Ty || read_flags.At || read_flags.pid) {
			if (read_flags.countParametrs != 0) throw PDIFException(lexer);
		}	
		parse_pin_def(pdif, mode);
	}
	else throw invalid_argument("mode must be either value | literal | end");
}

void PDIFParser::parse_nef_def(PDIFType& pdif, string mode)
{
	if (is_skiping_token() || !read_flags.NET_DEF) return;

	if (mode == "literal") {
		if (read_flags.Un) read_flags.countParametrs = 1;
		else if (read_flags.Ns) read_flags.countParametrs = 1;
		else if (read_flags.N_value) { 
			read_flags.countParametrs = 1; 
			pdif.DETAIL.NET_DEF.push_back(new N());
		}
	}
	else if (mode == "value") {
		if(read_flags.Ns || read_flags.Un) read_flags.countParametrs--;

		if (read_flags.Ns) pdif.DETAIL.NET_DEF.back()->Ns = lexer->get_string_value();
		else if (read_flags.Un) pdif.DETAIL.NET_DEF.back()->Un = lexer->get_string_value();
		else if (read_flags.N_value) {
			pdif.DETAIL.NET_DEF.back()->net_name = lexer->get_string_value(); 
			read_flags.countParametrs--;
		}
	}
	else if (mode == "end") {
		if (read_flags.Ns || read_flags.Un || read_flags.N_value) {
			if (read_flags.countParametrs != 0) throw PDIFException(lexer);
		}

		if (read_flags.N_value) read_flags.N_value = false;
	}
	else throw invalid_argument("mode must be either value | literal | end");
}

void PDIFParser::parse_comp_def(PDIFType& pdif, string mode)
{
	if (is_skiping_token() || !read_flags.COMP_DEF) return;

	if (mode == "literal") {
		if (read_flags.Org) read_flags.countParametrs = 2;
		else if (read_flags.Ty) read_flags.countParametrs = 1;
		else if (read_flags.Vr) read_flags.countParametrs = 1;
		else if (read_flags.pid) read_flags.countParametrs = 1;
		else if (read_flags.COMP_DEF_value) { read_flags.countParametrs = 1; pdif.DETAIL.COMP_DEFS.push_back(new COMP_DEF()); }
		parse_pin_def(pdif, mode);
	}
	else if (mode == "value") {
		if (read_flags.Ty || read_flags.Vr || read_flags.pid) read_flags.countParametrs--;
	
		if (read_flags.Org) {
			if(read_flags.countParametrs == 2) pdif.DETAIL.COMP_DEFS.back()->org.x = lexer->get_integer_value();
			else if(read_flags.countParametrs == 1) pdif.DETAIL.COMP_DEFS.back()->org.y = lexer->get_integer_value();
		}
		else if (read_flags.Ty) pdif.DETAIL.COMP_DEFS.back()->Ty = lexer->get_integer_value();
		else if (read_flags.Vr) pdif.DETAIL.COMP_DEFS.back()->Vr = lexer->get_integer_value();
		else if (read_flags.pid) pdif.DETAIL.COMP_DEFS.back()->pid = lexer->get_string_value();
		else if (read_flags.COMP_DEF_value) { pdif.DETAIL.COMP_DEFS.back()->file_name = lexer->get_string_value(); read_flags.countParametrs--; }
		parse_pin_def(pdif, mode);
	}
	else if (mode == "end") {
		if (read_flags.Org || read_flags.Ty || read_flags.Vr || read_flags.COMP_DEF_value || read_flags.pid) {
			if (read_flags.countParametrs != 0) throw PDIFException(lexer);
		}

		if (read_flags.COMP_DEF_value) read_flags.COMP_DEF_value = false;

		parse_pin_def(pdif, mode);
	}
	else throw invalid_argument("mode must be either value | literal | end");
}

void PDIFParser::parse_i(PDIFType& pdif, string mode)
{
	if (is_skiping_token() || !read_flags.I) return;

	if (mode == "literal") {
		if (read_flags.CN) read_flags.countParametrs = 0;
		else if (read_flags.ASG) read_flags.countParametrs = 1;
		else if (read_flags.Pl) read_flags.countParametrs = 2;
		else if (read_flags.Sc) read_flags.countParametrs = 2;
		else if (read_flags.Ro) read_flags.countParametrs = 1;
		else if (read_flags.Mr) read_flags.countParametrs = 1;
		else if (read_flags.Nl) read_flags.countParametrs = 2;
		else if (read_flags.I_new) { read_flags.countParametrs = 2; pdif.DETAIL.instances.push_back(new I()); }
	}
	else if (mode == "value") {
		I* i = nullptr;
		if (read_flags.I_new || read_flags.Pl || read_flags.ASG || read_flags.Sc || read_flags.Ro || read_flags.Mr || read_flags.Nl || read_flags.Sc || read_flags.CN) {
			i = pdif.DETAIL.instances.back();
			if(read_flags.Ro || read_flags.Mr) read_flags.countParametrs--;
		}
			
		if (read_flags.CN) {
			if (lexer->get_string_value() != "?") {
				read_flags.countParametrs++;
				for (N* n : pdif.DETAIL.NET_DEF) {
					if (n->net_name == lexer->get_string_value()) {
						i->CN.push_back(n);
						break;
					}
				}
			}
			else {
				i->comp_pin_def.push_back(i->comp_def->PIN_DEF[read_flags.countParametrs]);
				read_flags.countParametrs++;
			}			
		}
		else if (read_flags.ASG && !read_flags.Rd && !read_flags.Pn){
			i->asg_name = lexer->get_string_value();
			read_flags.countParametrs--;
		}
		else if (read_flags.Pl) {
			if (read_flags.countParametrs == 2) {
				i->Pl.x = lexer->get_integer_value();
			}
			else if (read_flags.countParametrs == 1) {
				i->Pl.y = lexer->get_integer_value();
			}
			read_flags.countParametrs--;
		}
		else if (read_flags.Sc) {
			if (read_flags.countParametrs == 2) {
				i->Sc.sx = lexer->get_integer_value();
			}
			else if (read_flags.countParametrs == 1) {
				i->Sc.sy = lexer->get_integer_value();
			}
			read_flags.countParametrs--;
		}
		else if (read_flags.Ro) i->Ro = lexer->get_integer_value();
		else if (read_flags.Mr) i->Mr = lexer->get_string_value();
		else if (read_flags.Nl) {
			if (read_flags.countParametrs == 2) {
				i->Nl.x = lexer->get_integer_value();
			}
			else if (read_flags.countParametrs == 1) {
				i->Nl.y = lexer->get_integer_value();
			}
			read_flags.countParametrs--;
		}
		else if (read_flags.I_new) {
			if (read_flags.countParametrs == 2) {
				i->file_name = lexer->get_string_value();
				if(pdif.DETAIL.COMP_DEFS.back()->file_name == i->file_name) i->comp_def = pdif.DETAIL.COMP_DEFS.back();
				else {
					bool comp_defs_flag = false;
					for (COMP_DEF* comp_def : pdif.DETAIL.COMP_DEFS) {
						if (comp_def->file_name == i->file_name) {
							i->comp_def = comp_def;
							comp_defs_flag = true;
							break;
						}
					}
					if (!comp_defs_flag) throw PDIFException(lexer,"no comp_def was found for I");
				}
			}
			else if (read_flags.countParametrs == 1) {
				i->inv_name = lexer->get_string_value();
			}
			read_flags.countParametrs--;
		}
	}
	else if (mode == "end") {
		if (read_flags.Pl || read_flags.ASG || read_flags.Sc || read_flags.Ro || read_flags.Mr || read_flags.Nl || read_flags.I_new) {
			if (read_flags.countParametrs != 0) throw PDIFException(lexer);
		}
		else if (read_flags.Sc) {
			if (read_flags.countParametrs == 0) throw PDIFException(lexer);
		}

		if (read_flags.I_new) read_flags.I_new = false;
	}
	else throw invalid_argument("mode must be either value | literal | end");
}

PDIFType PDIFParser::parse()
{
	PDIFType pdif;
		
	while (true) {

		next_token();
		//FINISH
		if (state == states::FINISH) {
			if (stack.empty()) return pdif;
			else throw PDIFException(lexer);
		}
		//START
		else if (state == states::START) {
			if (last_token == token_type::begin_object) state = states::BEGIN_OBJECT;
			else throw PDIFException(lexer);
		}	
		//BEGIN_OBJECT
		else if (state == states::BEGIN_OBJECT) {
			stack.push(last_token);

			if (lexer->is_key_words(last_token)) {
				state = states::LITERALS;
				update_flags(last_token);

				parse_environment(pdif,"literal");
				parse_user(pdif,"literal");
				parse_symbol(pdif, "literal");
				parse_nef_def(pdif, "literal");
				parse_comp_def(pdif, "literal");
				parse_i(pdif, "literal");
				parse_graphical_elements(pdif,"literal");				
			}
			else if (last_token == token_type::value_string) {
				state = states::LITERALS;

				read_flags.UnsupportedToken = last_token;
				print_unsupported_message("input.txt");
			}
			else throw PDIFException(lexer);
		}
		//LITERALS
		else if (state == states::LITERALS) {
			if (last_token == token_type::value_string || last_token == token_type::value_integer || last_token == token_type::value_float || lexer->is_key_words(last_token)) {
				state = states::VALUE;

				parse_environment(pdif, "value");
				parse_user(pdif, "value");
				parse_symbol(pdif, "value");
				parse_layer(pdif, "value");
				parse_nef_def(pdif, "value");
				parse_comp_def(pdif, "value");
				parse_i(pdif, "value");
				parse_graphical_elements(pdif, "value");
			}
			else if (last_token == token_type::end_object) {
				state = states::END_OBJECT;

				parse_environment(pdif, "end");
				parse_user(pdif, "end");
				parse_symbol(pdif, "end");		
				parse_nef_def(pdif, "end");
				parse_comp_def(pdif, "end");
				parse_i(pdif, "end");
				parse_graphical_elements(pdif, "end");
			}
			else if (last_token == token_type::begin_object) state = states::BEGIN_OBJECT;
			else if (last_token == token_type::begin_array) state = states::BEGIN_ARRAY;	
			else throw PDIFException(lexer);
		}
		//BEGIN_ARRAY
		else if (state == states::BEGIN_ARRAY) {
			stack.push(last_token);
			if (lexer->is_key_words(last_token)) { 
				state = states::LITERALS_ARRAY;

				update_flags(last_token);
				parse_layer(pdif, "literal");
			}
			else if (last_token == token_type::value_string) {
				state = states::LITERALS_ARRAY;

				read_flags.UnsupportedToken = last_token;
				print_unsupported_message("input.txt");
			}
			else throw PDIFException(lexer);
		}
		//LITERALS_ARRAY
		else if (state == states::LITERALS_ARRAY) {
			if (last_token == token_type::value_string || last_token == token_type::value_integer || last_token == token_type::value_float || lexer->is_key_words(last_token)) {
				state = states::VALUE_ARRAY;

				parse_layer(pdif, "value");
			}
			else throw PDIFException(lexer);
		}
		//VALUE_ARRAY
		else if (state == states::VALUE_ARRAY) {
			if (last_token == token_type::end_array) { 
				state = states::END_ARRAY; 

				parse_layer(pdif, "end");
			}
			else throw PDIFException(lexer);
		}
		//VALUE
		else if (state == states::VALUE) {
			if (last_token == token_type::value_string || last_token == token_type::value_integer || last_token == token_type::value_float || lexer->is_key_words(last_token)) {
				state = states::VALUE;

				parse_environment(pdif, "value");
				parse_user(pdif, "value");
				parse_symbol(pdif, "value");		
				parse_nef_def(pdif, "value");
				parse_comp_def(pdif, "value");
				parse_i(pdif, "value");
				parse_graphical_elements(pdif, "value");
			}
			else if (last_token == token_type::end_object) { 
				state = states::END_OBJECT;

				parse_environment(pdif, "end");
				parse_user(pdif, "end");
				parse_symbol(pdif, "end");
				parse_nef_def(pdif, "end");
				parse_comp_def(pdif, "end");
				parse_i(pdif, "end");
				parse_graphical_elements(pdif, "end");
			}
			else if (last_token == token_type::begin_object) { 
				state = states::BEGIN_OBJECT; 

				parse_environment(pdif, "end");
				parse_user(pdif, "end");
				parse_symbol(pdif, "end");
				parse_nef_def(pdif, "end");
				parse_comp_def(pdif, "end");
				parse_i(pdif, "end");
			}
			else if (last_token == token_type::begin_array) state = states::BEGIN_ARRAY;
			else throw PDIFException(lexer);
		}
		//END_OBJECT
		else if (state == states::END_OBJECT) {
			if (stack.empty()) throw PDIFException(lexer);
			if (stack.top() == read_flags.UnsupportedToken) read_flags.UnsupportedToken = token_type::uninitialized;
			update_flags();
			stack.pop();

			if (last_token == token_type::begin_object) state = states::BEGIN_OBJECT; 				
			else if (last_token == token_type::begin_array) state = states::BEGIN_ARRAY;
			else if (last_token == token_type::end_object) state = states::END_OBJECT;
			else if (last_token == token_type::end_of_input) state = states::FINISH;
			else throw PDIFException(lexer);
		}
		//END_ARRAY
		else if (state == states::END_ARRAY) {
			if (stack.empty()) throw PDIFException(lexer);
			if (stack.top() == read_flags.UnsupportedToken) read_flags.UnsupportedToken = token_type::uninitialized;
			update_flags();
			stack.pop();

			if (last_token == token_type::begin_array) state = states::BEGIN_ARRAY;
			else if (last_token == token_type::begin_object) state = states::BEGIN_OBJECT;
			else if (last_token == token_type::end_object) state = states::END_OBJECT;
			else throw PDIFException(lexer);
		}
	}

	throw PDIFException(lexer);

	return pdif;
}