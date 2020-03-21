#include "face_recog.h"
#include <security/pam_modules.h>


using namespace dlib;
using namespace std;


matrix<float, 0, 1> get_embedding(matrix<rgb_pixel> img, anet_type &net, shape_predictor &sp, frontal_face_detector &detector) {
    std::vector<matrix<rgb_pixel>> faces;
    for (auto face : detector(img)){
        auto shape = sp(img, face);
        matrix<rgb_pixel> face_chip;
        extract_image_chip(img, get_face_chip_details(shape,150,0.25), face_chip);
        faces.push_back(move(face_chip));
    }
    if (faces.size() == 0){
        cerr << "No faces found in image!" << endl;
        exit(PAM_AUTH_ERR);
    }
    if(faces.size() > 1) {
        cerr<<"More than one face found in the image"<<endl;
        exit(PAM_AUTH_ERR);
    }
    std::vector<matrix<float,0,1>> tmp = net(faces);
    return tmp[0];
}

inline void save_embedding(matrix<float, 0, 1> emb, char *path) { serialize(path)<<emb; }

bool predict(matrix<float, 0,1> test, double threshold) {
    matrix<float, 0, 1> source;
    deserialize("/home/athul/.config/i3-facelock/model/result.dat")>>source;
    return length(source-test)<threshold?true:false;
}

matrix<rgb_pixel> capture_from_cam() {
    uint8_t *rgb = get_rgbframe();
    matrix<rgb_pixel, 240, 320> mat;
    for(int i=0; i<(320*240*3); i+=3) {
        rgb_pixel pix;
        pix.red = rgb[i];
        pix.green = rgb[i+1];
        pix.blue = rgb[i+2];
        mat((i/3)/320 , (i/3)%320) = pix;
    }
    image_window img(mat);
    dlib::sleep(5000);
    return mat;
}