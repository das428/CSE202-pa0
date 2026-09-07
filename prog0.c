#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
// Union to store 4 bytes as an array of bytes, an unsigned, signed, or float number
union value{
    unsigned uval;
    int sval;
    float fval;
    unsigned char bytes[4];
};

char hexDigit(char c);

// reads 8 hex characters from string input and stores it in the union v
// returns -1 if the hexadecimal number is invalid, 0 otherwise
int read_hex(union value *v, char *input){
    if(strlen(input) != 8){
        return -1;
    }

    unsigned int result = 0;

    for(int i =0; i < 8; i++){
        char x = hexDigit(input[i]);
        if(x == -1){
            return -1;
        }
        else{
            result = result << 4 | (unsigned) x;
        }
    }
    v->uval = result;
    return 0;
}

// converts the ASCII hex character c to binary
// returns the hex value of c if c is a valid hex digit, -1 otherwise
char hexDigit(char c){
    if(c >= '0' && c <= '9'){
        return c - '0';
    }
    else if(c >= 'A' && c <= 'F'){
        return c - 'A' + 10;
    }
    else if(c >= 'a' && c <= 'f'){
        return c - 'a' + 10;
    }
    else{
        return -1;
    }
}

// returns true if x has any even bit equal to 1, 0 otherwise
int any_even_one(unsigned x){
    for(int i = 0; i < 31; i+=2){
        if((x >> i) & 1){
            return 1;
        }
    }
    return 0;
}
// returns a mask indicating the position of the left most one in x
int leftmost_one(unsigned x){
    for(int i = 31; i >= 0; i--){
        if(x >> i == 1){
            return 1u << i;
        }
    }
    return 0;
}
// returns x shifted n positions to the left with the n most significant bits of x 
// inserted at the right of x
unsigned rotate_left(unsigned x, int n){
    if(n > 32){
        return -1;
    }
    unsigned int i = (x >> (32 - n));
    x = x << n;
    return x | i;
}
// returns x shifted n positions to the right with the n least significant bits of x 
// inserted at the left of x
unsigned rotate_right(unsigned x, int n){
    if(n > 32){
        return -1;
    }
    unsigned int i = (x << (32 - n));
    x = x >> n;
    return x | i;
}

// returns x+y if no overflow occurs
// returns TMAX if a positive overflow occurs
// returns TMIN if a negative overflow occurs
int saturating_add(int x, int y){
    int sum = x + y;
    if(x > 0 && y > 0 && sum < 0){
        return INT_MAX;
    }
    else if(x < 0 && y < 0 && sum > 0){
        return INT_MIN;
    }
    return sum;
}
// multiplies the binary representation of a float number f by 2
unsigned float_twice(unsigned f){
    unsigned int sign = f >> 31;
    unsigned int exp = (f << 1) >> 24;
    unsigned int mant = (f << 9) >> 9;

    if(exp == 255){
        return f;
    }
    else if(exp == 0){
        mant = mant << 1;
        return (sign << 31) | mant;
    }
    else{
        exp = exp + 1;
        if(exp == 255){
            return (sign << 31) | (255 << 23);
        }
        return (sign << 31) | (exp << 23) | mant;
    }
    
}
// divides the binary representation of a float number f by 2
unsigned float_half(unsigned f){
    unsigned int sign = f >> 31;
    unsigned int exp = (f << 1) >> 24;
    unsigned int mant = (f << 9) >> 9;

    if(exp == 255){
        return f;
    }
    else if(exp == 0){
        mant = mant >> 1;
        return (sign << 31) | mant;
    }
    else if(exp == 1){
        mant = (mant | 0x800000) >> 1;
        return (sign << 31) | mant;
    }
    else{
        exp = exp - 1;
        return (sign << 31) | (exp << 23) | mant;
    }
}

int main(int argc, char** argv){
    if(argc != 3 && argc != 4){
        printf("Invalid number of arguments");
        exit(0);
    }

    union value val; //needed to actually initialize the storage for the union value

    int test = read_hex(&val, argv[2]);
    if(test == -1){
        printf("Invalid hex value\n");
        exit(0);
    }

    else if(strcmp(argv[1], "even") == 0){
        if(any_even_one(val.uval) == 1){
            printf("True\n");
        }
        else{
            printf("False\n");
        }
    }
    else if(strcmp(argv[1], "left") == 0){
        //%08x to add padding with 0s for 8 digits, and x to represent it in hex
        printf("%08x\n", leftmost_one(val.uval));
    }
    else if(strcmp(argv[1], "lrotate") == 0){
        unsigned int x = rotate_left(val.uval, atoi(argv[3]));
        if(x == -1){
            printf("Invalid number of shift positions\n");
        }
        else{
            printf("%08x\n", x);
        }
    }
    else if(strcmp(argv[1], "rrotate") == 0){
        unsigned int x = rotate_right(val.uval, atoi(argv[3]));
        if(x == -1){
            printf("Invalid number of shift positions\n");
        }
        else{
            printf("%08x\n", x);
        }
    }
    else if(strcmp(argv[1], "saturate") == 0){
        //need to initialize another union value for the second hex.
        union value val2;

        int test = read_hex(&val2, argv[3]);

        if(test == -1){
            printf("Invalid hex value\n");
            exit(0);
        }
        int satAdd = saturating_add(val.sval, val2.sval);

        printf("%08x %d\n", satAdd, satAdd);
    }
    else if(strcmp(argv[1], "twice") == 0){
        union value result;
        result.uval = float_twice(val.uval);
        printf("%08x %e\n", result.uval, result.fval);
    }
    else if(strcmp(argv[1], "half") == 0){
        union value result;
        result.uval = float_half(val.uval);
        printf("%08x %e\n", result.uval, result.fval);
    }

    else{
       printf("Invalid operation\n");
    }




    return 0;
}
