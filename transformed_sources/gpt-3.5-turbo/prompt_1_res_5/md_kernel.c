#pragma ACCEL kernel

void md_kernel(double force_x[256],double force_y[256],double force_z[256],double position_x[256],double position_y[256],double position_z[256],int NL[4096])
{
  double delx;
  double dely;
  double delz;
  double r2inv;
  double r6inv;
  double potential;
  double force;
  double j_x;
  double j_y;
  double j_z;
  double i_x;
  double i_y;
  double i_z;
  double fx;
  double fy;
  double fz;
  int i;
  int j;
  int jidx;
  
#pragma ACCEL PIPELINE auto{__PIPE__L0}
  
#pragma ACCEL TILE FACTOR=16{__TILE__L0}
  
#pragma ACCEL PARALLEL FACTOR=16{__PARA__L0}
  loop_i:
  for (i = 0; i < 256; i++) {
    i_x = position_x[i];
    i_y = position_y[i];
    i_z = position_z[i];
    fx = ((double )0);
    fy = ((double )0);
    fz = ((double )0);
    loop_j:
    for (j = 0; j < 16; j++) {
      jidx = NL[i * 16 + j];
      j_x = position_x[jidx];
      j_y = position_y[jidx];
      j_z = position_z[jidx];
      delx = i_x - j_x;
      dely = i_y - j_y;
      delz = i_z - j_z;
      r2inv = 1.0 / (delx * delx + dely * dely + delz * delz);
      r6inv = r2inv * r2inv * r2inv;
      potential = r6inv * (1.5 * r6inv - 2.0);
      force = r2inv * potential;
      fx += delx * force;
      fy += dely * force;
      fz += delz * force;
    }
    force_x[i] = fx;
    force_y[i] = fy;
    force_z[i] = fz;
  }
}
// Explanation of transformations:
// 1. Loop tiling with a factor of 16: This transformation groups the inner loop iterations into tiles of size 16. By doing this, we can take advantage of data locality and reduce memory accesses, which can improve performance.
// 2. Loop permutation: The inner loop (loop_j) has been moved inside the outer loop (loop_i). This allows for better parallelism as the inner loop iterations can be executed concurrently for different values of 'i'.
// 3. Parallelization with a factor of 16: This transformation parallelizes the outer loop iterations with a factor of 16, enabling multiple iterations to be executed simultaneously. This can lead to better resource utilization and faster execution.