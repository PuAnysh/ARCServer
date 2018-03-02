#ifndef PUBLICDATASTRUCT_H
#define PUBLICDATASTRUCT_H
#include <ARCFace/arcsoft_fsdk_face_detection.h>
#include <ARCFace/arcsoft_fsdk_face_recognition.h>
#include <QTime>
struct UserModel{
    AFR_FSDK_FACEMODEL model;
    int id;
};
struct GuestModel{
    AFR_FSDK_FACEMODEL model;
    int UserID;
    QTime addTime;
};
#endif // PUBLICDATASTRUCT_H
