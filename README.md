# DataFlowGraph-Modeling-for-Logic-Circuits

> Scheduling plays an important role in the VLSI design flow because it determines the amount of concurrency of 
the resulting implementing thus impacts the performance. It also plays an important role in determine the amount 
of hardware resources required thus impacts the area.

> This paper talks about designing and generating synthesizable high level state machine description from the Data flow graph
in Verilog while providing scheduling alternatives like LIST_L and LIST_R using modeling Paradigm. The user can construct Data 
flow graph (for ex data flow graph for 6th order FIR filter) using Atoms. Atoms can be logical operators such as adder, 
subtractor and multipliers etc. Atoms act as graph nodes and the edges can be used to define the relationship between 
them in the Data flow graph.

> Once the user construct the data flow graph, equivalent high level state machine description which adheres to the chosen 
scheduling algorithm will be generated as Verilog code. The Verilog code generated can be synthesizable on Xilinx.

## INTERPRETER DESIGN

Interpreter for this project will be developed using Object Oriented C++ Programming which  involves two main functionalities
i) Navigating the object network ii) Querying for Individual objects.  The object network is traversed, typically the
containment relationship, inheritance, connections are traversed to build the dependency equation from the Data Flow Graph. 
During traversal, objects are queried to get the attributes. Once the dependency equations are built, 
it is used to generate the HLSM description in Verilog of the chosen scheduling algorithm

## RESULTS AND CONCLUSION

A unique approach to model the data flow graph with GME modeling environment has been proposed and developed.
It gives the user to test the series of computation which can be scheduled by specifying latency and resource constraints,
without the onerous task of implementing the algorithms. With this modeling approach, user can able to determine the amount
of concurrency that can be achieved through timing analysis from the generated HLSM Verilog code as shown in figure 7. Along
with this, user will have opportunity to determine resource utilization since the HLSM code generated is synthesizable in
Xilinx; the utilization report obtained is as shown in figure 6.
 
> Overall it’s a unique approach which can be used to determine the amount of concurrency of the resulting implementing thus 
impacts the performance. It also can be used to determine the amount of hardware resources required thus impacts the area. 
So these results in optimal design of circuits by determining amount of concurrent operations and hardware utilization for 
the operations scheduled without the cost of implementing List heuristic algorithms for the domain expert.
