/****************************************************************************
** Meta object code from reading C++ file 'protocolaigenerator.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../app/ui/protocolaigenerator.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'protocolaigenerator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ProtocolAIGenerator_t {
    QByteArrayData data[13];
    char stringdata0[210];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProtocolAIGenerator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProtocolAIGenerator_t qt_meta_stringdata_ProtocolAIGenerator = {
    {
QT_MOC_LITERAL(0, 0, 19), // "ProtocolAIGenerator"
QT_MOC_LITERAL(1, 20, 18), // "generationComplete"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 37), // "CommandSettingsDialog::Protoc..."
QT_MOC_LITERAL(4, 78, 6), // "config"
QT_MOC_LITERAL(5, 85, 16), // "generationFailed"
QT_MOC_LITERAL(6, 102, 12), // "errorMessage"
QT_MOC_LITERAL(7, 115, 14), // "progressUpdate"
QT_MOC_LITERAL(8, 130, 7), // "message"
QT_MOC_LITERAL(9, 138, 22), // "onNetworkReplyFinished"
QT_MOC_LITERAL(10, 161, 14), // "onNetworkError"
QT_MOC_LITERAL(11, 176, 27), // "QNetworkReply::NetworkError"
QT_MOC_LITERAL(12, 204, 5) // "error"

    },
    "ProtocolAIGenerator\0generationComplete\0"
    "\0CommandSettingsDialog::ProtocolConfig\0"
    "config\0generationFailed\0errorMessage\0"
    "progressUpdate\0message\0onNetworkReplyFinished\0"
    "onNetworkError\0QNetworkReply::NetworkError\0"
    "error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProtocolAIGenerator[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       5,    1,   42,    2, 0x06 /* Public */,
       7,    1,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   48,    2, 0x08 /* Private */,
      10,    1,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,

       0        // eod
};

void ProtocolAIGenerator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProtocolAIGenerator *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->generationComplete((*reinterpret_cast< const CommandSettingsDialog::ProtocolConfig(*)>(_a[1]))); break;
        case 1: _t->generationFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->progressUpdate((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->onNetworkReplyFinished(); break;
        case 4: _t->onNetworkError((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply::NetworkError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProtocolAIGenerator::*)(const CommandSettingsDialog::ProtocolConfig & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolAIGenerator::generationComplete)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProtocolAIGenerator::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolAIGenerator::generationFailed)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ProtocolAIGenerator::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolAIGenerator::progressUpdate)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ProtocolAIGenerator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ProtocolAIGenerator.data,
    qt_meta_data_ProtocolAIGenerator,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProtocolAIGenerator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProtocolAIGenerator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProtocolAIGenerator.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ProtocolAIGenerator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ProtocolAIGenerator::generationComplete(const CommandSettingsDialog::ProtocolConfig & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ProtocolAIGenerator::generationFailed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ProtocolAIGenerator::progressUpdate(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
