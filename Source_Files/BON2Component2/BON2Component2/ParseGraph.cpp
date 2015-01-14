#include "stdafx.h"
#include "ParseGraph.h"
#include <Console.h>
#include<sstream>
#include<fstream>
#include<list>
#include<string>
#include<list_l.h>


using namespace std;
using namespace GMEConsole;
using namespace BON;

namespace parse{

	//###############################################################################################################################################
	//
	// 	C L A S S : parse::ParseGraph
	//
	//###############################################################################################################################################


	ParseGraph::ParseGraph() : array_num(0)
	{
		mul_qty_global = new list<int>[1];
		add_qty_global = new list<int>[1];
		log_qty_global = new list<int>[1];
	}

	ParseGraph::~ParseGraph(){

	}

	void ParseGraph::find_attributes(Project &project){

		using namespace GMEConsole;
		Console::Out::WriteLine(_T("Interpreter Started.........................!!!!!"));
		BON::Folder rf = project->getRootFolder();

		/* Now get all the child models*/
		if (rf){
			std::set<Model> models = rf->getChildModels();
			std::stringstream stream;
			/*	stream << "Total models in this project " << models.size();
			Console::Out::WriteLine(CString(stream.str().c_str())); */

			std::string atoms_array[] = { "sink", "add", "sub", "mul", "not", "rightshift", "leftshift", "comp", "greater", "lesser" };

			for (std::set<Model> ::iterator it = models.begin(); it != models.end(); it++){
				Model model = *it;

				/* Pass each child atom to get the attributes and store it in struct*/
				for (int i = 0; i < 10; i++){
					fill_attributes(atoms_array[i], model);
				}



				/*	stream << "I am executing";
				Console::Out::WriteLine(CString(stream.str().c_str())); */
				std::set<BON::FCO> list_l_comp = model->getChildFCOsAs("list_l");
				if (list_l_comp.size()){
					//Console::Out::WriteLine(_T("hmmmmmmmmm..."));

					//find the attributes for mul,add and logical
					for (std::set<BON::FCO>::iterator it5 = list_l_comp.begin(); it5 != list_l_comp.end(); it5++){
				//		Console::Out::WriteLine(_T("hmmmmmmmmm...yes i am inside loop"));
						num_mul = (*it5)->getAttribute("mul")->getIntegerValue();
						num_add = (*it5)->getAttribute("add")->getIntegerValue();
						num_logical = (*it5)->getAttribute("logical")->getIntegerValue();
					}
					int startnode = 0;
					int endnode = array_num;
					stream << "array_num" << array_num;

					/*		Console::Out::WriteLine(CString(stream.str().c_str()));
					stream << "num_mul" << num_mul;
					Console::Out::WriteLine(CString(stream.str().c_str()));
					stream << "num_add" << num_add;
					Console::Out::WriteLine(CString(stream.str().c_str()));
					stream << "num_logical" << num_logical;
					Console::Out::WriteLine(CString(stream.str().c_str()));  */

					int mul = num_mul;
					int add = num_add;
					int log = num_logical;
					int state_no_listl = 1;
					CDGF_Schedule(startnode, endnode);
					find_listl_label(startnode, endnode);
					state_no_listl = find_listl_states(startnode, endnode, state_no_listl, mul, add, log);

					total_listl_states = state_no_listl - 1;
					print_listl_verilog(model);

				}

				std::set<BON::FCO> list_r_comp = model->getChildFCOsAs("list_r");
				if (list_r_comp.size()){
					for (std::set<BON::FCO>::iterator it5 = list_r_comp.begin(); it5 != list_r_comp.end(); it5++){
						latency_sel = (*it5)->getAttribute("latency")->getIntegerValue();
					}
					int state_no_listr = 1;
					int startnode = 0;
					int latency = latency_sel;
					int endnode = array_num;
					CDGF_Schedule(startnode, endnode);
					alap_schedule(startnode, endnode, latency);
					state_no_listr = find_listr_states(startnode, endnode, state_no_listr, latency);
					total_listr_states = state_no_listr - 1;
					print_listr_verilog(model);

				}





			}

		} //end rf

	}  //end find_attribute




