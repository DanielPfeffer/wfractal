/*
A word about the 3D library. Even though this library supports
three dimensions, the matrices are 4x4 for the following reason.
With normal 3 dimensional vectors, translation is an ADDITION,
and rotation is a MULTIPLICATION. A vector {x,y,z} is represented
as a 4-tuple {x,y,z,1}. It is then possible to define a 4x4
matrix such that multiplying the vector by the matrix translates
the vector. This allows combinations of translation and rotation
to be obtained in a single matrix by multiplying a translation
matrix and a rotation matrix together. Note that in the code,
vectors have three components; since the fourth component is
always 1, that value is not included in the vector variable to
save space, but the routines make use of the fourth component
(see vec_mult()). Similarly, the fourth column of EVERY matrix is
always
         0
         0
         0
         1
but currently the C version of a matrix includes this even though
it could be left out of the data structure and assumed in the
routines. Vectors are ROW vectors, and are always multiplied with
matrices FROM THE LEFT (e.g. vector*matrix). Also note the order
of indices of a matrix is matrix[row][column], and in usual C
fashion, numbering starts with 0.

TRANSLATION MATRIX =  1     0     0     0
                      0     1     0     0
                      0     0     1     0
                      Tx    Ty    Tz    1

SCALE MATRIX =        Sx    0     0     0
                      0     Sy    0     0
                      0     0     Sz    0
                      0     0     0     1

Rotation about x axis i degrees:
ROTX(i) =             1     0     0     0
                      0   cosi  sini    0
                      0  -sini  cosi    0
                      0     0     0     1

Rotation about y axis i degrees:
ROTY(i) =           cosi    0  -sini    0
                      0     1     0     0
                    sini    0   cosi    0
                      0     0     0     1

Rotation about z axis i degrees:
ROTZ(i) =           cosi  sini    0     0
                   -sini  cosi    0     0
                      0     0     1     0
                      0     0     0     1

                      --  Tim Wegner  April 22, 1989
*/


#include <string.h>
  /* see Fractint.c for a description of the "include"  hierarchy */
#include "port.h"
#include "prototyp.h"

/* initialize a matrix and set to identity matrix
   (all 0's, 1's on diagonal) */
void identity(DMATRIX m)
{
   int i,j;
   for(i=0;i<CMAX;i++)
   for(j=0;j<RMAX;j++)
      if(i==j)
         m[j][i] = 1.0;
      else
          m[j][i] = 0.0;
}

/* Multiply two matrices */
void mat_mul(DMATRIX mat1, DMATRIX mat2, DMATRIX mat3)
{
     /* result stored in MATRIX new to avoid problems
        in case parameter mat3 == mat2 or mat 1 */
     DMATRIX new;
     int i,j;
     for(i=0;i<4;i++)
     for(j=0;j<4;j++)
        new[j][i] =  mat1[j][0]*mat2[0][i]+
                     mat1[j][1]*mat2[1][i]+
                     mat1[j][2]*mat2[2][i]+
                     mat1[j][3]*mat2[3][i];
     memcpy(mat3,new,sizeof(new));
}

/* multiply a matrix by a scalar */
void scale (double sx, double sy, double sz, DMATRIX m)
{
   DMATRIX scale;
   identity(scale);
   scale[0][0] = sx;
   scale[1][1] = sy;
   scale[2][2] = sz;
   mat_mul(m,scale,m);
}

/* rotate about X axis  */
void xrot (double theta, DMATRIX m)
{
   DMATRIX rot;
   double sintheta,costheta;
   sintheta = sin(theta);
   costheta = cos(theta);
   identity(rot);
   rot[1][1] = costheta;
   rot[1][2] = -sintheta;
   rot[2][1] = sintheta;
   rot[2][2] = costheta;
   mat_mul(m,rot,m);
}

/* rotate about Y axis  */
void yrot (double theta, DMATRIX m)
{
   DMATRIX rot;
   double sintheta,costheta;
   sintheta = sin(theta);
   costheta = cos(theta);
   identity(rot);
   rot[0][0] = costheta;
   rot[0][2] = sintheta;
   rot[2][0] = -sintheta;
   rot[2][2] = costheta;
   mat_mul(m,rot,m);
}

