Call-Graph-Viewer
==================

A simple tool for profiling processes based on callgraph data.

It already provides:
* A flat view of your most expensive functions
* A complete calltree, including costs / callcount at each node
* regex powered searching of both the call tree and the flat view 
* Callgrind integration:
  * Easily profile C / C++ and similar languages 
  * (Unlike some viewers), The full call tree (using --separate-callers with callgrind)
* Simple CSV based input allows manual profile data to be used (e.g see the TCL pluging provided)

Comming Soon:
* Shortened C++ names for simpler navigating
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
  

Below we investigate why "Put" is taking so long, and discover we are resizing every time!
```
|BinaryReader::Read> search DataVector::Put(long, unsigned char)
 --> BinaryWriter::Write(BinaryReader const&, long)/DataVector::Put(long, unsigned char) : 43840 / 160 (274)
     BinaryWriter::Write(BinaryReader const&, long)/DataVector::Put(long, unsigned char) : 20824 / 76 (274)
     BinaryWriter::Write(BinaryReader const&, long)/DataVector::Put(long, unsigned char) : 13152 / 48 (274)
There are 2 more results

|DataVector::Put> ls
  DataVector::Put(long, unsigned char)
      Calls: 160, Time: 43840, Av. Time: 274
      std::vector<unsigned char, std::allocator<unsigned char> >::resize(unsigned long)
          Calls: 160, Time: 33280, Av. Time: 208
      DataVector::operator[](long)
          Calls: 160, Time: 3680, Av. Time: 23
      std::vector<unsigned char, std::allocator<unsigned char> >::size() const
          Calls: 160, Time: 1920, Av. Time: 12

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
