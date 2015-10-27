/*
* Copyright (C) 2015 Jose Luis Toledano Lopez <j.l.toledano.l@gmail.com>
*
* This file is part of TurBoCapture3000.
*
* TurBoCapture3000 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* TurBoCapture3000 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with ImageQ. If not, see <http://www.gnu.org/licenses/>.
*/

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

