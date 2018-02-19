#include "arcfaceengines.h"
QSemaphore ARCFaceEngines::sem(10);
ARCFaceEngines::ARCFaceEngines()
{
    for(int i = 0 ; i < CMAX_NUM ; i++){
        ARCFaceEngine* engine = new ARCFaceEngine();
        engines.push_back(engine);
    }
    db = new ARCFaceFeatureDB("localhost" , "root" , "" ,"test");
}
/*
 * input: no
 * output:
 * return 0 : start all engines successed
 * else     : 1start all engines failed
*/
int ARCFaceEngines::startEngines()
{
    int res = 0;
    for(int i = 0 ; i < CMAX_NUM ; i++){
        res |= engines[i]->InitEngine();
        Enginespoll.push(i);
    }
    return res;
}

/*
 * input : no
 * output:
 * return 0 : stop all engines successed
 * else     : failed
*/

int ARCFaceEngines::stopEngines()
{
    int res = 0;
    for(int i = 0 ; i < CMAX_NUM ; i++){
        res |= engines[i]->UninitialEngine();
    }
    return res;
}

/*
 * input :
 * data     the jpg image data
 * width    the width of input image
 * heigit   the height of input image
 * ldate    the length of input data
 * output:
 * return 0 : match success
 * return 1 : match fail
*/

int ARCFaceEngines::compareFace(char *data , int ldata)
{
    int curEngineIndex = popEngineIndex();
    ARCFaceEngine* & CurEngine =  engines[curEngineIndex];
    QImage img;
    img.loadFromData(QByteArray(data , ldata) , "JPG");
    bool load_ok = img.loadFromData(QByteArray(data , ldata) , "JPG");

    if(!load_ok){
        printf("CurEngine is %d\r\n" , curEngineIndex);
        pushEngineIndex(curEngineIndex);
        //printf("load the image\r\n");
        return 1;
    }
    int size = img.byteCount()/4*3;
    uchar* rawdata = img.bits();
    uchar* bmpdata = (uchar*)malloc(size);
    int ptr_bmp = 0;
    for(int i = 0 ; i < size ; i++){
        if((i+1)%4 == 0) continue;
        bmpdata[ptr_bmp] = rawdata[i];
    }
    AFR_FSDK_FACEMODEL localmodel;
    memset(&localmodel , 0 , sizeof(AFR_FSDK_FACEMODEL));
    CurEngine->getFeature(bmpdata , img.width() , img.height() , ASVL_PAF_RGB24_B8G8R8, & localmodel);
    const list<UserModel>* userlist =  db->getCache();
    /*
     * compare face feature in our database , ret == 1 indicated that we find some feature match this feature
    */
    int ret = 0;
    for(list<UserModel>::const_iterator it = userlist->begin() ; it != userlist->end() ; it++){
        ret = CurEngine->compareFeature(const_cast<AFR_FSDK_FACEMODEL*>(&(it->model)) , &localmodel);
        if(ret == 1){
            break;
        }
    }
    pushEngineIndex(curEngineIndex);
    return ret == 1? 0 : 1;
}

int ARCFaceEngines::addFeature(char *data, int ldata , int id)
{
    int curEngineIndex = popEngineIndex();
    ARCFaceEngine* & CurEngine =  engines[curEngineIndex];
    QImage img;
    bool load_ok = img.loadFromData(QByteArray(data , ldata) , "JPG");
    if(!load_ok){
        pushEngineIndex(curEngineIndex);
        return 1;
    }
    int size = img.byteCount()/4*3;
    uchar* rawdata = img.bits();
    uchar* bmpdata = (uchar*)malloc(size);
    int ptr_bmp = 0;
    for(int i = 0 ; i < size ; i++){
        if((i+1)%4 == 0) continue;
        bmpdata[ptr_bmp] = rawdata[i];
    }
    AFR_FSDK_FACEMODEL localmodel;
    memset(&localmodel , 0 , sizeof(AFR_FSDK_FACEMODEL));
    int FRret = CurEngine->getFeature(bmpdata , img.width() , img.height() , ASVL_PAF_RGB24_B8G8R8, & localmodel);
    if(FRret == MOK){
        db->addFeature_User(localmodel , id);
    }
    else{
        return 1;
    }
    pushEngineIndex(curEngineIndex);
    return 0;
}

int ARCFaceEngines::popEngineIndex()
{
    sem.acquire(1);
    mutex.lock();
    int curEngineIndex = Enginespoll.top();
    Enginespoll.pop();
    mutex.unlock();
    return curEngineIndex;
}

void ARCFaceEngines::pushEngineIndex(int curEngineIndex)
{
    mutex.lock();
    Enginespoll.push(curEngineIndex);
    mutex.unlock();
    sem.release(1);
}
