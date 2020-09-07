#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "list.h"

//funkcja generujaca baze liczb pierwszych
void create_base(List *base, const mpz_t *n, const unsigned long *bound){
    mpz_t i;
    mpz_init(i);
    mpz_set_ui(i,2);
    Data data;

    while(mpz_cmp_ui(i,*bound)<=0){
        if(mpz_legendre(*n,i)==1) {
            data.ul=mpz_get_ui(i);
            ListAdd(data, base);
        }
        mpz_add_ui(i,i,1);
        while(!mpz_probab_prime_p(i,30))
            mpz_add_ui(i,i,1);
    }
    mpz_clear(i);
}

unsigned long ul_pow_mod(unsigned long a, unsigned long b, unsigned long c){
    mpz_t temp;
    mpz_init(temp);
    mpz_ui_pow_ui(temp,a,b);
    mpz_mod_ui(temp,temp,c);
    unsigned long ret = mpz_get_ui(temp);
    mpz_clear(temp);
    return ret;
}

unsigned long legendre(unsigned long a, unsigned long p){
    if(a % p == 0){
        return 0;
    }else{
        return ul_pow_mod(a, (p-1)/2, p);
    }
}

unsigned long tonelli(mpz_t n, unsigned long p){
    unsigned long q = p-1, s=0, z=2, i=1;
    mpz_t mpz_p;
    mpz_init_set_ui(mpz_p,p);
    mpz_t wynik;
    mpz_init(wynik);
    while(q % 2 == 0){
        q /=2;
        s += 1;
    }
    if(s == 1){
        mpz_powm_ui(wynik,n,(p+1)/4,mpz_p);
        unsigned long ret = mpz_get_ui(wynik);
        mpz_clear(wynik);
        mpz_clear(mpz_p);
        return ret;
    }
    while(z < p && p-1 != legendre(z,p)){
        z += 1;
    }
    unsigned long c = ul_pow_mod(z,q,p);
    mpz_powm_ui(wynik,n,(q+1)/2,mpz_p);
    unsigned long r = mpz_get_ui(wynik);
    mpz_powm_ui(wynik,n,q,mpz_p);
    unsigned long t = mpz_get_ui(wynik);
    unsigned long m = s;
    unsigned long t2 = 0;

    while((t-1) % p != 0){
        t2 = (t*t)%p;
        i=1;
        while (i<m){
            if ((t2-1) % p == 0)
                break;
            t2 = (t2 * t2) % p;
            i++;
        }
        unsigned long b = ul_pow_mod(c, 1 << (m-i-1), p);
        r = (r*b) % p;
        c = (b*b) % p;
        t = (t*c) % p;
        m=i;
    }

    mpz_clear(mpz_p);
    mpz_clear(wynik);
    return r;
}

void solve(unsigned long start_vals[][2], mpz_t *b, mpz_t *n, unsigned long *base, unsigned long needed) {
    unsigned long r1, r2;
    mpz_t start1, start2, wynik;
    mpz_init(start1);
    mpz_init(start2);
    mpz_init(wynik);

    for (int i = 0; i < needed; ++i) {
        r1 = tonelli(*n, base[i]);
        long temp = -1 * r1;
        if(temp < 0)
            temp += base[i];
        r2 = temp % base[i];

        mpz_sub_ui(wynik, *b, r1);
        mpz_neg(wynik,wynik);
        mpz_tdiv_r_ui(start1, wynik, base[i]);
        if (mpz_cmp_ui(start1, 0) < 0) {
            mpz_add_ui(start1, start1, base[i]);
        }

        mpz_sub_ui(wynik, *b, r2);
        mpz_neg(wynik,wynik);
        mpz_tdiv_r_ui(start2,wynik,base[i]);
        if(mpz_cmp_ui(start2,0)<0)
            mpz_add_ui(start2,start2,base[i]);
        start_vals[i][0]=mpz_get_ui(start1);
        start_vals[i][1]=mpz_get_ui(start2);
    }
    mpz_clear(start1);
    mpz_clear(start2);
    mpz_clear(wynik);
}

//funkcje wykorzystywane przy obliczeniu gornego zakresu liczby B-gladkiej
double bigNumLog2(mpz_t n){
    signed long int exp;
    const double m = mpz_get_d_2exp(&exp, n);
    return log2(m) + log2(2) * (double) exp;
}

unsigned long getBSmooth(mpz_t n){
    unsigned long wynik;
    double x, loge2, lnn, lnlnn;

    loge2 = 0.69314781;
    lnn = bigNumLog2(n) * loge2;
    lnlnn = log2(lnn) * loge2;
    x = ceil(lnn * lnlnn);
    x = sqrt(x) * 0.5;
    x = pow(2.71, x);

    wynik = ceil(x);
    return wynik;
}

