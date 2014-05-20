
int double_it( int x ) {
    return 2 * x;
}

int quad_it( int x ) {
    return double_it(double_it(x));
}

int main(int argc, const char *argv[])
{
    int i =0;
    for ( i = 0; i <100; i ++ ) {
        double_it(i);
    }
    for ( i = 0; i <100; i ++ ) {
        quad_it(i);
    }
    return 0;
}
