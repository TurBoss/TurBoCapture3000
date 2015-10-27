#ifndef STICH
#define STICH

#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/stitching.hpp"
#include "opencv2/xfeatures2d.hpp"

class Stich
{

    public:
        Stich();
        ~Stich();

    private:

        void stich(const char *fileA, const char *fileB);

        void stichv2(const char *fileA, const char *fileB);

        void stichv2_video(const char *file);

};

#endif // STICH

