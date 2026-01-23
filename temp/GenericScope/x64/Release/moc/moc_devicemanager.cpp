/****************************************************************************
** Meta object code from reading C++ file 'devicemanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../app/device/devicemanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'devicemanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DeviceManager_t {
    QByteArrayData data[14];
    char stringdata0[193];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DeviceManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DeviceManager_t qt_meta_stringdata_DeviceManager = {
    {
QT_MOC_LITERAL(0, 0, 13), // "DeviceManager"
QT_MOC_LITERAL(1, 14, 17), // "connectionChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 9), // "connected"
QT_MOC_LITERAL(4, 43, 12), // "dataReceived"
QT_MOC_LITERAL(5, 56, 4), // "data"
QT_MOC_LITERAL(6, 61, 13), // "errorOccurred"
QT_MOC_LITERAL(7, 75, 5), // "error"
QT_MOC_LITERAL(8, 81, 23), // "onTransferStatusChanged"
QT_MOC_LITERAL(9, 105, 19), // "ScopeTransferStatus"
QT_MOC_LITERAL(10, 125, 6), // "status"
QT_MOC_LITERAL(11, 132, 22), // "onTransferDataReceived"
QT_MOC_LITERAL(12, 155, 15), // "onTransferError"
QT_MOC_LITERAL(13, 171, 21) // "onPollingDataReceived"

    },
    "DeviceManager\0connectionChanged\0\0"
    "connected\0dataReceived\0data\0errorOccurred\0"
    "error\0onTransferStatusChanged\0"
    "ScopeTransferStatus\0status\0"
    "onTransferDataReceived\0onTransferError\0"
    "onPollingDataReceived"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DeviceManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    1,   52,    2, 0x06 /* Public */,
       6,    1,   55,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   58,    2, 0x08 /* Private */,
      11,    1,   61,    2, 0x08 /* Private */,
      12,    1,   64,    2, 0x08 /* Private */,
      13,    1,   67,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QByteArray,    5,
    QMetaType::Void, QMetaType::QString,    7,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, QMetaType::QByteArray,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QByteArray,    5,

       0        // eod
};

void DeviceManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DeviceManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connectionChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->dataReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 2: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->onTransferStatusChanged((*reinterpret_cast< ScopeTransferStatus(*)>(_a[1]))); break;
        case 4: _t->onTransferDataReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 5: _t->onTransferError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->onPollingDataReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DeviceManager::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DeviceManager::connectionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DeviceManager::*)(const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DeviceManager::dataReceived)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DeviceManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DeviceManager::errorOccurred)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DeviceManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DeviceManager.data,
    qt_meta_data_DeviceManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DeviceManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DeviceManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DeviceManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void DeviceManager::connectionChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceManager::dataReceived(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DeviceManager::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