void poly(mpz_t *wynik, unsigned long x, mpz_t b, mpz_t N){
    mpz_set_ui(*wynik,x);
    mpz_add(*wynik,*wynik,b);
    mpz_pow_ui(*wynik,*wynik,2);
    mpz_sub(*wynik,*wynik,N);
}

void trial(unsigned char *ret, mpz_t n, unsigned long *base, unsigned long needed) {
    mpz_t temp,temp_n;
    mpz_init(temp);
    mpz_init_set(temp_n,n);
    if(mpz_cmp_ui(temp_n,0)>0){
        for (int i = 0; i < needed; ++i) {
            mpz_tdiv_r_ui(temp,temp_n,base[i]);
            while (mpz_cmp_ui(temp,0)==0){
                mpz_tdiv_q_ui(temp_n,temp_n,base[i]);
                ret[i]=(ret[i] + 1) % 2;
                mpz_tdiv_r_ui(temp,temp_n,base[i]);
            }
        }
    }
    mpz_clear(temp);
    mpz_clear(temp_n);
}

bool notIn(unsigned char *exp, List list, unsigned long needed) {
    if(ListEmpty(&list))
        return true;
    while(hasNext(list)){
        if(memcmp(exp,list->data.exp,needed*sizeof(char))) {
            list = next(&list);
            continue;
        }
        return false;
    }
    return true;
}

void show(Data dat){
    printf("%lu ",dat.ul);
}

void gauss(unsigned long needed, size_t marks_size, unsigned char M[][needed], bool *marks) {
    for (int j = 0; j < needed; ++j) {
        for (int i = 0; i < marks_size; ++i) {
            if(M[i][j] == 1){
                marks[i]=true;
                for (int k = 0; k < j; ++k) {
                    if (M[i][k] == 1){
                        for (int l = 0; l < marks_size; ++l) {
                            M[l][k] = (M[l][k] + M[l][j]) % 2;
                        }
                    }
                }
                for (int k = j+1; k < needed; ++k) {
                    if (M[i][k] == 1){
                        for (int row = 0; row < marks_size; ++row) {
                            M[row][k] = (M[row][k] + M[row][j]) % 2;
                        }
                    }
                }
                break;
            }
        }
    }
}

void getDepCols(List *ret, const unsigned char *row, unsigned long needed) {
    Data data;
    for (unsigned long i = 0; i < needed; ++i) {
        if (row[i] == 1){
            data.ul = i;
            ListAdd(data, ret);
        }
    }
}

bool isDependent(List cols, const unsigned char *rows) {
    while (hasNext(cols)){
        if (rows[cols->data.ul] == 1)
            return true;
        cols = next(&cols);
    }
    return false;
}

void rowAdd(int new_row, List ret, unsigned long needed, unsigned char M[][needed]) {
    while (hasNext(ret)){
        for (int i = 0; i < needed; ++i) {
            ret->data.exp[i] ^= M[new_row][i];
        }
        ret=next(&ret);
    }
}

bool sum(List list, unsigned long needed) {
    while (hasNext(list)){
        for (int i = 0; i < needed; ++i) {
            if (list->data.exp[i] != 0)
                return false;
        }
        list = next(&list);
    }
    return true;
}

void findLinearDeps(List *ret, unsigned long row, unsigned long needed, unsigned char M[][needed], size_t size) {
    List dep_cols, current_rows, current_sum;
    ListInit(&dep_cols);
    ListInit(&current_rows);
    ListInit(&current_sum);
    Data data;

    getDepCols(&dep_cols, M[row], needed);
    data.ul = row;
    ListAdd(data, &current_rows);
    data.exp = malloc(needed*sizeof(unsigned char));
    memcpy(data.exp,M[row],needed);
    ListAdd(data, &current_sum);
    for (int i = 0; i < size; ++i) {
        if (i==row)
            continue;
        if (isDependent(dep_cols,M[i])){
            data.ul=i;
            ListAdd(data, &current_rows);
            rowAdd(i, current_sum,needed,M);
            List tmp;
            if (sum(current_sum, needed)) {
                tmp = current_rows;
                ListInit(&data.innerList);
                while (hasNext(tmp)) {
                    ListAdd(tmp->data, &data.innerList);
                    tmp = next(&tmp);
                }
                ListAdd(data, ret);
            }
        }
    }
    ListClear(&dep_cols);
    ListClear(&current_rows);
    List wsk = current_sum;
    while (hasNext(wsk)){
        free(wsk->data.exp);
        wsk = next(&wsk);
    }
    ListClear(&current_sum);
}