	void ParseGraph::fill_attributes(std::string atom_name, Model &model){

		try {
			std::stringstream stream;
			using namespace GMEConsole;
			std::set<BON::FCO>  node1 = model->getChildFCOsAs(atom_name);

			for (std::set<BON::FCO>::iterator it2 = node1.begin(); it2 != node1.end(); it2++)
			{
				if (*it2){
					int flag = 0;
					int count_connx = 0; //to find the constraint voilation for each gate type 

					std::set<BON::Connection> conns = (*it2)->getConnLinks();
					for (std::set<BON::Connection>::iterator it4 = conns.begin(); it4 != conns.end(); it4++)
					{

						BON::ConnectionEnd object;
						if ((*it4)->getDst() == (*it2)) {
							object = (*it4)->getSrc();
							if (BON::FCO(object)) {
								count_connx++;
								BON::FCO otherFCO(object);
								node.push_back(gate_node());
								if (flag == 0){
									node[array_num].input1 = otherFCO->getName();
									node[array_num].src_type1 = otherFCO->getObjectMeta().name();
								}

								else{
									node[array_num].input2 = otherFCO->getName();
									node[array_num].src_type2 = otherFCO->getObjectMeta().name();
								}
							}
							flag++;
							node[array_num].node_num = array_num;
							node[array_num].status = TRUE;
							node[array_num].node_name = (*it2)->getName();
							node[array_num].atom_type = (*it2)->getObjectMeta().name();
						}
					}

					if ((node[array_num].atom_type == "add") && (count_connx > 2) ){
						throw "add";
					}
					if ((node[array_num].atom_type == "Sink") && (count_connx > 2)){
						throw "sink";
					}
					if ((node[array_num].atom_type == "sub") && (count_connx > 2) ){
						throw "sub";
					}
					if ((node[array_num].atom_type == "mul") && (count_connx > 2) ){
						throw "mul";
					}
					if ((node[array_num].atom_type == "rightshift") && (count_connx > 2)){
						throw "rightshift";
					}
					if ((node[array_num].atom_type == "leftshift") && (count_connx > 2)){
						throw "leftshift";
					}
					if ((node[array_num].atom_type == "greater") && (count_connx > 2)){
						throw "greater";
					}
					if ((node[array_num].atom_type == "lesser") && (count_connx > 2)){
						throw "lesser";
					}
					if ((node[array_num].atom_type == "comp") && (count_connx > 2)){
						throw "comp";
					}
					if ((node[array_num].atom_type == "not") && (count_connx >= 2)){
						throw 1;
					} 

					array_num++;
					flag = 0;
				}
			}

		} //end of try


		catch (const char* x){
			std::stringstream stream;
			stream << "ERROR:  " << x << " should have exactly 2 input.............." << endl;
			Console::Out::WriteLine(CString(stream.str().c_str()));
	
			
		}

		catch (int x){
			std::stringstream stream;
			stream << "ERROR:  Not " << "  should have exactly 1 input.............." << endl;
			Console::Out::WriteLine(CString(stream.str().c_str()));
		}

	} //End of fill attributes

	


	void ParseGraph::CDGF_Schedule(int startnode, int endnode){
		list<int> *dep = new list<int>[100];
		for (int i = startnode; i <= endnode; i++)
		{
			for (int j = startnode; j <= endnode; j++)
			{
				if (j == i) continue;
				//if (!strcmp(equations[i].ip1, equations[j].op) || !strcmp(equations[i].ip2, equations[j].op) || !strcmp(equations[i].ip3, equations[j].op))
				if ((node[j].node_name == node[i].input1) || (node[j].node_name == node[i].input2))
				{
					dep[i].push_back(j);
				}
			}
		}

		for (int i = 0; i< array_num; i++)
		{
			list<int>::iterator j;
			for (j = dep[i].begin(); j != dep[i].end(); ++j)
			{
				//cout << *j << " ";
			}
			//cout << endl;
		}

		for (int i = startnode; i <= endnode; i++)
		{
			node[i].cdfg_start_time = 1;
			if (node[i].atom_type == "mul")
			{
				node[i].cdfg_end_time = 2;
			}
			else
			{
				node[i].cdfg_end_time = 1;
			}
		}
		//cout<< "cdfg" << endl;
		for (int i = startnode; i <= endnode; i++)
		{
			int end_time = 1;
			list<int>::iterator j;
			for (j = dep[i].begin(); j != dep[i].end(); ++j)
			{
				if (node[*j].cdfg_end_time >  end_time)
				{
					end_time = node[*j].cdfg_end_time;
				}
				node[i].cdfg_start_time = end_time + 1;
				node[i].cdfg_end_time = end_time + 1;
				if (node[i].atom_type == "mul")
				{
					node[i].cdfg_end_time = end_time + 2;
				}
			}

			// cout << "node" << i << " ";
			//cout <<  equations[i].cdfg_start_time << " ";
			//cout <<  equations[i].cdfg_end_time << endl;
		}
		//Console::Out::WriteLine(_T("CDFG started..."));
	} //end of CDFG Schedule 


