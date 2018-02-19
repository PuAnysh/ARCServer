#ifndef ARCFACEENGINES_H
#define ARCFACEENGINES_H
#include <bits/stdc++.h>
#include <arcfaceengine.h>
#include <arcfacefeaturedb.h>
#include <publicdatastruct.h>
#include <QSemaphore>
#include <QMutex>
#include <QImage>
#define CMAX_NUM 10


using namespace  std;
class ARCFaceEngines
{
public:
    ARCFaceEngines();
    int startEngines();
    int stopEngines();
    int compareFace(char* data , int ldata);
    int addFeature(char* data , int ldata, int id);
private:
    static QSemaphore sem;
    int popEngineIndex();
    void pushEngineIndex(int curEngineIndex);
    QMutex mutex;
    vector<ARCFaceEngine*> engines;
    stack<int> Enginespoll;
    ARCFaceFeatureDB* db;
};

#endif // ARCFACEENGINES_H
