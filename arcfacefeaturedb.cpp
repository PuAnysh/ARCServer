#include "arcfacefeaturedb.h"

ARCFaceFeatureDB::ARCFaceFeatureDB(QString ip, QString username , QString password, QString database)
{
    _ip = ip;
    _username = username;
    _password = password;
    _database = database;
    openDataBase(db);
    updateFeature(&cache);
}

int ARCFaceFeatureDB::addFeature_User(const AFR_FSDK_FACEMODEL &model , int id)
{
    //QSqlDatabase db;
    //openDataBase(db);
    QByteArray data((const char*)model.pbFeature, model.lFeatureSize);
    QSqlQuery query = QSqlQuery(db);
    query.prepare("update UserInformation set LenOfFeature = ?, UserFeature = ? where UserID = " + QString::number(id) + ";");
    //qDebug()<<("INSERT INTO UserInformation (LenOfFeature, UserFeature) VALUES (?, ?) where UserID = " + QString::number(id));

    query.bindValue(0, model.lFeatureSize);
    query.bindValue(1, data);
    if(query.exec()){
        return 0;
    }
    fprintf(stderr , "Error on insert data\r\n");
    qDebug()<<query.lastError();
    return 1;
}

int ARCFaceFeatureDB::addFeature_Guest(const AFR_FSDK_FACEMODEL &model, int id)
{
    GuestModel guest;
    guest.UserID = id;
    guest.model = model;
    guest.addTime = QTime::currentTime();
    lock_guest.lockForWrite();
    cache_guest.push_back(guest);
    fprintf(stderr ,"size is %d" , cache_guest.size());
    lock_guest.unlock();
    return 0;
}

int ARCFaceFeatureDB::updateFeature(list<UserModel>* _cache)
{
    //QSqlDatabase db;
    //openDataBase(db);
    QSqlQuery query = QSqlQuery(db);
    QString q = "select UserID, lenOFfeature,UserFeature from UserInformation;";
    if(!query.exec(q)){
        printf("fail on query.exec() for update Feature cache\r\n");

    }
    else{
        QSqlRecord rec = query.record();
        int id = rec.indexOf("UserID");
        int len = rec.indexOf("lenOFfeature");
        int feature = rec.indexOf("UserFeature");
        int cnt = 0;
        while (query.next()) {
            /*
             * doing
            */
            UserModel model;
            model.model.lFeatureSize = query.value(len).toInt();
            if(model.model.lFeatureSize == 0){
                continue;
            }
            model.id = query.value(id).toInt();
            model.model.pbFeature = (MByte*) malloc(model.model.lFeatureSize);
            QByteArray data = query.value(feature).toByteArray();
            memcpy(model.model.pbFeature , data.constData() , model.model.lFeatureSize);
            cnt++;
            _cache->push_back(model);
        }
        db.close();
        printf("get %d totally\r\n" , cnt);
        return 0;
    }
    return 1;
}

const list<UserModel>* ARCFaceFeatureDB::getCache()
{
    lock_user.lockForRead();
    return &(cache);
}

const list<GuestModel> *ARCFaceFeatureDB::getCache_guest()
{
    lock_guest.lockForRead();
    return &(cache_guest);
}

void ARCFaceFeatureDB::freeGuestLock()
{
    lock_guest.unlock();
}

void ARCFaceFeatureDB::freeUserLock()
{
    lock_user.unlock();
}

void ARCFaceFeatureDB::queryADDonDB(int id, int type, string ip)
{
    QSqlQuery query = QSqlQuery(db);
    query.prepare("insert into UserImport(UserID , ImportTime , GateIP , UserType) values(? , now() , ? , ?);");
    query.bindValue(0, id);
    query.bindValue(1 , ip.c_str());
    query.bindValue(2 , type);
    query.exec();
}

void ARCFaceFeatureDB::openDataBase(QSqlDatabase db)
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(_ip);
    db.setDatabaseName(_database);
    db.setUserName(_username);
    db.setPassword(_password);
    db.setPort(3306);
    /*
     * add useer feature  selected from database
    */
    if(db.open()){
        /*
         * sql query
        */
        printf("open database success\r\n");
        //updateFeature(&cache);
    }
    else{
        fprintf(stderr ,"Error on open databse!");
    }
}
