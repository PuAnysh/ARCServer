#include "arcfaceengines.h"
QSemaphore ARCFaceEngines::sem(10);
ARCFaceEngines::ARCFaceEngines()
{
    for(int i = 0 ; i < CMAX_NUM ; i++){
        ARCFaceEngine* engine = new ARCFaceEngine();
        engines.push_back(engine);
    }
    db = new ARCFaceFeatureDB("111.230.236.192" , "root" , "123456" ,"ARCServer");
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

int ARCFaceEngines::compareFace(char *data , int ldata, string ip)
{
    qDebug()<<"compare face";
    int curEngineIndex = popEngineIndex();
    ARCFaceEngine* & CurEngine =  engines[curEngineIndex];
    QImage img;
    bool load_ok = img.loadFromData((uchar*)data , ldata);
    if(!load_ok){
        fprintf(stderr ,"fial on load image\r\n" );
        pushEngineIndex(curEngineIndex);
        return 1;
    }
    int size = img.byteCount()/4*3;
    uchar* rawdata = img.bits();
    uchar* bmpdata = (uchar*)malloc(size);
    int ptr_bmp = 0;
    for(int i = 0 ; i < size ; i++){
        if((i+1)%4 == 0) continue;
        bmpdata[ptr_bmp++] = rawdata[i];
    }
    AFR_FSDK_FACEMODEL localmodel;
    memset(&localmodel , 0 , sizeof(AFR_FSDK_FACEMODEL));
    CurEngine->getFeature(bmpdata , img.width() , img.height() , ASVL_PAF_RGB24_B8G8R8, & localmodel);
    const list<UserModel>* userlist =  db->getCache();
    /*
     * compare face feature in user database , ret == 1 indicated that we find some feature match this feature
    */
    int User_ret = 0,Guest_ret = 0;
    for(list<UserModel>::const_iterator it = userlist->begin() ; it != userlist->end() ; it++){
        User_ret = CurEngine->compareFeature(const_cast<AFR_FSDK_FACEMODEL*>(&(it->model)) , &localmodel);
        if(User_ret == 1){
            db->queryADDonDB(it->id , 1 , ip);
            break;
        }
    }
    db->freeUserLock();
    /*
     * compare face feature in guest database
    */
    if(User_ret != 1){
        const list<GuestModel>* guestlist =  db->getCache_guest();
        fprintf(stderr , "size is  %d\r\n" , guestlist->size());
        for(list<GuestModel>::const_iterator it = guestlist->begin() ; it != guestlist->end() ; it++){
            Guest_ret = CurEngine->compareFeature(const_cast<AFR_FSDK_FACEMODEL*>(&(it->model)) , &localmodel);
            if(Guest_ret == 1){
                db->queryADDonDB(it->UserID , 2 , ip);
                break;
            }
        }
        db->freeGuestLock();
    }
    pushEngineIndex(curEngineIndex);
    return User_ret|Guest_ret == 1? 0 : 1;
}

int ARCFaceEngines::addFeature(char *data, int ldata , int id)
{
    int curEngineIndex = popEngineIndex();
    ARCFaceEngine* & CurEngine =  engines[curEngineIndex];
    QImage img;
    bool load_ok = img.loadFromData((uchar*)data , ldata);
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
        bmpdata[ptr_bmp++] = rawdata[i];
    }
    AFR_FSDK_FACEMODEL localmodel;
    memset(&localmodel , 0 , sizeof(AFR_FSDK_FACEMODEL));
    int FRret = CurEngine->getFeature(bmpdata , img.width() , img.height() , ASVL_PAF_RGB24_B8G8R8, & localmodel);
    pushEngineIndex(curEngineIndex);
    if(FRret == MOK){
        db->addFeature_User(localmodel , id);
        return 0;
    }
    else{
        return 1;
    }
    return 0;
}

int ARCFaceEngines::addFeature_g(char *data, int ldata, int id)
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
        bmpdata[ptr_bmp++] = rawdata[i];
    }
    AFR_FSDK_FACEMODEL localmodel;
    memset(&localmodel , 0 , sizeof(AFR_FSDK_FACEMODEL));
    int FRret = CurEngine->getFeature(bmpdata , img.width() , img.height() , ASVL_PAF_RGB24_B8G8R8, & localmodel);
    if(FRret == MOK){
        //db->addFeature_User(localmodel , id);
        fprintf(stderr ," fdsfsdafsdafsdf\r\n");
        db->addFeature_Guest(localmodel , id);
    }
    else{
        pushEngineIndex(curEngineIndex);
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