void testdep(mpz_t *gcd, List dep, List smooth_vals, mpz_t N) {
    mpz_t x,y;
    mpz_init_set_ui(x,1);
    mpz_init_set_ui(y,1);
    while (hasNext(dep)){
        unsigned long i = 0;
        List tmp2 = smooth_vals;
        while (true){
            if (i++ == dep->data.ul){
                mpz_mul(x,x,tmp2->data.smoothVals.vals[0]);
                mpz_mul(y,y,tmp2->data.smoothVals.vals[1]);
                break;
            }
            tmp2 = next(&tmp2);
        }
        dep = next(&dep);
    }
    mpz_sqrt(y,y);
    mpz_sub(x,x,y);
    mpz_gcd(*gcd,N,x);
    mpz_clear(x);
    mpz_clear(y);
}

int main(int argc, char *argv[]) {
    unsigned long over_smooth=20, sieve_interval=100000;
    if(argc<2) {
        fprintf(stderr, "HOWTO - %s [number] [args]\n", argv[0]);
        exit(0);
    }else if(argc>2){
        if(strncmp(argv[2],"--interval=",11)==0) {
            sieve_interval=strtol(&argv[2][11],NULL,10);

            if(argc>3 && strncmp(argv[3],"--over_smooth=",14)==0) {
                over_smooth=strtol(&argv[3][14],NULL,10);
            }
        }else if(strncmp(argv[2],"--over_smooth=",14)==0){
            over_smooth=strtol(&argv[2][14],NULL,10);

            if(argc>3 && strncmp(argv[3],"--interval=",11)==0) {
                sieve_interval=strtol(&argv[3][11],NULL,10);
            }
        }
    }else{
        if(strcmp(argv[1],"--help")==0 || strcmp(argv[1],"-h")==0) {
            puts("Program factors unsigned integer number using quadratic sieve and displays possible solution.");
            puts("It aims to achieve prime number but may fail to do so. In such case you can try adjusting arguments.");
            puts("Possible arguments:");
            puts("--over_smooth=[number] - adds number to bound value (default is 20)");
            puts("--interval=[number] - sets interval value (default is 10 000)");
            puts("--help - displays help");
            exit(0);
        }

    }

    mpz_t N;
    mpz_init_set_str(N,argv[1],10);

    mpz_t b;
    mpz_init(b);
    mpz_sqrt(b,N);
    mpz_add_ui(b,b,1);

    unsigned long bound = getBSmooth(N) + over_smooth;
    printf("%lu\n",bound);

    List baseTemp;
    ListInit(&baseTemp);

    create_base(&baseTemp,&N,&bound);
    List temp1 = baseTemp;

    unsigned long base[ListSize(&baseTemp)];
    for (int i = 0; hasNext(temp1) ; ++i) {
        base[i]=temp1->data.ul;
        temp1=next(&temp1);
    }

    ListClear(&baseTemp);

    unsigned long needed = sizeof(base) / sizeof(unsigned long);
    unsigned long sieve_start=0,sieve_stop=0;
    unsigned long start_vals[needed][2];
//    mpz_t interval[sieve_interval]; this doesn't seem to work with sizes over 100000
    mpz_t *interval;
    interval = malloc(sieve_interval*sizeof(mpz_t));
    for (unsigned long i = 0; i < sieve_interval; ++i) {
        mpz_init(interval[i]);
    }

    solve(start_vals, &b, &N, base, needed);

    List seen, smooth_vals;
    ListInit(&seen);
    ListInit(&smooth_vals);

    mpz_t temp;
    mpz_init(temp);

    printf("==\nSearching for %lu integers which are smooth to the factor base\n==\n\n",needed);

    while(ListSize(&smooth_vals) < needed){
        sieve_start=sieve_stop;
        sieve_stop+=sieve_interval;
        for (unsigned long j = 0,k=sieve_start; j < sieve_interval; ++j,k++) {
            poly(&interval[j],k,b,N);
        }
        for (int p = 0; p < needed; ++p) {
            unsigned long t = start_vals[p][0];

            while(start_vals[p][0] < sieve_start + sieve_interval && start_vals[p][0] >= sieve_start){
                mpz_set(temp,interval[start_vals[p][0] - sieve_start]);
                mpz_tdiv_r_ui(temp,temp,base[p]);
                while(mpz_get_ui(temp)==0){
                    mpz_tdiv_q_ui(interval[start_vals[p][0] - sieve_start],interval[start_vals[p][0] - sieve_start],base[p]);
                    mpz_set(temp,interval[start_vals[p][0] - sieve_start]);
                    mpz_tdiv_r_ui(temp,temp,base[p]);
                }
                start_vals[p][0] += base[p];
            }

            if(start_vals[p][1] != t){
                while(start_vals[p][1] < sieve_start + sieve_interval && start_vals[p][1] >= sieve_start){
                    mpz_set(temp,interval[start_vals[p][1] - sieve_start]);
                    mpz_tdiv_r_ui(temp,temp,base[p]);
                    while(mpz_get_ui(temp)==0){
                        mpz_tdiv_q_ui(interval[start_vals[p][1] - sieve_start],interval[start_vals[p][1] - sieve_start],base[p]);
                        mpz_set(temp,interval[start_vals[p][1] - sieve_start]);
                        mpz_tdiv_r_ui(temp,temp,base[p]);
                    }
                    start_vals[p][1] += base[p];
                }
            }
        }

        for (int i = 0; i < sieve_interval; ++i) {
            if (mpz_cmp_ui(interval[i],1)==0){
                unsigned char exp[needed];
                for (int k = 0; k < needed; ++k) {
                    exp[k]=0;
                }
                unsigned long x = sieve_start + i;
                mpz_t y;
                mpz_init(y);
                poly(&y,x,b,N);
                trial(exp, y, base, needed);
                if(notIn(exp,seen,needed)){
                    mpz_t wynik;
                    Data vals;
                    mpz_init(wynik);
                    mpz_add_ui(wynik,b,x);
                    mpz_init_set(vals.smoothVals.vals[0],wynik);
                    mpz_init_set(vals.smoothVals.vals[1],y);
                    ListAdd(vals, &smooth_vals);
                    vals.exp = malloc(sizeof(unsigned char)*needed);
                    memcpy(vals.exp,exp,needed);
                    ListAdd(vals, &seen);
                    mpz_clear(wynik);
                }
                mpz_clear(y);
            }
        }
        printf("After sieving the next interval we have %d\n", ListSize(&smooth_vals));
    }

    mpz_clear(b);
    for (int i = 0; i < sieve_interval; ++i) {
        mpz_clear(interval[i]);
    }
    free(interval);

    printf("\n==\nRunning Gaussian Elimination\n==\n\n");

    unsigned char M[ListSize(&seen)][needed];

    List wsk = seen;
    for (int n = 0; hasNext(wsk); ++n) {
        memcpy(M[n],wsk->data.exp,needed);
        wsk = next(&wsk);
    }

    bool marks[ListSize(&seen)];

    wsk = seen;
    while (hasNext(wsk)){
        free(wsk->data.exp);
        wsk=next(&wsk);
    }
    ListClear(&seen);

    for (int l = 0; l < sizeof(marks)/sizeof(bool); ++l) {
        marks[l] = false;
    }

    gauss(needed,sizeof(marks)/sizeof(bool),M,marks);

    puts("Done!");

    printf("\n==\nTesting Linear Dependencies\n==\n\n");

    List deps;
    for (unsigned long i = 0; i < sizeof(marks) / sizeof(bool); ++i) {
        ListInit(&deps);
        if(!marks[i]){
            findLinearDeps(&deps,i,needed,M,sizeof(marks) / sizeof(bool));
            List dep = deps;
            puts("");
            while (hasNext(dep)) {
                ListIterate(&dep->data.innerList, &show);
                puts("");
                mpz_t gcd;
                mpz_init(gcd);
                testdep(&gcd, dep->data.innerList, smooth_vals, N);
                if (mpz_cmp_ui(gcd,1)!=0 && mpz_cmp(gcd,N)){
                    gmp_printf("\nNon-trivial factor: %Zd\n",gcd);

                    mpz_clear(N);
                    mpz_clear(temp);
                    mpz_clear(gcd);

                    wsk = smooth_vals;
                    while (hasNext(wsk)){
                        mpz_clear(wsk->data.smoothVals.vals[0]);
                        mpz_clear(wsk->data.smoothVals.vals[1]);
                        wsk=next(&wsk);
                    }
                    ListClear(&smooth_vals);

                    dep = deps;
                    while (hasNext(dep)){
                        ListClear(&dep->data.innerList);
                        dep = next(&dep);
                    }
                    ListClear(&deps);

                    exit(0);
                }
                dep = next(&dep);
            }
        }
        ListClear(&deps);
    }

    return 0;
}
