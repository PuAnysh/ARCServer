#ifndef ARCFACEFEATUREDB_H
#define ARCFACEFEATUREDB_H

#include <bits/stdc++.h>
#include "ARCFace//arcsoft_fsdk_face_recognition.h"
#include <publicdatastruct.h>
#include <QByteArray>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QVariant>
#include <QTime>
#include <QReadWriteLock>
#include <QDebug>
using namespace std;

class ARCFaceFeatureDB
{
public:

    ARCFaceFeatureDB(QString ip , QString username , QString password , QString database);
    int addFeature_User(const AFR_FSDK_FACEMODEL& model , int id);
    int addFeature_Guest(const AFR_FSDK_FACEMODEL &model, int id);
    int updateFeature(list<UserModel>* _cache);
    const list<UserModel>* getCache();
    const list<GuestModel>* getCache_guest();
    void freeGuestLock();
    void freeUserLock();
    void queryADDonDB(int id , int type , string ip);
private:
    QString _ip;
    QString _username;
    QString _password;
    QString _database;
    void openDataBase(QSqlDatabase db);
    list<UserModel> cache;
    list<GuestModel> cache_guest;
    QReadWriteLock lock_guest,lock_user;
    QSqlDatabase db;
};

#endif // ARCFACEFEATUREDB_H
