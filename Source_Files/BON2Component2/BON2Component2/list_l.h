/*

#ifndef list_l_h
#define list_l_h

#include "stdafx.h"
#include "ParseGraph.h"

using namespace parse;


namespace list_l_scheduling{

	class list_l :public ParseGraph {

	public:
	int total_listl_states;

	public:
		list_l();
		~list_l();

	public:
		void find_listl_label(int startnode, int endnode);
		int find_listl_states(int startnode, int endnode, int state_no_listl, int mul, int add, int log);
		void print_listl_verilog();
	};


};


#endif 


*/