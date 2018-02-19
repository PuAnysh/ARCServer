#ifndef ARCFACEFEATUREDB_H
#define ARCFACEFEATUREDB_H

#include <bits/stdc++.h>
#include "ARCFace//arcsoft_fsdk_face_recognition.h"
#include <publicdatastruct.h>
#include <QByteArray>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QVariant>
using namespace std;

class ARCFaceFeatureDB
{
public:

    ARCFaceFeatureDB(QString ip , QString username , QString password , QString database);
    int addFeature_User(const AFR_FSDK_FACEMODEL& model , int id);
    int updateFeature(list<UserModel>* _cache);
    const list<UserModel>* getCache();
private:

    list<UserModel> cache;
    list<GuestModel> cache_guest;
    QSqlDatabase db;
};

#endif // ARCFACEFEATUREDB_H
