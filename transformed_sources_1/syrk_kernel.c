#pragma ACCEL kernel

void kernel_syrk(double alpha,double beta,double C[80][80],double A[80][60])
{
  int i;
  int j;
  int k;
//BLAS PARAMS
//TRANS = 'N'
//UPLO  = 'L'
// =>  Form  C := alpha*A*A**T + beta*C.
//A is NxM
//C is NxN
  
#pragma ACCEL PIPELINE auto{__PIPE__L0}
  
#pragma ACCEL TILE FACTOR=auto{__TILE__L0}
  
#pragma ACCEL PARALLEL FACTOR=auto{__PARA__L0}
  for (i = 0; i < 80; i++) {
    
#pragma ACCEL PARALLEL FACTOR=auto{__PARA__L1}
    for (k = 0; k < 60; k++) {
      
#pragma ACCEL PIPELINE auto{__PIPE__L2}
      
#pragma ACCEL TILE FACTOR=auto{__TILE__L2}
      
#pragma ACCEL PARALLEL FACTOR=auto{__PARA__L2}
      for (j = 0; j < 80; j++) {
        if (j <= i) {
          C[i][j] *= beta;
          C[i][j] += alpha * A[i][k] * A[j][k];
        }
      }
    }
  }
}
// Explanation of transformations:
// 1. Loop permutation: The innermost loop over `k` has been moved to be the second loop after the loop over `i`. This allows for better data locality as the operations involving `A` are now closer together.
// 2. Loop fusion: The loop over `j` that was originally split into two separate loops has been fused into one loop. This reduces loop overhead and improves parallelism by combining the two sets of operations into a single loop.
// 3. Loop tiling: No explicit loop tiling has been applied in this transformation, but the existing tiling directives have been kept in place for potential further optimization.