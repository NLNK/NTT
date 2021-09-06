/* 
Compuatation of NTT for Dilithium - Digital Signature Scheme without using Montgomery reduction 
- NTT is used in Dilithium to compute the product of two polynomials.
- Array always has N(256) elements, thus the polynomial has N coefficients

Usage:
  $ gcc -o ntt ntt.c
  $ ./ntt

*/

#include <stdint.h>
#include<stdio.h>

#define mod_Q(x,y) ((x % y) + y) % y
#define N 256
#define Q 8380417

static const int32_t zetas[N] = {0, -3572223, 3765607, 3761513, -3201494, -2883726, -3145678, -3201430, -601683, 3542485, 2682288, 2129892, 3764867, -1005239, 557458, -1221177, -3370349, -4063053, 2663378, -1674615, -3524442, -434125, 676590, -1335936, -3227876, 1714295, 2453983, 1460718, -642628, -3585098, 2815639, 2283733, 3602218, 3182878, 2740543, -3586446, -3110818, 2101410, 3704823, 1159875, 394148, 928749, 1095468, -3506380, 2071829, -4018989, 3241972, 2156050, 3415069, 1759347, -817536, -3574466, 3756790, -1935799, -1716988, -3950053, -2897314, 3192354, 556856, 3870317, 2917338, 1853806, 3345963, 1858416, 3073009, 1277625, -2635473, 3852015, 4183372, -3222807, -3121440, -274060, 2508980, 2028118, 1937570, -3815725, 2811291, -2983781, -1109516, 4158088, 1528066, 482649, 1148858, -2962264, -565603, 169688, 2462444, -3334383, -4166425, -3488383, 1987814, -3197248, 1736313, 235407, -3250154, 3258457, -2579253, 1787943, -2391089, -2254727, 3482206, -4182915, -1300016, -2362063, -1317678, 2461387, 3035980, 621164, 3901472, -1226661, 2925816, 3374250, 1356448, -2775755, 2683270, -2778788, -3467665, 2312838, -653275, -459163, 348812, -327848, 1011223, -2354215, -3818627, -1922253, -2236726, 1744507, 1753, -1935420, -2659525, -1455890, 2660408, -1780227, -59148, 2772600, 1182243, 87208, 636927, -3965306, -3956745, -2296397, -3284915, -3716946, -27812, 822541, 1009365, -2454145, -1979497, 1596822, -3956944, -3759465, -1685153, -3410568, 2678278, -3768948, -3551006, 635956, -250446, -2455377, -4146264, -1772588, 2192938, -1727088, 2387513, -3611750, -268456, -3180456, 3747250, 2296099, 1239911, -3838479, 3195676, 2642980, 1254190, -12417, 2998219, 141835, -89301, 2513018, -1354892, 613238, -1310261, -2218467, -458740, -1921994, 4040196, -3472069, 2039144, -1879878, -818761, -2178965, -1623354, 2105286, -2374402, -2033807, 586241, -1179613, 527981, -2743411, -1476985, 1994046, 2491325, -1393159, 507927, -1187885, -724804, -1834526, -3033742, -338420, 2647994, 3009748, -2612853, 4148469, 749577, -4022750, 3980599, 2569011, -1615530, 1723229, 1665318, 2028038, 1163598, -3369273, 3994671, -11879, -1370517, 3020393, 3363542, 214880, 545376, -770441, 3105558, -1103344, 508145, -553718, 860144, 3430436, 140244, -1514152, -2185084, 3123762, 2358373, -2193087, -3014420, -1716814, 2926054, -392707, -303005, 3531229, -3974485, -3773731, 1900052, -781875, 1054478, -731434};

/* Inplace forward NTT*/
void ntt(int32_t a[N]) {
  unsigned int len, start, j, m;
  int32_t zeta, t;

  m = 0;
  for(len = 128; len > 0; len >>= 1) {
    for(start = 0; start < N; start = j + len) {
      zeta = zetas[++m];
      for(j = start; j < start + len; j++) {        
        t = mod_Q((int64_t)zeta * a[j + len],Q);
        a[j + len] = mod_Q(a[j] - t,Q);        
        a[j] = mod_Q(a[j] + t,Q);
      }
    }
  }
}

/* Inverse NTT */
void intt(int32_t a[N]) {
  unsigned int start, len, j, m;
  int32_t t, zeta;
  const int32_t f = 8347681; // 256^-1 modQ

  m = 256;
  for(len = 1; len < N; len <<= 1) {
    for(start = 0; start < N; start = j + len) {
      zeta = -zetas[--m];
      for(j = start; j < start + len; j++) {
        t = a[j];
        a[j] = t + a[j + len];
        a[j + len] = t - a[j + len];
        a[j + len] = mod_Q((int64_t)zeta * a[j + len],Q);        
      }
    }
  }

  for(j = 0; j < N; j++)
     a[j] = mod_Q(((int64_t)f * a[j]) + (Q >> 1), Q) - (Q >> 1);
  
}

void main()
{
  int32_t coeffs[N]= {-4, 0, -2, 1, -2, -4, -1, 2, 1, -1, -4, 0, 2, 1, 4, 0, -2, -4, 2, 2, -1, 0, 2, 3, 4, 2, -2, -1, 0, -3, -3, 2, 0, 4, -4, 0, 3, 3, 4, -1, -2, 1, 0, -1, -2, 2, -2, -4, 1, 0, -2, -4, 3, 4, -1, 2, -2, -4, 3, -3, -2, 2, -2, -1, -2, -3, 3, 1, -2, 1, 0, -3, 3, 1, 0, -3, -2, 1, 0, 4, 1, -2, -2, 1, 2, 2, -3, -3, 4, -1, 2, 2, 3, -2, 3, -1, 2, -1, 4, 2, -4, 1, -1, 3, 4, -1, 1, -3, -4, 3, -4, -3, 3, 0, 1, 0, -2, 3, 4, -2, -3, -3, 0, 2, 1, 0, 3, 0, -4, 4, -3, -4, 1, -2, 0, -3, -4, 3, -3, -4, 1, 1, 0, -1, -2, -1, -1, 2, 0, 2, 4, -3, -2, 0, 2, 3, -2, 1, -3, -1, -4, 4, 0, 2, 1, -1, 4, 0, -1, 1, -3, 0, -3, 3, 4, 0, 3, 1, 2, 1, 0, 0, 4, 2, -4, 4, 2, 3, -1, 3, 2, -4, 3, 2, -2, 4, 3, 0, 2, -2, 3, 1, 0, -2, -2, -1, 2, -1, 1, 0, 0, 2, -2, 3, -2, 0, -1, -3, -3, -4, -1, 1, -4, -1, -4, 1, -3, 2, -1, -1, 0, 2, 1, 4, 2, -4, 4, -1, 1, 0, -3, 4, -4, -1, -3, -3, 0, 4, -1, -2, 4, 4, 0, -3, -1, -3};    // Sample test vector

  printf("\nGiven coefficients are:\n");
  for(int i=0;i<N;i++)  
    printf("%d, ",coeffs[i]);
  printf("\n");
  
  ntt(coeffs);  // Compute NTT for the given coeffs

  printf("\n\nNTT for the given coefficients:\n\n");
  for(int i=0;i<N;i++)
    printf("%d, ",coeffs[i]);
  printf("\n");

  intt(coeffs);  // Compute INTT for the given coeffs

  printf("\n\nINTT:\n\n");
  for(int i=0;i<N;i++)
    printf("%d, ",coeffs[i]);
  printf("\n");

}
