# pam-face
Linux PAM module for face authenication.

## Installation Instructions
1. Install the following dependenices.
* Dlib
* libcblas

2. Download the following files and store it in ./models directory
* [face landmark model](http://dlib.net/files/shape_predictor_5_face_landmarks.dat.bz2)
* [face recognition model](http://dlib.net/files/dlib_face_recognition_resnet_model_v1.dat.bz2)

3.
```
$ make
$ sudo make install
```
4. Add `auth sufficient pam_face.so` to your pam configuration file to use this method of authenication.
