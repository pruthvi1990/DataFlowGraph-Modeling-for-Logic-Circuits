#include "stdafx.h"
#include<list_l.h>
#include<list>
#include<sstream>
#include<fstream>
#include<Console.h>


using namespace std;
using namespace GMEConsole;

/*
namespace list_l_scheduling{

//###############################################################################################################################################
//
// 	C L A S S : list_l_scheduling::list_l
//
//###############################################################################################################################################
	list_l::list_l(){

	}

	list_l::~list_l(){

	}

	void list_l::find_listl_label(int startnode, int endnode){
		std::stringstream stream;
		stream << "start node " << startnode;
		Console::Out::WriteLine(CString(stream.str().c_str()));
		stream << "vector size" << node.size();
		Console::Out::WriteLine(CString(stream.str().c_str()));

		for (int i = startnode; i < endnode; i++)
		{
			ParseGraph::node[i].listl_lable = 1;
		}

	/*Get the dependency for each of the nodes*/
/*		std::list<int> *dep = new list<int>[100];
		for (int i = startnode; i < endnode; i++){
			for (int j = startnode; j < endnode; j++){

				if ((node[i].node_name == node[j].input1) || (node[i].node_name == node[j].input2)){
					dep[i].push_back(j);
				}
			}
		} //end for


		/*label each of the node to schedule with the first of the nodes being schedule will have the highest label*/
	/*	for (int i = endnode; i >= startnode; i--)
		{
			std::list<int>::iterator j;
			for (j = dep[i].begin(); j != dep[i].end(); ++j)
			{
				if (node[*j].listl_lable <= node[i].listl_lable)
				{
					node[*j].listl_lable = node[i].listl_lable + 1;
				}
			}
		} 
	
	
	} //end of find_listl_lable

	int list_l::find_listl_states(int startnode, int endnode, int state_no_listl, int mul, int add, int log){

		int latency = 1;
		int infinity = 1000;
		int buffer_mul = mul;
		int buffer_add = add;
		int buffer_log = log;
		int max_mul_label = 1;
		int max_add_label = 1;
		int max_log_label = 1;
		int max_mul_node;
		int max_add_node;
		int max_log_node;
		bool all_scheduled = 0;
		bool sch_flag = 0;

		//-------------------------Finding depenancy list---------------------//
		list<int> *dep = new list<int>[100];
		for (int i = startnode; i <= endnode; i++)
		{
			for (int j = startnode; j <= endnode; j++)
			{
				if (j == i) continue;
				//if (!strcmp(node[i].ip1, node[j].op) || !strcmp(node[i].ip2, node[j].op) || !strcmp(node[i].ip3, node[j].op))
				if ((node[i].node_name == node[j].input1) || (node[i].node_name == node[j].input2))
				{
					dep[i].push_back(j);
				}
			}
		}
		//-------------------------End Finding depenancy list---------------------//


		//-------------------------Initializing schedule status---------------------//
		for (int j = startnode; j <= endnode; j++)
		{
			node[j].listl_sch_status = 0;
			node[j].post_dummy_state_list_lr = 0;
			node[j].listl_end_time = infinity;
		}

		//-------------------------End Initializing schedule status---------------------//

		list<int> *U_mul = new list<int>[1];
		list<int> *U_add = new list<int>[1];
		list<int> *U_log = new list<int>[1];
		list<int> *U_reg = new list<int>[1];


		while (!all_scheduled)
		{

			//-------------------------- Start Fining U array------------------------------------------//
			for (int k = startnode; k <= endnode; k++)
			{
				int schedule = 1;
				if (node[k].listl_sch_status == 0)
				{
					if (node[k].cdfg_start_time <= latency)
					{
						//if (!strcmp(node[k].opr1, "*"))
						if (node[k].node_name == "mul")
						{
							list<int>::iterator u;
							for (u = dep[k].begin(); u != dep[k].end(); ++u)
							{
								if ((node[*u].listl_sch_status == 0) || (node[*u].listl_end_time > latency))
								{
									schedule = 0;
								}
							}
							if (schedule == 1)
							{
								U_mul[0].push_back(k);
							}
						}
						//else if (!strcmp(node[k].opr1, "+") || !strcmp(node[k].opr1, "-"))
						else if ((node[k].node_name == "add") || (node[k].node_name == "sub"))
						{
							list<int>::iterator v;
							for (v = dep[k].begin(); v != dep[k].end(); ++v)
							{
								if ((node[*v].listl_sch_status == 0) || (node[*v].listl_end_time >= latency))
								{
									schedule = 0;
								}
							}
							if (schedule == 1)
							{
								U_add[0].push_back(k);
							}
						}
					//	else if (!strcmp(node[k].opr1, ">") || !strcmp(node[k].opr1, "<") || !strcmp(node[k].opr1, "==") || !strcmp(node[k].opr1, "?") || !strcmp(node[k].opr1, ">>") || !strcmp(node[k].opr1, "<<"))
						else if ((node[k].node_name == "leftshift") || (node[k].node_name == "rightshift") || (node[k].node_name == "comp") || (node[k].node_name == "not") || (node[k].node_name == "greater") || (node[k].node_name == "lesser"))
						{
							list<int>::iterator x;
							for (x = dep[k].begin(); x != dep[k].end(); ++x)
							{
								if ((node[*x].listl_sch_status == 0) || (node[*x].listl_end_time >= latency))
								{
									schedule = 0;
								}
							}
							if (schedule == 1)
							{
								U_log[0].push_back(k);
							}
						}
						else
						{
							list<int>::iterator w;
							for (w = dep[k].begin(); w != dep[k].end(); ++w)
							{
								if ((node[*w].listl_sch_status == 0) || (node[*w].listl_end_time >= latency))
								{
									schedule = 0;
								}
							}
							if (schedule == 1)
							{
								U_reg[0].push_back(k);
							}
						}
					}
				}
			}

			list<int>::iterator m;
			//cout << "U_mul " ;
			for (m = U_mul[0].begin(); m != U_mul[0].end(); ++m)
			{
				//cout << *m << " ";
			}
			//cout << endl;

			list<int>::iterator n;
			//cout << "U_add " ;
			for (n = U_add[0].begin(); n != U_add[0].end(); ++n)
			{
				//cout << *n << " ";
			}
			//cout << endl;

			list<int>::iterator s;
			//cout << "U_log " ;
			for (s = U_log[0].begin(); s != U_log[0].end(); ++s)
			{
				//cout << *s << " ";
			}
			//cout << endl;

			list<int>::iterator t;
			//cout << "U_reg " ;
			for (t = U_reg[0].begin(); t != U_reg[0].end(); ++t)
			{
				//cout << *t << " ";
			}
			//cout << endl;

			if ((U_mul[0].size() == 0) && (U_add[0].size() == 0) && (U_log[0].size() == 0) && (U_reg[0].size() == 0))
			{
				for (int i1 = startnode; i1 <= endnode; i1++)
				{
					if ((node[i1].listl_end_time == latency) && node[i1].node_name=="mul")
					{
						node[i1].post_dummy_state_list_lr = 1;
						break;
					}
				}
			}

			//-------------------------- End Fining U array------------------------------------------//

			//--------------------------Find S array-------------------------------------------------//
			list<int>::iterator p;
			//---S array for Mul----//
			for (m = U_mul[0].begin(); m != U_mul[0].end(); ++m)
			{
				for (p = U_mul[0].begin(); p != U_mul[0].end(); ++p)
				{
					if (node[*p].listl_sch_status == 1) continue;
					if (max_mul_label <= node[*p].listl_label)
					{
						max_mul_label = node[*p].listl_label;
						max_mul_node = *p;
					}
				}

				node[max_mul_node].state_no_listl = state_no_listl + latency - 1;
				node[max_mul_node].listl_start_time = latency;
				node[max_mul_node].listl_end_time = latency + 1;
				node[max_mul_node].listl_sch_status = 1;
				buffer_mul = buffer_mul - 1;


				max_mul_label = 1;
				if (buffer_mul == 0)
				{
					break;
				}

			}
			buffer_mul = mul;
			//------end S arrray for mul---//

			//---S array for Add----//
			for (m = U_add[0].begin(); m != U_add[0].end(); ++m)
			{
				for (p = U_add[0].begin(); p != U_add[0].end(); ++p)
				{
					if (node[*p].listl_sch_status == 1) continue;
					if (max_add_label <= node[*p].listl_label)
					{
						max_add_label = node[*p].listl_label;
						max_add_node = *p;
					}
				}

				node[max_add_node].state_no_listl = state_no_listl + latency - 1;
				node[max_add_node].listl_start_time = latency;
				node[max_add_node].listl_end_time = latency;
				node[max_add_node].listl_sch_status = 1;
				buffer_add = buffer_add - 1;

				max_add_label = 1;
				if (buffer_add == 0)
				{
					break;
				}

			}
			buffer_add = add;
			//------end S arrray for add---//

			//---S array for log----//
			for (m = U_log[0].begin(); m != U_log[0].end(); ++m)
			{
				for (p = U_log[0].begin(); p != U_log[0].end(); ++p)
				{
					if (node[*p].listl_sch_status == 1) continue;
					if (max_log_label <= node[*p].listl_label)
					{
						max_log_label = node[*p].listl_label;
						max_log_node = *p;
					}
				}

				node[max_log_node].state_no_listl = state_no_listl + latency - 1;
				node[max_log_node].listl_start_time = latency;
				node[max_log_node].listl_end_time = latency;
				node[max_log_node].listl_sch_status = 1;
				buffer_log = buffer_log - 1;

				max_log_label = 1;
				if (buffer_log == 0)
				{
					break;
				}
			}
			buffer_log = log;
			//------end S arrray for log---//

			//-------s array for reg-----//
			for (t = U_reg[0].begin(); t != U_reg[0].end(); ++t)
			{
				node[*t].state_no_listl = state_no_listl + latency - 1;
				node[*t].listl_start_time = latency;
				node[*t].listl_end_time = latency;
				node[*t].listl_sch_status = 1;
			}
			//-------end s array for reg-----//

			for (int i = startnode; i <= endnode; i++)
			{
				if (node[i].listl_sch_status == 0)
				{
					sch_flag = 1;
				}

				if (sch_flag == 1)
				{
					all_scheduled = 0;
					sch_flag = 0;
					break;
				}
				else all_scheduled = 1;

			}
			//----Print s array ---------//
			//cout << endl <<" Time slot " << latency << endl;
			for (int i = startnode; i <= endnode; i++)
			{
				if ((node[i].listl_sch_status == 1) && (node[i].listl_start_time == latency))
				{
					//cout << "node " << i << " state number , st time , end time is " << node[i].listl_start_time << " " << node[i].listl_end_time  << " " << node[i].state_no_listl << endl ;
				}
			}
			//------end print s array------//

			U_mul[0].clear();
			U_add[0].clear();
			U_log[0].clear();
			U_reg[0].clear();
			latency++;

		}
		for (int i = startnode; i <= endnode; i++)
		{
			if ((node[i].state_no_listl == state_no_listl + latency - 2) && node[i].node_name=="mul")
			{
				state_no_listl++;
				node[i].post_dummy_state_list_lr = 1;
				break;
			}
		}
		state_no_listl = state_no_listl + latency - 1;
		return state_no_listl;
		
	}//end of find_listl_states

	void list_l::print_listl_verilog()
	{
		ofstream verilog_file;
		verilog_file.open("verilog_listl.v");

		Console::Out::WriteLine(_T("yay, i am printer"));

		verilog_file << "`timescale 1ns / 1ps" << endl << endl;

		verilog_file << "// ------------------------------------------------------------------------------------------------- " << endl;
		verilog_file << " // Copyright (c) 2013 Pruthvi & Mukunda MJ , University of Arizona" << endl;
		verilog_file << " // Permission to copy is granted provided that this header remains" << endl;
		verilog_file << " // intact. This software is provided with no warranties" << endl;
		verilog_file << " // -------------------------------------------------------------------------------------------------" << endl;

		verilog_file << "module" << " " << "HLSM (Clk,Rst,Start,Done";
	
		//print all the inputs and the outputs

		/*	for (int i = 0; i< nofip; i++)
		{
			verilog_file << "," << inputs[i];
		}

		for (int j = 0; j< nofop; j++)
		{
			verilog_file << "," << outputs[j];
		}
		verilog_file << ");" << "\n" << endl;

		verilog_file << "input Clk, Rst, Start;" << endl;
		verilog_file << "output reg Done;" << endl;
		verilog_file << "input[31:0]" << " ";
	
		
		 verilog_file << inputs[0] ;
		for (int i = 1; i< nofip ; i++)
		{
		verilog_file << "," << inputs[i]  ;
		}
		verilog_file << ";"<< endl;

		verilog_file << "output reg [31:0] " << " ";
		verilog_file << outputs[0] ;
		for (int i = 1; i< nofop ; i++)
		{
		verilog_file << "," << outputs[i] ;
		}
		verilog_file << ";"<< endl;

		if ( nofr > 0)
		{
		verilog_file << "reg[31:0]" << " ";
		verilog_file << reg[0];
		for (int i = 1; i< nofr ; i++)
		{
		verilog_file << "," << reg[i] ;
		}
		verilog_file << ";"<< "\n" << endl;
		}
		


		if ((total_listl_states + 2) <= 2)
			verilog_file << "reg [1:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listl_states + 2) <= 4)
			verilog_file << "reg [1:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listl_states + 2) <= 8)
			verilog_file << "reg [2:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listl_states + 2) <= 16)
			verilog_file << "reg [3:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listl_states + 2) <= 32)
			verilog_file << "reg [4:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listl_states + 2) <= 64)
			verilog_file << "reg [5:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listl_states + 2) <= 128)
			verilog_file << "reg [6:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listl_states + 2) <= 2)
			verilog_file << "reg [7:0]" << " " << "state" << " " << ";" << endl;

		verilog_file << "parameter" << " " << "start_wait" << " " << "=" << " " << "0" << ";" << endl;

		for (int i = 1; i <= total_listl_states; i++)
		{
			verilog_file << "parameter" << " " << "state" << "_" << i << " " << "=" << " " << i << " ;" << endl;
		}

		verilog_file << "parameter" << " " << "Final = " << (total_listl_states + 1) << " ;" << endl;
		verilog_file << "\n" << endl;

		verilog_file << "always@" << " " << "(posedge Clk)" << " begin" << endl;
		verilog_file << "if" << " " << "(" << " " << "Rst" << " " << "==" << "1" << " " << ")" << " " << "begin" << endl;
		

		//initilize all the outputs and regs to zero

	/*	verilog_file << outputs[0] << "<=0;" << endl;
		for (int i = 1; i < nofop; i++)
		{
			verilog_file << outputs[i] << "<=0;" << endl;
		}
		verilog_file << reg[0] << "<=0;" << endl;
		for (int i = 1; i < nofr; i++)
		{
			verilog_file << reg[i] << "<=0;" << endl;
		} 

		verilog_file << "Done <= 0;" << endl;
		verilog_file << "state" << " " << "<=" << " " << "start_wait" << " " << ";" << endl;



		verilog_file << "end" << endl;

		verilog_file << endl;

		verilog_file << "else begin" << endl;
		verilog_file << "case( state )" << endl;

		verilog_file << "start_wait : begin" << endl;
		verilog_file << "if(Start == 1) begin " << endl;
		verilog_file << "state <= state_1 ;" << endl;
		verilog_file << "Done <= 0;" << endl;
		verilog_file << "end" << endl;

		verilog_file << "else begin" << endl;
		verilog_file << "state <= start_wait ;" << endl;
		verilog_file << "end" << endl;
		verilog_file << "end" << endl;


		bool print_flag = 0;

		for (int j = 1; j <= total_listl_states; j++)
		{

			/*         for(int i=0 ; i< node ; i++)
			{
			if( node[i].state_no_listl == j)
			{
			if (!strcmp(node[i].node_type , "while") )
			{
			verilog_file << "state_" << j << " : begin" << endl;
			verilog_file << "if (" << node[i].condition << " )  begin " <<endl;
			verilog_file << "state <= state_" << (node[i].state_no_listl+1) << " ;" << endl;
			verilog_file << "end" << endl;

			verilog_file << "else  begin" << endl;
			if ( (node[i].listl_jump_state) == total_listl_states)
			{
			verilog_file << "state <= Final ;" <<endl;
			}
			else
			{
			verilog_file << "state <= state_" << (node[i].listl_jump_state+1) << " ;" <<endl;
			}
			verilog_file << "end" << endl;
			verilog_file << "end" << endl;

			verilog_file << "state_" << node[i].listl_jump_state << " : begin" << endl;
			verilog_file << "state <= state_" << node[i].state_no_listl << " ;" << endl;
			verilog_file << "end" << endl;
			print_flag = 1;
			break;
			}

			else if (!strcmp(node[i].node_type , "if") )
			{
			verilog_file << "state_" << j << " : begin" << endl;
			verilog_file << "if (" << node[i].condition << " ) begin " << endl;
			verilog_file << "state <= state_" << (node[i].state_no_listl+1) << " ;" << endl;
			verilog_file << "end" << endl;

			verilog_file << "else begin" << endl;
			verilog_file << "state <= state_" << (node[i].listl_jump_state) << " ;" <<endl;
			verilog_file << "end" << endl;
			verilog_file << "end" << endl;

			verilog_file << "state_" << node[i].listl_jump_state << " : begin" << endl;
			if ( (node[i].listl_jump_state) == total_listl_states)
			{
			verilog_file << "state <= Final ;" <<endl;
			}
			else
			{
			verilog_file << "state <= state_" << (node[i].listl_jump_state+1) << " ;" << endl;
			}
			verilog_file << "end" << endl;
			print_flag = 1;
			break;
			}

			else if (!strcmp(node[i].node_type , "if/while") )
			{
			print_flag = 1;
			break;
			}
			}
			} 

			if (print_flag == 0)
			{
				verilog_file << "state_" << j << " : begin" << endl;
				for (int i = 0; i< array_num; i++)
				{
					if (node[i].state_no_listl == j)
					{
						if (node[i].node_name == "not")
						{
							verilog_file << node[i].node_name << " = " << node[i].input1 << ";" << endl;
						} 

						else if (node[i].node_name == "add"){ verilog_file << node[i].node_name<< " <= " << node[i].input1 << " + " << node[i].input2 << ";" << endl; }

						else if (node[i].node_name == "sub"){ verilog_file << node[i].node_name << " <= " << node[i].input1 << " - " << node[i].input2 << ";" << endl; }

						else if (node[i].node_name == "mul"){ verilog_file << node[i].node_name << " <= " << node[i].input1 << " * " << node[i].input2 << ";" << endl; }

						else if (node[i].node_name == "greater") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " > " << node[i].input2 << ";" << endl; }

						else if (node[i].node_name == "lesser") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " < " << node[i].input2 << ";" << endl; }

						else if (node[i].node_name == "comp") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " == " << node[i].input2 << ";" << endl; }

						else if (node[i].node_name == "leftshift") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " << " << node[i].input2 << ";" << endl; }

						else if (node[i].node_name == "rightshift") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " >> " << node[i].input2 << ";" << endl; }

					}
				}
				if (j == total_listl_states)
				{
					verilog_file << "state <= Final" << ";" << endl << "end" << endl;
					continue;
				}
				else
				{
					verilog_file << "state <= state_" << j + 1 << ";" << endl << "end" << endl;
				}
			}

			print_flag = 0;
		}
		verilog_file << "Final: begin" << endl << "Done <= 1;" << endl << "state <= start_wait;" << endl << "end" << endl;
		verilog_file << "endcase" << endl << "end" << endl << "end" << endl << "endmodule";

	}


};  */