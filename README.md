# pam-face
Linux PAM module for face authenication

## Installation Instructions
```
$ make
$ sudo make install
```
Add `auth sufficient pam_face.so` to your pam configuration file to use this method of authenication.
