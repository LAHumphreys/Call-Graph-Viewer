#include "callgrindTree.h"
#include "csv.h"
#include "path.h"
#include "stdReader.h"

using namespace std;

// id, name, path
using DataFile = CSV<int,string,Path>;
static const size_t ID = 0;
static const size_t FN_NAME = 1;
static const size_t PATH = 2;

// id, name, path
using CallsFile = CSV<int,int,int,long>;
static const size_t CALLER = 0;
static const size_t CALLED = 1;
static const size_t CALLS  = 2;
static const size_t COST   = 3;

// id, cost
using CostsFile = CSV<int,long>;
static const size_t TOTAL_COST = 1;

CallgrindCallTree::CallgrindCallTree (
    const string& fname)
       : root()
{
    IFStreamReader reader(fname.c_str());
    DataFile funcs(DataFile::LoadCSV(reader));

    for( int i =0; i< funcs.Rows(); ++i) {
        NodePtr node = root.CreateNode(
                               funcs.GetCell<PATH>(i),
                               funcs.GetCell<FN_NAME>(i));
        idMap.emplace(funcs.GetCell<ID>(i),node);
    }
}

void CallgrindCallTree::LoadCalls(const string& fname) {
    IFStreamReader reader(fname.c_str());
    CallsFile calls(CallsFile::LoadCSV(reader));

    /*
     * Loop over eached called function and add the count
     * / cost data.
     *
     * Since we have already graphed everything we can
     * actualyl discard the caller information (its
     * implicit)
     */
    for( int i =0; i< calls.Rows(); ++i) {
        auto it = idMap.find(calls.GetCell<CALLED>(i));
        if ( it != idMap.end() ) {
            NodePtr called = it->second;
            called->AddCall( calls.GetCell<COST>(i),
                             calls.GetCell<CALLS>(i));
        }
    }
}

void CallgrindCallTree::LoadCosts(const string& fname) {
    IFStreamReader reader(fname.c_str());
    CostsFile costs(CostsFile::LoadCSV(reader));

    /*
     * Loop over eached called function and add the count
     * / cost data.
     *
     * Since we have already graphed everything we can
     * actualyl discard the caller information (its
     * implicit)
     */
    for( int i =0; i< costs.Rows(); ++i) {
        auto it = idMap.find(costs.GetCell<ID>(i));
        if ( it != idMap.end() ) {
            NodePtr called = it->second;
            long cost = costs.GetCell<TOTAL_COST>(i);
            counter.AddCall( called->Name(), cost, 0);
        }
    }

    // Now we need to get the call count...
    root.ForEach([=] ( NodePtr&& node ) -> void {
        this->AddCalls(node);
    });
}

void CallgrindCallTree::AddCalls(NodePtr node) {
    counter.AddCall(node->Name(),
                    0,
                    node->Calls());
    node->ForEach([=] ( NodePtr&& node ) -> void {
        this->AddCalls(node);
    });
}
