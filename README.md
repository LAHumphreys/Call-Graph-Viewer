Call-Graph-Viewer
==================

A simple tool for profiling processes based on callgraph data.

It already provides:
* A flat view of your most expensive functions
* Ability to navigate and view the callgraph
* Callgrind integration:
  * Easily profile C / C++ and similary languages 
  * (Unlike some viewers), The full call tree (using --separate-callers with callgrind)
* Simple CSV based input allows manual profile data to be used (e.g see the TCL pluging provided)

Comming Soon:
* Shortened C++ names for simpler navigating
* readline integration
* Graphical front end for viewing the data

Flat View
---------
The flat view allows you to quickly identify bottle necks in your code by ranking functions by the cost
```
lhumphreys@localhost ~/Documents/Profiler $ ./readCallGrind.sh ../../test/callgrind.out.8104 

|ROOT> table
------------------------------------------------------------------------
|-               Most Time Spent in Function                          -|
------------------------------------------------------------------------
|    Function Name                   | Calls  | Time(us)   | us/call   |
------------------------------------------------------------------------
| main                               | 0      | 152        | 0         |
| div                                | 5      | 87         | 17        |
| odds                               | 1      | 75         | 75        |
| evens                              | 1      | 65         | 65        |
| pos_div2                           | 1      | 7          | 7         |
| pos_div4                           | 1      | 7          | 7         |
------------------------------------------------------------------------

------------------------------------------------------------------------
|-               Most Expensive Function Calls                        -|
------------------------------------------------------------------------
|    Function Name                   | Calls  | Time(us)   | us/call   |
------------------------------------------------------------------------
| odds                               | 1      | 75         | 75        |
| evens                              | 1      | 65         | 65        |
| div                                | 5      | 87         | 17        |
| pos_div2                           | 1      | 7          | 7         |
| pos_div4                           | 1      | 7          | 7         |
| main                               | 0      | 152        | 0         |
------------------------------------------------------------------------
```

Searching for Calls
--------------------
If you have identified a troublesome function from the flat view, you can search for points in the code where it is called:
```
|ROOT> search div
 --> odds/div : 45 / 3 (15)
     evens/div : 42 / 2 (21)
There are 1 more results
|div> next
     odds/div : 45 / 3 (15)
 --> evens/div : 42 / 2 (21)
There are 0 more results
|div> pwd
ROOT/main/evens/div
```

Viewing the call tree
--------------------
The tree view lets you identify bottle necks by seeing where your time is being spent. 

Because the viewer builds a complete tree it won't distract you with child functions unless they were invoked 
from this point in the call graph:
```
|main> tree 
  main
      Calls: 0, Time: 0, Av. Time: 0
      odds
          Calls: 1, Time: 75, Av. Time: 75
          div
              Calls: 3, Time: 45, Av. Time: 15
      evens
          Calls: 1, Time: 65, Av. Time: 65
          div
              Calls: 2, Time: 42, Av. Time: 21
              pos_div2
                  Calls: 1, Time: 7, Av. Time: 7
              pos_div4
                  Calls: 1, Time: 7, Av. Time: 7
```

Navigating the tree
-------------------
The primary navigation tool is the search function (see above), but you can also navigate the tree with shell style commands:
```
|div> n
     odds/div : 45 / 3 (15)
 --> evens/div : 42 / 2 (21)
There are 0 more results

|div> pwd
ROOT/main/evens/div

|div> cd ..

|evens> ls
  evens
      Calls: 1, Time: 65, Av. Time: 65
      div
          Calls: 2, Time: 42, Av. Time: 21


|evens> cd div/pos_div2

|pos_div2> pwd
ROOT/main/evens/div/pos_div2

```
