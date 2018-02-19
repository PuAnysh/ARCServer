#include "arcfacefeaturedb.h"

ARCFaceFeatureDB::ARCFaceFeatureDB(QString ip, QString username , QString password, QString database)
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName(database);
    db.setUserName(username);
    db.setPassword(password);
    /*
     * add useer feature  selected from database
    */
    if(db.open()){
        /*
         * sql query
        */
        printf("open database success\r\n");
        updateFeature(&cache);
    }
    else{
        fprintf(stderr ,"Error on open databse!");
    }
}

int ARCFaceFeatureDB::addFeature_User(const AFR_FSDK_FACEMODEL &model , int id)
{
    QByteArray data((const char*)model.pbFeature, model.lFeatureSize);
    QString sql = "insert into usertable VALUEs()";
    QSqlQuery query = QSqlQuery(db);
    query.prepare("INSERT INTO userfeature (id, featurelen, feature) "
                  "VALUES (?, ?, ?)");
    query.bindValue(0, id);
    query.bindValue(1, model.lFeatureSize);
    query.bindValue(2, data);
    if(query.exec()){
        return 1;
    }
    return 0;
}

int ARCFaceFeatureDB::updateFeature(list<UserModel>* _cache)
{
    QSqlQuery query = QSqlQuery(db);
    QString q = "select * from userfeature;";
    if(!query.exec(q)){
        printf("fail on query.exec() for update Feature cache\r\n");

    }
    else{
        QSqlRecord rec = query.record();
        int id = rec.indexOf("id");
        int len = rec.indexOf("featurelen");
        int feature = rec.indexOf("feature");
        int cnt = 0;
        while (query.next()) {
            cnt++;
            /*
             * doing
            */
            UserModel model;
            model.id = query.value(id).toInt();
            model.model.lFeatureSize = query.value(len).toInt();
            model.model.pbFeature = (MByte*) malloc(model.model.lFeatureSize);
            QByteArray data = query.value(feature).toByteArray();
            memcpy(model.model.pbFeature , data.constData() , model.model.lFeatureSize);
            _cache->push_back(model);
        }
        printf("get %d totally\r\n" , cnt);
        return 0;
    }
    return 1;
}

const list<UserModel>* ARCFaceFeatureDB::getCache()
{
    return &(cache);
}
