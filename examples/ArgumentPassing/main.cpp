#include <initializer_list>
#include <vector>
#include <utility>

int basic(const int& i1, 
        const int& i2, 
        const int& i3, 
        const int& i4, 
        const int& i5);
int c_style(const int& i1, 
        const int& i2, 
        const int& i3, 
        const int& i4, 
        const int& i5);
int vector(const int& i1, 
        const int& i2, 
        const int& i3, 
        const int& i4, 
        const int& i5);
int initializer(const int& i1, 
        const int& i2, 
        const int& i3, 
        const int& i4, 
        const int& i5);
int array(const int& i1, 
        const int& i2, 
        const int& i3, 
        const int& i4, 
        const int& i5);
int templ(const int& i1, 
        const int& i2, 
        const int& i3, 
        const int& i4, 
        const int& i5);

int main(int argc, const char *argv[])
{
    int i[5] = {0,0,0,0,0};
    int s = 0;
    s += basic(i[0],i[1],i[2],i[3],i[4]);
    s += c_style(i[0],i[1],i[2],i[3],i[4]);
    s += array(i[0],i[1],i[2],i[3],i[4]);
    s += vector(i[0],i[1],i[2],i[3],i[4]);
    s += initializer(i[0],i[1],i[2],i[3],i[4]);
    s += templ(i[0],i[1],i[2],i[3],i[4]);
    return 0;
}
