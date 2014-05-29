#include <initializer_list>
#include <vector>
#include <array>
#include <utility>
#include <cstdarg>

/**************************************
 *          Basic Sum: 6 Reads (x1.0)
 *************************************/
int sum(const int& i1, 
        const int& i2, 
        const int& i3, 
        const int& i4, 
        const int& i5) 
{
    return i1 + i2 + i3 + i4 + i5;
}

int basic(const int& i1, 
            const int& i2, 
            const int& i3, 
            const int& i4, 
            const int& i5) 
{
    return sum(i1,i2,i3,i4,i5);
}

/**************************************
 * c-style var args: 85 Reads (x14.1)
 *************************************/
int varsum(const int& n, ...) 
{
    va_list vl;
    va_start(vl,n);
    int s = 0;
    for ( int i = 0; i< n; ++i) {
        s += va_arg(vl,int);
    }
    va_end(vl);
    return s;
}

int c_style(const int& i1, 
            const int& i2, 
            const int& i3, 
            const int& i4, 
            const int& i5) 
{
    return varsum(5,i1,i2,i3,i4,i5);
}

/**************************************
 *          Templates: 6 Reads (x1.0)
 *************************************/
template <typename T>
int tsum(T&& x) {
    return std::forward<T>(x);
}
template <typename T, typename ...Args>
int tsum(T x, Args... args) {
    return std::forward<T>(x) 
            + tsum(std::forward<Args>(args)...);
}

int templ(const int& i1, 
            const int& i2, 
            const int& i3, 
            const int& i4, 
            const int& i5) 
{
    return tsum(i1,i2,i3,i4,i5);
}

/**************************************
 *          Array: 11 (x1.8)
 *************************************/

template< int N>
int asum(const std::array<int,N>& arr) {
    int s = 0;
    for ( const int& i : arr ) {
        s+= i;
    }
    return s;
}

int array(const int& i1, 
            const int& i2, 
            const int& i3, 
            const int& i4, 
            const int& i5) 
{
    return asum<5>({i1,i2,i3,i4,i5});
}

/**************************************
 *          Initializer : 39 (x6.5)
 *************************************/
int isum(std::initializer_list<int> list) {
    int s = 0;
    for ( const int& i : list ) {
        s+= i;
    }
    return s;
}


int initializer(const int& i1, 
            const int& i2, 
            const int& i3, 
            const int& i4, 
            const int& i5) 
{
    return isum({i1,i2,i3,i4,i5});
}

/**************************************
 *       Vector: 62855 (x10475.8)
 *************************************/

int vsum(std::vector<int>&& vec ) {
    int s = 0;
    for ( const int& i : vec ) {
        s+= i;
    }
    return s;
}


int vector(const int& i1, 
            const int& i2, 
            const int& i3, 
            const int& i4, 
            const int& i5) 
{
    return vsum({i1,i2,i3,i4,i5});
}
