#ifndef PHOTO
#define PHOTO

#include <gphoto2/gphoto2-camera.h>

#include <stdio.h>
#include <fcntl.h>

#include <QMessageBox>

class Photo
{

    public:
        Photo();
        ~Photo();

    private:
        static void ctx_error_func (GPContext *context, const char *str, void *data);

        static void ctx_status_func (GPContext *context, const char *str, void *data);

        void capture(const char *filename);

        //void shootPhoto ();

    private:
        Camera *camera;
        GPContext *context;

        int ret;
};

#endif // PHOTO

