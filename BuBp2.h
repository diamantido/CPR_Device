//Band pass butterworth filter order=2 alpha1=0.0048 alpha2=0.012
class  FilterBuBp2
{
  public:
    FilterBuBp2()
    {
      for (int i = 0; i <= 4; i++)
        v[i] = 0.0;
    }
  private:
    float v[5];
  public:
    float step(float x) //class II
    {
      v[0] = v[1];
      v[1] = v[2];
      v[2] = v[3];
      v[3] = v[4];
      v[4] = (4.962220399347190992e-4 * x)
             + (-0.93802609774498546535 * v[0])
             + (3.80776129871011903560 * v[1])
             + (-5.80130838877156307376 * v[2])
             + (3.93156817990936380269 * v[3]);
      return
        (v[0] + v[4])
        - 2 * v[2];
    }

    float val() {
      return (v[0] + v[4]) - 2 * v[2];
    }
};
