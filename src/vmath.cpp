
#include "vmath.h"

#include <math.h>
#include <float.h>
#include <xmmintrin.h>

#ifdef __GNUC__
  #define _MM_ALIGN16 __attribute__ ((aligned (16)))
#endif

namespace math
{
  // turn those verbose intrinsics into something readable.
  #define loadps(mem)         _mm_load_ps((const float * const)(mem))
  #define storess(ss,mem)     _mm_store_ss((float * const)(mem),(ss))
  #define minss               _mm_min_ss
  #define maxss               _mm_max_ss
  #define minps               _mm_min_ps
  #define maxps               _mm_max_ps
  #define mulps               _mm_mul_ps
  #define subps               _mm_sub_ps
  #define rotatelps(ps)       _mm_shuffle_ps((ps),(ps), 0x39) // a,b,c,d -> b,c,d,a
  #define muxhps(low,high)    _mm_movehl_ps((low),(high)) // low{a,b,c,d}|high{e,f,g,h} = {c,d,g,h}

  namespace {
    static const float inf = -logf(0); // let's keep C and C++ compilers happy.
    static const float _MM_ALIGN16 plusInf[4]  = {  inf,  inf,  inf,  inf },
                                   minusInf[4] = { -inf, -inf, -inf, -inf };
  }

  // -- Vector3 --

  Vector3::Vector3()
    : x(0), y(0), z(0)
  { }

  Vector3::Vector3(float x, float y, float z)
    : x(x), y(y), z(z)
  { }

  Vector3::Vector3(Vector3 const& v)
    : x(v.x), y(v.y), z(v.z)
  { }

  // -- Vector4 --

  Vector4::Vector4()
    : x(0), y(0), z(0), w(0)
  { }

  Vector4::Vector4(float x, float y, float z, float w)
    : x(x), y(y), z(z), w(w)
  { }

  Vector4::Vector4(Vector3 const& v)
    : x(v.x), y(v.y), z(v.z), w(0)
  { }

  Vector4::Vector4(Vector4 const& v)
    : x(v.x), y(v.y), z(v.z), w(v.w)
  { }

  // -- Box --

  Box::Box()
    : min(),
      max()
  { }

  Box::Box(Box const& box)
    : min(box.min),
      max(box.max)
  { }

  Box::Box(Vector3 const& min, Vector3 const& max)
    : min(min),
      max(max)
  { }

  namespace box
  {
  }

  // -- Ray --

  Ray::Ray(Vector3 const& start, Vector3 const& dir)
    : start(start),
      dir(dir)
  { }

  namespace ray
  {
    bool insersect(Ray const& ray, Box const& box)
    {
      const Vector4 bMin(box.min);
      const Vector4 bMax(box.max);
      const Vector4 rStart(ray.start);
      const Vector4 rDir(1.0f / ray.dir);

      // you may already have those values hanging around somewhere
      const __m128 plus_inf  = loadps(plusInf),
                   minus_inf = loadps(minusInf);

      // use whatever's apropriate to load.
      const __m128 box_min = loadps(&bMin),
                   box_max = loadps(&bMax),
                   pos = loadps(&rStart),
                   inv_dir = loadps(&rDir);

      // use a div if inverted directions aren't available
      const __m128 l1 = mulps(subps(box_min, pos), inv_dir);
      const __m128 l2 = mulps(subps(box_max, pos), inv_dir);

      // the order we use for those min/max is vital to filter out
      // NaNs that happens when an inv_dir is +/- inf and
      // (box_min - pos) is 0. inf * 0 = NaN
      const __m128 filtered_l1a = minps(l1, plus_inf);
      const __m128 filtered_l2a = minps(l2, plus_inf);

      const __m128 filtered_l1b = maxps(l1, minus_inf);
      const __m128 filtered_l2b = maxps(l2, minus_inf);

      // now that we're back on our feet, test those slabs.
      __m128 lmax = maxps(filtered_l1a, filtered_l2a);
      __m128 lmin = minps(filtered_l1b, filtered_l2b);

      // unfold back. try to hide the latency of the shufps & co.
      const __m128 lmax0 = rotatelps(lmax);
      const __m128 lmin0 = rotatelps(lmin);
      lmax = minss(lmax, lmax0);
      lmin = maxss(lmin, lmin0);

      const __m128 lmax1 = muxhps(lmax,lmax);
      const __m128 lmin1 = muxhps(lmin,lmin);
      lmax = minss(lmax, lmax1);
      lmin = maxss(lmin, lmin1);

      const bool ret = _mm_comige_ss(lmax, _mm_setzero_ps()) & _mm_comige_ss(lmax,lmin);

      //storess(lmin, &rs.t_near);
      //storess(lmax, &rs.t_far);

      return  ret;
    }
  }

}