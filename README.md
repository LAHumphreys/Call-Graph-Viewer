Call-Graph-Viewer [![Build Status](https://travis-ci.org/Grauniad/Call-Graph-Viewer.png)](https://travis-ci.org/Grauniad/Call-Graph-Viewer) [![Coverage Status](https://coveralls.io/repos/Grauniad/Call-Graph-Viewer/badge.png?branch=master)](https://coveralls.io/r/Grauniad/Call-Graph-Viewer?branch=master)
==================
!["Example screen shot showing the annotaiton, list and tree views"](https://raw.githubusercontent.com/Grauniad/Call-Graph-Viewer/master/ExampleScreenshot.png "Example screen shot showing the annotaiton, list and tree view in a split-pane mode")


A simple tool for profiling processes based on callgraph data.

It already provides:
* A flat view of your most expensive functions
* A complete calltree, including costs / callcount at each node
* regex powered searching of both the call tree and the flat view 
* Code annotation
* Callgrind integration:
  * Easily profile C / C++ and similar languages 
  * (Unlike some viewers), The full call tree (using --separate-callers with callgrind)
* Simple CSV based input allows manual profile data to be used (e.g see the TCL pluging provided)

Comming Soon:
* readline integration
* Graphical front end for viewing the data

Flat View
---------
The flat view allows you to quickly identify bottle necks in your code by ranking functions by the cost.

(If you're function names have been truncated, try the "widetable" command.)
```
|ROOT> table 10
-----------------------------------------------------------------------------------------------------------
|-               Most Time Spent in Function                                                             -|
-----------------------------------------------------------------------------------------------------------
|    Function Name                                                      | Calls  | Reads (Ir) | Ir/call   |
-----------------------------------------------------------------------------------------------------------
| main                                                                  | 0      | 1147387    | 0         |
| CallgrindCallTree::CallgrindCallTree(std::string const&)              | 1      | 576200     | 576200    |
| CSV<int, std::string, Path>::LoadCSV(BinaryReader)                    | 1      | 480685     | 480685    |
| bool boost::escaped_list_separator<char, std::char_traits<char> >::...| 59     | 323271     | 5479      |
| CallgrindCallTree::LoadCalls(std::string const&)                      | 1      | 279234     | 279234    |
| strcmp                                                                | 3252   | 274154     | 84        |
| CSV<int, int, int, long>::LoadCSV(BinaryReader)                       | 1      | 266798     | 266798    |
| CSV<int, std::string, Path>::NewRow(boost::tokenizer<boost::escaped...| 7      | 259203     | 37029     |
| boost::detail::postfix_increment_result<boost::token_iterator<boost...| 39     | 237815     | 6097      |
| BinaryReader::Read(BinaryWriter&, long) const                         | 20     | 221750     | 11087     |
-----------------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------------
|-               Most Expensive Function Calls                                                           -|
-----------------------------------------------------------------------------------------------------------
|    Function Name                                                      | Calls  | Reads (Ir) | Ir/call   |
-----------------------------------------------------------------------------------------------------------
| CallgrindCallTree::CallgrindCallTree(std::string const&)              | 1      | 576200     | 576200    |
| CSV<int, std::string, Path>::LoadCSV(BinaryReader)                    | 1      | 480685     | 480685    |
| CallgrindCallTree::LoadCalls(std::string const&)                      | 1      | 279234     | 279234    |
| CSV<int, int, int, long>::LoadCSV(BinaryReader)                       | 1      | 266798     | 266798    |
| CallgrindCallTree::LoadCosts(std::string const&)                      | 1      | 215861     | 215861    |
| CSV<int, long>::LoadCSV(BinaryReader)                                 | 1      | 186308     | 186308    |
| CSV<int, std::string, Path>::NewRow(boost::tokenizer<boost::escaped...| 7      | 259203     | 37029     |
| DataVector::DataVector(long)                                          | 3      | 110029     | 36676     |
| std::enable_if<(2)!=(0), void>::type CSV<int, std::string, Path>::A...| 7      | 208432     | 29776     |
| CSV<int, int, int, long>::NewRow(boost::tokenizer<boost::escaped_li...| 6      | 136249     | 22708     |
-----------------------------------------------------------------------------------------------------------

```
You can also search the flat view. In this example we find all constant member functions of the BinaryReader object:
```
    |ROOT> searchtable ^BinaryReader::.* const$
    Searching flat table for: '^BinaryReader::.* const$'
                     Most Time Spent in Function
                   ===============================
      Calls      Reads Ir      Ir/call        Name
    ---------  -----------   -------------  --------
     278        8340          30             BinaryReader::End() const
     274        3014          11             BinaryReader::Get() const
     2          2506          1253           BinaryReader::Dup(long) const
     136        2448          18             BinaryReader::Pos(long) const
     2          2273          1136           BinaryReader::ReadLine(BinaryWriter&, long, char) const
     1          1879          1879           BinaryReader::AppendString(std::string&) const
     1          1669          1669           BinaryReader::ReadString() const
     2          1490          745            BinaryReader::Read(BinaryWriter&, long) const
     2          1396          698            BinaryReader::ReadLine(void*, long, char) const
     23         861           37             BinaryReader::Read(void*, long) const
     2          817           408            BinaryReader::Find(unsigned char) const
     2          653           326            BinaryReader::ReadString(std::string&) const
     281        562           2              BinaryReader::Offset() const
     276        552           2              BinaryReader::operator long() const
     11         231           21             BinaryReader::Begin() const
     2          195           97             BinaryReader::RFind(unsigned char) const

                     Most Expensive Function Calls
                   =================================
      Calls      Reads Ir      Ir/call        Name
    ---------  -----------   -------------  --------
     1          1879          1879           BinaryReader::AppendString(std::string&) const
     1          1669          1669           BinaryReader::ReadString() const
     2          2506          1253           BinaryReader::Dup(long) const
     2          2273          1136           BinaryReader::ReadLine(BinaryWriter&, long, char) const
     2          1490          745            BinaryReader::Read(BinaryWriter&, long) const
     2          1396          698            BinaryReader::ReadLine(void*, long, char) const
     2          817           408            BinaryReader::Find(unsigned char) const
     2          653           326            BinaryReader::ReadString(std::string&) const
     2          195           97             BinaryReader::RFind(unsigned char) const
     23         861           37             BinaryReader::Read(void*, long) const
     278        8340          30             BinaryReader::End() const
     11         231           21             BinaryReader::Begin() const
     136        2448          18             BinaryReader::Pos(long) const
     274        3014          11             BinaryReader::Get() const
     276        552           2              BinaryReader::operator long() const
     281        562           2              BinaryReader::Offset() const
```
Searching for Calls
--------------------
If you have identified a troublesome function from the flat view, you can search for points in the code where it is called.

There are two search commands supported:
 * search (s): A fast, cached search of the entire tree. The full name must be provided
 * searchchildren (sc): Searches children of the active node. Regex pattern may be provided.
    * searchroot (sr): Invoke searchchildren on the root node.
  

Below we investigate why "Put" is taking so long, and discover we are resizing nearly every time!
```
|main> searchroot DataVector::Put.*
 --> validatePutAndGrow(DefaultTestLogger&)/DataVector::Put(long, unsigned char) : 1837 / 4 (459)
     validatePut(DefaultTestLogger&)/DataVector::Put(long, unsigned char) : 59 / 1 (59)
There are 1 more results

|DataVector::Put> ls
DataVector::Put(long, unsigned char)
    Calls: 4, Time: 1837, Av. Time: 459

                 Most Time Spent in Function
               ===============================
  Calls      Reads Ir      Ir/call        Name
---------  -----------   -------------  --------
 3          1580          526            std::vector<unsigned char, std::allocator<unsigned char> >::resize(unsigned long)
 4          92            23             DataVector::operator[](long)
 4          48            12             std::vector<unsigned char, std::allocator<unsigned char> >::size() const


                 Most Expensive Function Calls
               =================================
  Calls      Reads Ir      Ir/call        Name
---------  -----------   -------------  --------
 3          1580          526            std::vector<unsigned char, std::allocator<unsigned char> >::resize(unsigned long)
 4          92            23             DataVector::operator[](long)
 4          48            12             std::vector<unsigned char, std::allocator<unsigned char> >::size() const

|DataVector::Put> annotate
   23:            ( 0%): 
   24:         28 ( 1%): void DataVector::Put(long offset, unsigned char c) {
   25:         80 ( 4%):     if (offset >= static_cast<long>(this->size()))
   26:       1601 (87%):         this->resize(offset+1);
   27:        120 ( 6%):     (*this)[offset] = c;
   28:          8 ( 0%): }
   29:            ( 0%): 

```
Here we use searchchildren to find all calls to LoadCSV, regardless of template parameters:

```
|ROOT> sc CSV<.*>::LoadCSV.*
 --> CallgrindCallTree::CallgrindCallTree(std::string const&)/CSV<int, std::string, Path>::LoadCSV(BinaryReader) : 115264814 / 1 (115264814)
     CallgrindCallTree::LoadCosts(std::string const&)/CSV<int, long>::LoadCSV(BinaryReader) : 10241080 / 1 (10241080)
     CallgrindCallTree::LoadCalls(std::string const&)/CSV<int, int, int, long>::LoadCSV(BinaryReader) : 21348087 / 1 (21348087)
There are 2 more results

```
n(ext) and p(revious) navigate the results:
```
|DataVector::Put> search DataVector::Put(long, unsigned char)
 --> BinaryWriter::Write(BinaryReader const&, long)/DataVector::Put(long, unsigned char) : 43840 / 160 (274)
     BinaryWriter::Write(BinaryReader const&, long)/DataVector::Put(long, unsigned char) : 20824 / 76 (274)
     BinaryWriter::Write(BinaryReader const&, long)/DataVector::Put(long, unsigned char) : 13152 / 48 (274)
There are 2 more results

|DataVector::Put> pwd
ROOT/
  main/
  CallgrindCallTree::CallgrindCallTree(std::string const&)/
  CSV<int, std::string, Path>::LoadCSV(BinaryReader)/
  BinaryReader::Read(BinaryWriter&, long) const/
  BinaryWriter::Write(BinaryReader const&, long)/
  DataVector::Put(long, unsigned char)

|DataVector::Put> next
     BinaryWriter::Write(BinaryReader const&, long)/DataVector::Put(long, unsigned char) : 43840 / 160 (274)
 --> BinaryWriter::Write(BinaryReader const&, long)/DataVector::Put(long, unsigned char) : 20824 / 76 (274)
     BinaryWriter::Write(BinaryReader const&, long)/DataVector::Put(long, unsigned char) : 13152 / 48 (274)
There are 1 more results

|DataVector::Put> pwd
ROOT/
  main/
  CallgrindCallTree::LoadCalls(std::string const&)/
  CSV<int, int, int, long>::LoadCSV(BinaryReader)/
  BinaryReader::Read(BinaryWriter&, long) const/
  BinaryWriter::Write(BinaryReader const&, long)/
  DataVector::Put(long, unsigned char)
```
Annotating the source code
--------------------
A single annotation command is provided
 * annotate (a): (where available) Display the code for this function with call cost annotations

In this example we profile the part of the class which handles callgrind files:

```
lhumphreys@localhost ~/Documents/Profiler $ ./profile gather.out 
Building call graph...done
Process started up in 27 mili-seconds

|ROOT> sr .*SetCurrent.*
 --> CallgrindNative::CallgrindNative(std::string const&)/CallgrindNative::SetCurrentFunction(std::string const&) : 17750144 / 3040 (5838)
There are 0 more results

|CallgrindNative::SetCurrentFunction> a
  191:            ( 0%): 
  192:            ( 0%): //        01234
  193:            ( 0%): // Format fn=(<ID>) [fname'parent'...'root]
  194:      42560 ( 0%): void CallgrindNative::SetCurrentFunction ( const std::string& line) {
  195:            ( 0%):     size_t id_end = line.find_first_of(')');
  196:      76000 ( 1%):     int id = atoi(line.substr(4,(id_end - 4)).c_str());
  197:            ( 0%):     auto it = idMap.find(id);
  198:     359934 ( 6%):     Path path("");
  199:       6080 ( 0%):     if ( it == idMap.end() ) {
  200:       9768 ( 0%):         if ( line.find_first_of(' ') != string::npos ) {
  201:            ( 0%):             // Haven't seen this before, need the path...
  202:            ( 0%):             size_t index = id_end;
  203:       6040 ( 0%):             size_t last_index = line.length()-1;
  204:     156381 ( 2%):             bool foundMain = false;
  205:      56075 ( 1%):             for ( index = line.find_last_of('\'');
  206:            ( 0%):                   index != string::npos;
  207:            ( 0%):                   index = line.find_last_of('\'',index-1))
  208:            ( 0%):             {
  209:     112146 ( 2%):                 string&& token = line.substr(index+1,(last_index-index));
  210:            ( 0%): 
  211:            ( 0%):         SLOG_FROM(LOG_VERBOSE, "CallgrindNative::SetCurrentFunction", "Token:...
  212:      13641 ( 0%):                 last_index = index-1;
  213:      88756 ( 1%):                 if ( !foundMain && token == "main" ) {
  214:            ( 0%):                     foundMain = true;
  215:            ( 0%):                 }
  216:            ( 0%):                 if ( foundMain ) {
  217:            ( 0%):                     path.Extend(token);
  218:            ( 0%):                 }
  219:            ( 0%):             }
  220:            ( 0%): 
  221:            ( 0%):             // Finally pick up the actual function name
  222:      16610 ( 0%):             string name(line.substr(id_end + 2,(last_index+1)-(id_end +2)));
  223:       7174 ( 0%):             if ( foundMain || name == "main" ) {
  224:            ( 0%):                 // Place the node in the graph
  225:       3402 ( 0%):                 if ( name == "main" ) {
  226:            ( 0%):                     current = root.CreateNode(Path(""),name);
  227:            ( 0%):                 } else {
  228:    4156788 (79%):                     current = root.CreateNode(path,name);
  229:            ( 0%):                 }
  230:            ( 0%): 
  231:            ( 0%):                 // Link the ID to the node
  232:       4536 ( 0%):                 idMap.emplace(id,current);
  233:            ( 0%): 
  234:            ( 0%):                 // Link the node to the source file...
  235:      23357 ( 0%):                 current->SourceId() = currentFile;
  236:            ( 0%):                  
  237:            ( 0%):                 SLOG_FROM(LOG_VERBOSE,"CallgrindNative::SetCurrentFunction", ...
  238:            ( 0%):             } else {
  239:        752 ( 0%):                 SLOG_FROM(LOG_VERBOSE,"CallgrindNative::SetCurrentFunction", ...
  240:            ( 0%):             }
  241:            ( 0%):         }
  242:            ( 0%):     } else {
  243:            ( 0%):         // Alreay know about this function - nothing else to do...
  244:       2352 ( 0%):         current = it->second;
  245:            ( 0%): 
  246:       2352 ( 0%):         currentFile = current->SourceId();
  247:      71979 ( 1%):         childFile = currentFile;
  248:            ( 0%): 
  249:            ( 0%):         SLOG_FROM(LOG_VERBOSE, "CallgrindNative::SetCurrentFunction", "Curren...
  250:            ( 0%):     }
  251:      35588 ( 0%): }

```

Viewing the call tree
--------------------
The tree view lets you identify bottle necks by seeing where your time is being spent. Items are sorted by the total time spent in them.

Because the viewer builds a complete tree it won't distract you with child functions unless they were invoked 
from this point in the call graph:

The argument is the depth below the current node you want to print (default is the full tree).

```
|BinaryReader::Read> tree 3
  BinaryReader::Read(BinaryWriter&, long) const
      Calls: 7, Time: 124638, Av. Time: 17805
      BinaryWriter::Write(BinaryReader const&, long)
          Calls: 7, Time: 124281, Av. Time: 17754
          BinaryReader::Get() const
              Calls: 160, Time: 69442, Av. Time: 434
              StdReader::Get(long) const
                  Calls: 160, Time: 66562, Av. Time: 416
          DataVector::Put(long, unsigned char)
              Calls: 160, Time: 43840, Av. Time: 274
              std::vector<unsigned char, std::allocator<unsigned char> >::resize(unsigned long)
                  Calls: 160, Time: 33280, Av. Time: 208
              DataVector::operator[](long)
                  Calls: 160, Time: 3680, Av. Time: 23
              std::vector<unsigned char, std::allocator<unsigned char> >::size() const
                  Calls: 160, Time: 1920, Av. Time: 12
          BinaryReader BinaryReader::operator+<long>(long) const
              Calls: 160, Time: 6400, Av. Time: 40
              BinaryReader::BinaryReader(FileLikeReader const&, long const&)
                  Calls: 160, Time: 2880, Av. Time: 18
      Logger::Instance()
          Calls: 7, Time: 98, Av. Time: 14
      Logger::LogEnabled(LOG_LEVEL)
          Calls: 7, Time: 70, Av. Time: 10
```

Navigating the tree
-------------------
The primary navigation tool is the search function (see above), but you can also navigate the tree with shell style commands:
```
|ROOT> search Time::Time(std::string const&)
Building one time search cache.. 
done
Search cache built in 0 seconds
 --> void std::vector<Time, std::allocator<Time> >::_M_emplace_back_aux<std::string const&>(std::string const&)/Time::Time(std::string const&) : 178177 / 5 (35635)
     CSV<Time, std::string, std::string>::NewRow(boost::tokenizer<boost::escaped_list_separator<char, std::char_traits<char> >, __gnu_cxx::__normal_iterator<char const*, std::string>, std::string> const&)/Time::Time(std::string const&) : 352266 / 11 (32024)
There are 1 more results

|Time::Time> ls
  Time::Time(std::string const&)
      Calls: 5, Time: 178177, Av. Time: 35635
      Time::operator=(std::string const&)
          Calls: 5, Time: 178172, Av. Time: 35634


|Time::Time> cd Time::operator=(std::string const&)/mktime

|mktime> pwd
ROOT/
  main/
  CallProfile::ProcessFile(std::string const&)/
  ProfDataFile::ProfDataFile(std::string const&)/
  CSV<Time, std::string, std::string>::LoadCSV(BinaryReader)/
  CSV<Time, std::string, std::string>::NewRow(boost::tokenizer<boost::escaped_list_separator<char, std::char_traits<char> >, __gnu_cxx::__normal_iterator<char const*, std::string>, std::string> const&)/
  void std::vector<Time, std::allocator<Time> >::_M_emplace_back_aux<std::string const&>(std::string const&)/
  Time::Time(std::string const&)/
  Time::operator=(std::string const&)/
  mktime
  
|mktime> cd ..

|Time::operator=> pwd
ROOT/
  main/
  CallProfile::ProcessFile(std::string const&)/
  ProfDataFile::ProfDataFile(std::string const&)/
  CSV<Time, std::string, std::string>::LoadCSV(BinaryReader)/
  CSV<Time, std::string, std::string>::NewRow(boost::tokenizer<boost::escaped_list_separator<char, std::char_traits<char> >, __gnu_cxx::__normal_iterator<char const*, std::string>, std::string> const&)/
  void std::vector<Time, std::allocator<Time> >::_M_emplace_back_aux<std::string const&>(std::string const&)/
  Time::Time(std::string const&)/
  Time::operator=(std::string const&)


```