	//alap schedule
	void ParseGraph::alap_schedule(int startnode, int endnode, int latency){
		list<int> *dep = new list<int>[100];
		for (int i = startnode; i <= endnode; i++)
		{
			for (int j = startnode; j <= endnode; j++)
			{
				if (j == i) continue;
				//if (!strcmp(equations[i].ip1, equations[j].op) || !strcmp(equations[i].ip2, equations[j].op) || !strcmp(equations[i].ip3, equations[j].op))
				if ((node[j].node_name == node[i].input1) || (node[j].node_name == node[i].input2))
				{
					dep[i].push_back(j);
				}
			}
		}

		for (int i = startnode; i <= endnode; i++)
		{
			node[i].alap_end_time = latency;
			if (node[i].atom_type == "mul")
			{
				node[i].alap_start_time = latency - 1;
			}
			else
			{
				node[i].alap_start_time = latency;
			}
		}
		// cout<< "alap" << endl;

		try {
			for (int i = endnode; i >= startnode; i--)
			{
				list<int>::iterator j;
				for (j = dep[i].begin(); j != dep[i].end(); ++j)
				{
					if (node[*j].alap_end_time >= (node[i].alap_start_time - 1))
					{
						node[*j].alap_end_time = node[i].alap_start_time - 1;
					}
					if (node[i].atom_type == "mul")
					{
						node[*j].alap_start_time = node[*j].alap_end_time - 1;
					}
					else
					{
						node[*j].alap_start_time = node[*j].alap_end_time;
					}
				}
				if (node[i].alap_start_time <= 0)
				{
					throw 1;
				}

			}

		}

		catch (int x){
			std::stringstream stream;
			stream << "ERROR:  Scheduling is not possible with this latency.............." << endl;
			Console::Out::WriteLine(CString(stream.str().c_str()));

		}
	}

	void ParseGraph::test_print(){

		std::ofstream outfile;
		outfile.open("test1.cpp");
		if (outfile.is_open()){

			for (std::vector<struct gate_node>::iterator it = node.begin(); it != node.end(); ++it){

				outfile << (*it).node_name << "=" << (*it).input1 << " ," << (*it).input2 << std::endl;
			}
			outfile << array_num;
		} //end if 

		outfile.close();
	}

	void ParseGraph::find_listl_label(int startnode, int endnode){
		/*std::stringstream stream;
		stream << "start node " << startnode;
		Console::Out::WriteLine(CString(stream.str().c_str()));
		stream << "vector size" << node.size();
		Console::Out::WriteLine(CString(stream.str().c_str())); */

		for (int i = startnode; i <= endnode; i++)
		{
			node[i].listl_label = 1;
		}

		list<int> *dep = new list<int>[100];
		for (int i = startnode; i <= endnode; i++)
		{
			for (int j = startnode; j <= endnode; j++)
			{
				if (j == i) continue;
				//if(!strcmp(node[i].ip1,node[j].op) || !strcmp(node[i].ip2,node[j].op) || !strcmp(node[i].ip3,node[j].op))
				if ((node[j].node_name == node[i].input1) || (node[j].node_name == node[i].input2))
				{
					dep[i].push_back(j);
				}
			}
		}

		for (int i = endnode; i >= startnode; i--)
		{
			list<int>::iterator j;
			for (j = dep[i].begin(); j != dep[i].end(); ++j)
			{
				if (node[*j].listl_label <= node[i].listl_label)
				{
					node[*j].listl_label = node[i].listl_label + 1;
				}
			}
		}


		for (int i = startnode; i <= endnode; i++)
		{
			//cout << "listl_label for node " << i << " is " << node[i].listl_label << endl;
		}



	} //end of find_listl_lable

