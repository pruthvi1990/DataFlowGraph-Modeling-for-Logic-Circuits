
#ifndef ParseGraph_h
#define ParseGraph_h


#include<string>
#include<vector>
#include<list>
#include <BON2Component.h>


using namespace BON;

namespace parse{

class ParseGraph{

public:
	int total_listl_states;
	int total_listr_states;
	int array_num;
	int latency_sel;
	int num_mul;
	int num_add;
	int num_logical;

	std::list<int> *mul_qty_global;
	std::list<int> *add_qty_global;
	std::list<int> *log_qty_global;

	struct gate_node{
	std::string node_name;
	std::string atom_type;
	std::string src_type1;
	std::string src_type2;
	int node_num;
	std::string input1;
	std::string input2;
	int status;
	int cdfg_start_time;
	int cdfg_end_time;
	int listl_lable;
	int alap_start_time;
	int alap_end_time;
	int state_no_listr;
	bool listr_sch_status;
	int slack_time;
	int listr_start_time;
	int listr_end_time;
	int listr_jump_state;
	bool listr_jmpst_flag;
	int state_no_listl;
	bool listl_sch_status;
	int listl_start_time;
	int listl_end_time;
	int listl_jump_state;
	bool listl_jmpst_flag;
	int listl_label;
	bool post_dummy_state_list_lr;
	};
	std::vector < struct gate_node > node;

	public:
	ParseGraph();
	~ParseGraph(); 

	public:
	void find_attributes(Project &project);
	void fill_attributes(std::string atom_name, Model &model);
	void CDGF_Schedule(int startnode, int endnode);
	void find_listl_label(int startnode, int endnode);
	int find_listl_states(int startnode, int endnode, int state_no_listl, int mul, int add, int log);
	void print_listl_verilog(Model &model);
	void alap_schedule(int startnode, int endnode, int latency);
	int find_listr_states(int startnode,int endnode, int state_no_listr, int latency);
	void print_listr_verilog(Model &model);
	void test_print();

};  


};

#endif