/* rotate about Z axis  */
void zrot (double theta, DMATRIX m)
{
   DMATRIX rot;
   double sintheta,costheta;
   sintheta = sin(theta);
   costheta = cos(theta);
   identity(rot);
   rot[0][0] = costheta;
   rot[0][1] = -sintheta;
   rot[1][0] = sintheta;
   rot[1][1] = costheta;
   mat_mul(m,rot,m);
}

/* translate  */
void trans (double tx, double ty, double tz, DMATRIX m)
{
   DMATRIX trans;
   identity(trans);
   trans[3][0] = tx;
   trans[3][1] = ty;
   trans[3][2] = tz;
   mat_mul(m,trans,m);
}

/* cross product  - useful because cross is perpendicular to v and w */
int fcross_product (VECTOR v, VECTOR w, VECTOR cross)
{
   VECTOR tmp;
   tmp[0] =  v[1]*w[2] - w[1]*v[2];
   tmp[1] =  w[0]*v[2] - v[0]*w[2];
   tmp[2] =  v[0]*w[1] - w[0]*v[1];
   cross[0] = tmp[0];
   cross[1] = tmp[1];
   cross[2] = tmp[2];
   return(0);
}

/* cross product integer arguments (not fudged) */
/*** pb, unused
int icross_product (IVECTOR v, IVECTOR w, IVECTOR cross)
{
   IVECTOR tmp;
   tmp[0] =  v[1]*w[2] - w[1]*v[2];
   tmp[1] =  w[0]*v[2] - v[0]*w[2];
   tmp[2] =  v[0]*w[1] - w[0]*v[1];
   cross[0] = tmp[0];
   cross[1] = tmp[1];
   cross[2] = tmp[2];
   return(0);
}
***/

/* normalize a vector to length 1 */
int fnormalize_vector(VECTOR v)
{
    double vlength;
    vlength = dot_product(v,v);

    /* bailout if zero vlength */
    if(vlength < FLT_MIN || vlength > FLT_MAX)
       return(-1);
    vlength = sqrt(vlength);
    if(vlength < FLT_MIN)
       return(-1);

    v[0] /= vlength;
    v[1] /= vlength;
    v[2] /= vlength;
    return(0);
}

/* multiply source vector s by matrix m, result in target t */
/* used to apply transformations to a vector */
int vmult(VECTOR s, DMATRIX m, VECTOR t)
{
   VECTOR tmp;
   int i,j;
   for(j=0;j<CMAX-1;j++)
   {
      tmp[j] = 0.0;
      for(i=0;i<RMAX-1;i++)
         tmp[j] += s[i]*m[i][j];
      /* vector is really four dimensional with last component always 1 */
      tmp[j] += m[3][j];
   }
   /* set target = tmp. Necessary to use tmp in case source = target */
   memcpy(t,tmp,sizeof(tmp));
   return(0);
}

/* multiply vector s by matrix m, result in s */
/* use with a function pointer in line3d.c */
/* must coordinate calling conventions with */
/* mult_vec in general.asm */
void mult_vec(VECTOR s)
{
   VECTOR tmp;
   int i,j;
   for(j=0;j<CMAX-1;j++)
   {
      tmp[j] = 0.0;
      for(i=0;i<RMAX-1;i++)
         tmp[j] += s[i]*m[i][j];
      /* vector is really four dimensional with last component always 1 */
      tmp[j] += m[3][j];
   }
   /* set target = tmp. Necessary to use tmp in case source = target */
   memcpy(s,tmp,sizeof(tmp));
}

/* perspective projection of vector v with respect to viewpont vector view */
int
perspective(VECTOR v)
{
   double denom;
   denom = view[2] - v[2];

   if(denom >= 0.0)
   {
      v[0] = bad_value;   /* clipping will catch these values */
      v[1] = bad_value;   /* so they won't plot values BEHIND viewer */
      v[2] = bad_value;
      return(-1);
   }
   v[0] = (v[0]*view[2] - view[0]*v[2])/denom;
   v[1] = (v[1]*view[2] - view[1]*v[2])/denom;

   /* calculation of z if needed later */
   /* v[2] =  v[2]/denom;*/
   return(0);
}