	int ParseGraph::find_listl_states(int startnode, int endnode, int state_no_listl, int mul, int add, int log){

		std::ofstream outfile;
		outfile.open("test.cpp");

	std:stringstream stream;
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
				if ((node[j].node_name == node[i].input1) || (node[j].node_name == node[i].input2))
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
						if (node[k].atom_type == "mul")
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
						else if ((node[k].atom_type == "add") || (node[k].atom_type == "sub"))
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
						else if ((node[k].atom_type == "leftshift") || (node[k].atom_type == "rightshift") || (node[k].atom_type == "comp") || (node[k].node_name == "not") || (node[k].atom_type == "greater") || (node[k].atom_type == "lesser"))
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
					if ((node[i1].listl_end_time == latency) && node[i1].atom_type == "mul")
					{
						node[i1].post_dummy_state_list_lr = 1;
						break;
					}
				}
			}

			//-------------------------- End Fining U array------------------------------------------//

			/*		stream << " u_mul" << U_mul[0].size();
			Console::Out::WriteLine(CString(stream.str().c_str())); */

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
					outfile << "node " << i << " state number , st time , end time is " << node[i].listl_start_time << " " << node[i].listl_end_time << " " << node[i].state_no_listl << endl;
					//Console::Out::WriteLine(CString(stream.str().c_str()));
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
			if ((node[i].state_no_listl == state_no_listl + latency - 2) && node[i].atom_type == "mul")
			{
				state_no_listl++;
				node[i].post_dummy_state_list_lr = 1;
				break;
			}
		}
		state_no_listl = state_no_listl + latency - 1;
		return state_no_listl;

	}//end of find_listl_states 

	void ParseGraph::print_listl_verilog(Model &model)
	{
		ofstream verilog_file;
		verilog_file.open("verilog_listl.v");

		//Console::Out::WriteLine(_T("yay, i am printer"));

		verilog_file << "`timescale 1ns / 1ps" << endl << endl;

		verilog_file << "// ------------------------------------------------------------------------------------------------- " << endl;
		verilog_file << "// Copyright (c) 2014 Pruthvi , University of Arizona" << endl;
		verilog_file << "// Permission to copy is granted provided that this header remains" << endl;
		verilog_file << "// intact. This software is provided with no warranties" << endl;
		verilog_file << "// -------------------------------------------------------------------------------------------------" << endl;

		verilog_file << "module" << " " << "HLSM (Clk,Rst,Start,Done";

		//print all the inputs and the outputs

		std::set<BON::FCO>sources = model->getChildFCOsAs("source");
		for (std::set<BON::FCO> ::iterator it4 = sources.begin(); it4 != sources.end(); ++it4)
		{
			verilog_file << "," << (*it4)->getName();
		}

		std::set<BON::FCO>sinks = model->getChildFCOsAs("sink");
		for (std::set<BON::FCO> ::iterator it4 = sinks.begin(); it4 != sinks.end(); ++it4)
		{
			verilog_file << "," << (*it4)->getName();
		}
		verilog_file << ");" << "\n" << endl;

		verilog_file << "input Clk, Rst, Start;" << endl;
		verilog_file << "output reg Done;" << endl;
		verilog_file << "input[31:0]" << " ";

		int count = 0;
		for (std::set<BON::FCO> ::iterator it4 = sources.begin(); it4 != sources.end(); ++it4)
		{
			if (count == 0){
				verilog_file << (*it4)->getName();
				count++;
			}
			else
				verilog_file << "," << (*it4)->getName();
		}
		verilog_file << ";" << endl;

		verilog_file << "output reg [31:0] " << " ";

		for (std::set<BON::FCO> ::iterator it4 = sinks.begin(); it4 != sinks.end(); ++it4)
		{
			if (count == 1){
				verilog_file << (*it4)->getName();
				count++;
			}
			else
				verilog_file << "," << (*it4)->getName();
		}
		verilog_file << ";" << endl;

		//define all the intermediate components as registers
		verilog_file << "reg [31:0] ";
		count = 0;
		for (std::vector<struct gate_node>::iterator it5 = node.begin(); it5 != node.end(); ++it5){

			if ((count == 0) && ((*it5).atom_type != "sink")){
				verilog_file << (*it5).node_name;
				count++;
			}

			else if ((*it5).atom_type != "sink" && (*it5).atom_type != "\0"){
				verilog_file << "," << (*it5).node_name;
			}
			
		}

		verilog_file << "; " << endl;

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

		for (std::set<BON::FCO> ::iterator it4 = sinks.begin(); it4 != sinks.end(); ++it4)
		{
			verilog_file << (*it4)->getName() << "<= 0;" << endl;;
		}
		//end of initilization

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

			if (print_flag == 0)
			{
				verilog_file << "state_" << j << " : begin" << endl;
				for (int i = 0; i< array_num; i++)
				{
					if (node[i].state_no_listl == j)
					{
						if (node[i].atom_type == "not" || node[i].atom_type == "sink")
						{
							verilog_file << node[i].node_name << " <= " << node[i].input1 << ";" << endl;
						}

						else if (node[i].atom_type == "add"){ verilog_file << node[i].node_name << " <= " << node[i].input1 << " + " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "sub"){ verilog_file << node[i].node_name << " <= " << node[i].input1 << " - " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "mul"){ verilog_file << node[i].node_name << " <= " << node[i].input1 << " * " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "greater") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " > " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "lesser") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " < " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "comp") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " == " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "leftshift") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " << " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "rightshift") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " >> " << node[i].input2 << ";" << endl; }



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


	int ParseGraph::find_listr_states(int startnode, int endnode, int state_no_listr, int latency){
		int local_mul_qty = 0;
		int local_add_qty = 0;
		int local_log_qty = 0;
		int mul_qty = 1;
		int add_qty = 1;
		int log_qty = 1;
		int buffer_mul = 1;
		int buffer_add = 1;
		int buffer_log = 1;
		int schedule_break_flag = 0;

		std::ofstream outfile;
		outfile.open("test.cpp");

		//-------------------------Finding depenancy list---------------------//
		list<int> *dep = new list<int>[100];
		for (int i = startnode; i <= endnode; i++)
		{
			for (int j = startnode; j <= endnode; j++)
			{
				if (j == i) continue;
				//if(!strcmp(node[i].ip1,node[j].op) || !strcmp(node[i].ip2,node[j].op) || !strcmp(node[i].ip3,node[j].op))
				if ((node[j].node_name == node[i].input1) || (node[j].node_name == node[i].input2))
				{
					dep[i].push_back(j);
				}
			}
		}
		//-------------------------End Finding depenancy list---------------------//

		//-------------------------Initializing slack asn schedule status---------------------//
		for (int j = startnode; j <= endnode; j++)
		{
			node[j].listr_sch_status = 0;
			node[j].post_dummy_state_list_lr = 0;
			node[j].listr_end_time = latency;
		}

		//-------------------------End Initializing slack asn schedule status---------------------//

		list<int> *U_mul = new list<int>[1];
		list<int> *U_add = new list<int>[1];
		list<int> *U_log = new list<int>[1];
		list<int> *U_reg = new list<int>[1];

		for (int l = 1; l <= latency; l++)
		{

			for (int j = startnode; j <= endnode; j++)
			{
				node[j].slack_time = node[j].alap_start_time - l;
			}
			//-------------------------- Finding U array------------------------------------------//
			for (int k = startnode; k <= endnode; k++)
			{
				int schedule = 1;
				if (node[k].listr_sch_status == 0)
				{
					if (node[k].cdfg_start_time <= l)
					{
						if (node[k].atom_type == "mul")
						{
							list<int>::iterator u;
							for (u = dep[k].begin(); u != dep[k].end(); ++u)
							{
								if ((node[*u].listr_sch_status == 0) || (node[*u].listr_end_time > l))
								{
									schedule = 0;
								}
							}
							if (schedule == 1)
							{
								U_mul[0].push_back(k);
							}
						}
						else if ((node[k].atom_type == "add") || (node[k].atom_type == "sub"))
						{
							list<int>::iterator v;
							for (v = dep[k].begin(); v != dep[k].end(); ++v)
							{
								if ((node[*v].listr_sch_status == 0) || (node[*v].listr_end_time >= l))
								{
									schedule = 0;
								}
							}
							if (schedule == 1)
							{
								U_add[0].push_back(k);
							}
						}
						else if ((node[k].atom_type == "leftshift") || (node[k].atom_type == "rightshift") || (node[k].atom_type == "comp") || (node[k].node_name == "not") || (node[k].atom_type == "greater") || (node[k].atom_type == "lesser"))
						{
							list<int>::iterator x;
							for (x = dep[k].begin(); x != dep[k].end(); ++x)
							{
								if ((node[*x].listr_sch_status == 0) || (node[*x].listr_end_time >= l))
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
								if ((node[*w].listr_sch_status == 0) || (node[*w].listr_end_time >= l))
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
			// cout << "U_mul " ;
			for (m = U_mul[0].begin(); m != U_mul[0].end(); ++m)
			{
				//  cout << *m << " ";
			}
			//  cout << endl;

			list<int>::iterator n;
			//  cout << "U_add " ;
			for (n = U_add[0].begin(); n != U_add[0].end(); ++n)
			{
				//   cout << *n << " ";
			}
			// cout << endl;

			list<int>::iterator s;
			// cout << "U_log " ;
			for (s = U_log[0].begin(); s != U_log[0].end(); ++s)
			{
				//     cout << *s << " ";
			}
			// cout << endl;

			list<int>::iterator t;
			// cout << "U_reg " ;
			for (t = U_reg[0].begin(); t != U_reg[0].end(); ++t)
			{
				//    cout << *t << " ";
			}
			// cout << endl;

			if ((U_mul[0].size() == 0) && (U_add[0].size() == 0) && (U_log[0].size() == 0) && (U_reg[0].size() == 0))
			{

			}
			if ((U_mul[0].size() == 0) && (U_add[0].size() == 0) && (U_log[0].size() == 0) && (U_reg[0].size() == 0))
			{
				schedule_break_flag = 0;
				for (int i1 = startnode; i1 <= endnode; i1++)
				{
					if ((node[i1].listr_end_time == l) && node[i1].atom_type == "mul")
					{
						node[i1].post_dummy_state_list_lr = 1;
						schedule_break_flag = 1;
						break;
					}
				}
				if (schedule_break_flag == 0)
				{
					state_no_listr = state_no_listr + l - 1;
					mul_qty_global[0].push_back(mul_qty);
					add_qty_global[0].push_back(add_qty);
					log_qty_global[0].push_back(log_qty);
					return state_no_listr;
				}
			}
			//-------------------------- End Fining U array------------------------------------------//

			//--------------------------Find S array-------------------------------------------------//
			//---S array for Mul----//
			//list<int>::iterator m;
			for (m = U_mul[0].begin(); m != U_mul[0].end(); ++m)
			{
				if (node[*m].slack_time == 0)
				{
					node[*m].state_no_listr = state_no_listr + l - 1;
					node[*m].listr_start_time = l;
					node[*m].listr_end_time = l + 1;
					node[*m].listr_sch_status = 1;
					local_mul_qty = local_mul_qty + 1;
					buffer_mul = buffer_mul - 1;
				}
			}
			for (m = U_mul[0].begin(); m != U_mul[0].end(); ++m)
			{
				if ((node[*m].slack_time != 0) && (buffer_mul > 0))
				{
					node[*m].state_no_listr = state_no_listr + l - 1;
					node[*m].listr_start_time = l;
					node[*m].listr_end_time = l + 1;
					node[*m].listr_sch_status = 1;
					buffer_mul = buffer_mul - 1;
					local_mul_qty = local_mul_qty + 1;
				}
			}
			if (mul_qty <= local_mul_qty)
			{
				mul_qty = local_mul_qty;
				buffer_mul = local_mul_qty;
			}
			local_mul_qty = 0;
			//------end S arrray for mul---//

			//-----S array for adder----//
			//list<int>::iterator m;
			for (m = U_add[0].begin(); m != U_add[0].end(); ++m)
			{
				if (node[*m].slack_time == 0)
				{
					node[*m].state_no_listr = state_no_listr + l - 1;
					node[*m].listr_start_time = l;
					node[*m].listr_end_time = l;
					node[*m].listr_sch_status = 1;
					local_add_qty = local_add_qty + 1;
					buffer_add = buffer_add - 1;
				}
			}
			for (m = U_add[0].begin(); m != U_add[0].end(); ++m)
			{
				if ((node[*m].slack_time != 0) && (buffer_add > 0))
				{
					node[*m].state_no_listr = state_no_listr + l - 1;
					node[*m].listr_start_time = l;
					node[*m].listr_end_time = l;
					node[*m].listr_sch_status = 1;
					buffer_add = buffer_add - 1;
					local_add_qty = local_add_qty + 1;
				}
			}
			if (add_qty <= local_add_qty)
			{
				add_qty = local_add_qty;
				buffer_add = local_add_qty;
			}
			local_add_qty = 0;

			//-------end s array for adder-----//

			//-----S array for logical units----//
			//list<int>::iterator m;
			for (m = U_log[0].begin(); m != U_log[0].end(); ++m)
			{
				if (node[*m].slack_time == 0)
				{
					node[*m].state_no_listr = state_no_listr + l - 1;
					node[*m].listr_start_time = l;
					node[*m].listr_end_time = l;
					node[*m].listr_sch_status = 1;
					local_log_qty = local_log_qty + 1;
					buffer_log = buffer_log - 1;
				}
			}
			for (m = U_log[0].begin(); m != U_log[0].end(); ++m)
			{
				if ((node[*m].slack_time != 0) && (buffer_log > 0))
				{
					node[*m].state_no_listr = state_no_listr + l - 1;
					node[*m].listr_start_time = l;
					node[*m].listr_end_time = l;
					node[*m].listr_sch_status = 1;
					buffer_log = buffer_log - 1;
					local_log_qty = local_log_qty + 1;
				}
			}
			if (log_qty <= local_log_qty)
			{
				log_qty = local_log_qty;
				buffer_log = local_log_qty;
			}
			local_log_qty = 0;

			//-------end s array for logical unit-----//

			//-------s array for reg-----//
			for (t = U_reg[0].begin(); t != U_reg[0].end(); ++t)
			{
				node[*t].state_no_listr = state_no_listr + l - 1;
				node[*t].listr_start_time = l;
				node[*t].listr_end_time = l;
				node[*t].listr_sch_status = 1;
			}
			//-------end s array for reg-----//

			//----Print s array ---------//
			//  cout << endl <<" Time slot " << l << endl;

			for (int i = startnode; i <= endnode; i++)
			{
				if ((node[i].listr_sch_status == 1) && (node[i].listr_start_time == l))
				{
					//outfile << "node " << i << " state number , st time , end time is " << node[i].listr_start_time << " " << node[i].listr_end_time << " " << node[i].state_no_listr << endl;
					outfile << "node " << i << " state number " << node[i].state_no_listr << endl;
				}
			}
			outfile << "array_num" << array_num << endl;
			//outfile << "total_listr_states" << total_listr_states << endl;
			//------end print s array------//
			U_mul[0].clear();
			U_add[0].clear();
			U_log[0].clear();
			U_reg[0].clear();
		}

		state_no_listr = state_no_listr + latency;
		mul_qty_global[0].push_back(mul_qty);
		add_qty_global[0].push_back(add_qty);
		log_qty_global[0].push_back(log_qty);
		return state_no_listr;
	}


	void ParseGraph::print_listr_verilog(Model &model)
	{
		ofstream verilog_file;
		verilog_file.open("verilog_listr.v");

		verilog_file << "`timescale 1ns / 1ps" << endl << endl;

		verilog_file << "// ------------------------------------------------------------------------------------------------- " << endl;
		verilog_file << "// Copyright (c) 2014 Pruthvi, University of Arizona" << endl;
		verilog_file << "// Permission to copy is granted provided that this header remains" << endl;
		verilog_file << "// intact. This software is provided with no warranties" << endl;
		verilog_file << "// -------------------------------------------------------------------------------------------------" << endl;

		verilog_file << "module" << " " << "HLSM (Clk,Rst,Start,Done";

		//print all the inputs and the outputs

		std::set<BON::FCO>sources = model->getChildFCOsAs("source");
		for (std::set<BON::FCO> ::iterator it4 = sources.begin(); it4 != sources.end(); ++it4)
		{
			verilog_file << "," << (*it4)->getName();
		}

		std::set<BON::FCO>sinks = model->getChildFCOsAs("sink");
		for (std::set<BON::FCO> ::iterator it4 = sinks.begin(); it4 != sinks.end(); ++it4)
		{
			verilog_file << "," << (*it4)->getName();
		}
		verilog_file << ");" << "\n" << endl;

		verilog_file << "input Clk, Rst, Start;" << endl;
		verilog_file << "output reg Done;" << endl;
		verilog_file << "input[31:0]" << " ";

		int count = 0;
		for (std::set<BON::FCO> ::iterator it4 = sources.begin(); it4 != sources.end(); ++it4)
		{
			if (count == 0){
				verilog_file << (*it4)->getName();
				count++;
			}
			else
				verilog_file << "," << (*it4)->getName();
		}
		verilog_file << ";" << endl;

		verilog_file << "output reg [31:0] " << " ";

		for (std::set<BON::FCO> ::iterator it4 = sinks.begin(); it4 != sinks.end(); ++it4)
		{
			if (count == 1){
				verilog_file << (*it4)->getName();
				count++;
			}
			else
				verilog_file << "," << (*it4)->getName();
		}
		verilog_file << ";" << endl;

		//define all the intermediate components as registers
		verilog_file << "reg [31:0] ";
		count = 0;
		for (std::vector<struct gate_node>::iterator it5 = node.begin(); it5 != node.end(); ++it5){

			if (count == 0 && (*it5).atom_type != "sink"){
				verilog_file << (*it5).node_name;
				count++;
			}
			
			else if ((*it5).atom_type != "sink" && (*it5).atom_type!="\0" ){
				verilog_file << "," << (*it5).node_name;
				}
		}
		
		verilog_file << "; " << endl;
		if ((total_listr_states + 2) <= 2)
			verilog_file << "reg [1:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listr_states + 2) <= 4)
			verilog_file << "reg [1:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listr_states + 2) <= 8)
			verilog_file << "reg [2:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listr_states + 2) <= 16)
			verilog_file << "reg [3:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listr_states + 2) <= 32)
			verilog_file << "reg [4:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listr_states + 2) <= 64)
			verilog_file << "reg [5:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listr_states + 2) <= 128)
			verilog_file << "reg [6:0]" << " " << "state" << " " << ";" << endl;
		else if ((total_listr_states + 2) <= 2)
			verilog_file << "reg [7:0]" << " " << "state" << " " << ";" << endl;

		verilog_file << "parameter" << " " << "start_wait" << " " << "=" << " " << "0" << ";" << endl;

		for (int i = 1; i <= total_listr_states; i++)
		{
			verilog_file << "parameter" << " " << "state" << "_" << i << " " << "=" << " " << i << " ;" << endl;
		}

		verilog_file << "parameter" << " " << "Final = " << (total_listr_states + 1) << " ;" << endl;
		verilog_file << "\n" << endl;

		verilog_file << "always@" << " " << "(posedge Clk)" << " begin" << endl;
		verilog_file << "if" << " " << "(" << " " << "Rst" << " " << "==" << "1" << " " << ")" << " " << "begin" << endl;

		//initilize all the outputs and regs to zero
	
		for (std::set<BON::FCO> ::iterator it4 = sinks.begin(); it4 != sinks.end(); ++it4)
		{
			verilog_file << (*it4)->getName() << "<= 0;" << endl;;
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

		for (int j = 1; j <= total_listr_states; j++)
		{

			if (print_flag == 0)
			{
				verilog_file << "state_" << j << " : begin" << endl;
				for (int i = 0; i< array_num; i++)
				{
					if (node[i].state_no_listr == j)
					{
						if (node[i].atom_type == "not" || node[i].atom_type == "sink")
						{
							verilog_file << node[i].node_name << " <= " << node[i].input1 << ";" << endl;
						}

						else if (node[i].atom_type == "add"){ verilog_file << node[i].node_name << " <= " << node[i].input1 << " + " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "sub"){ verilog_file << node[i].node_name << " <= " << node[i].input1 << " - " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "mul"){ verilog_file << node[i].node_name << " <= " << node[i].input1 << " * " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "greater") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " > " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "lesser") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " < " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "comp") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " == " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "leftshift") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " << " << node[i].input2 << ";" << endl; }

						else if (node[i].atom_type == "rightshift") { verilog_file << node[i].node_name << " <= " << node[i].input1 << " >> " << node[i].input2 << ";" << endl; }



					}
				}
				if (j == total_listr_states)
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

}; //end namespace parse 