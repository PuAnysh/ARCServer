#ifndef ARCFACEENGINE_H
#define ARCFACEENGINE_H
#define WORKSIZE (40*1024*1024)
#include <bits/stdc++.h>
#include <ARCFace/arcsoft_fsdk_face_detection.h>
#include <ARCFace/arcsoft_fsdk_face_recognition.h>
#include <ARCFace/merror.h>
#include <key.h>
#include "arcfacefeaturedb.h"
class ARCFaceEngine
{
public:
    ARCFaceEngine();
    int InitEngine();
    int UninitialEngine();
    int getFeature(unsigned char* data , int width , int height , MUInt32 format , LPAFR_FSDK_FACEMODEL localmodel);
    int compareFeature(AFR_FSDK_FACEMODEL *model, AFR_FSDK_FACEMODEL *localmodel);
private:
    int getFaceInputImg(unsigned char* data , int width , int height , MUInt32 format, ASVLOFFSCREEN & inputImg );
    MHandle FDEngine;
    MHandle FREngine;
    MByte *pFDWorkBUF;
    MByte *pFRWorkBUF;
    //ARCFaceFeatureDB* pdb;
};

#endif // ARCFACEENGINE_H
