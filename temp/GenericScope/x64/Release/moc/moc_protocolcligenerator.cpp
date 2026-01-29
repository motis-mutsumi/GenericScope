/****************************************************************************
** Meta object code from reading C++ file 'protocolcligenerator.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../app/ui/protocolcligenerator.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'protocolcligenerator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ProtocolCLIGenerator_t {
    QByteArrayData data[18];
    char stringdata0[292];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProtocolCLIGenerator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProtocolCLIGenerator_t qt_meta_stringdata_ProtocolCLIGenerator = {
    {
QT_MOC_LITERAL(0, 0, 20), // "ProtocolCLIGenerator"
QT_MOC_LITERAL(1, 21, 18), // "generationComplete"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 37), // "CommandSettingsDialog::Protoc..."
QT_MOC_LITERAL(4, 79, 6), // "config"
QT_MOC_LITERAL(5, 86, 16), // "generationFailed"
QT_MOC_LITERAL(6, 103, 12), // "errorMessage"
QT_MOC_LITERAL(7, 116, 14), // "progressUpdate"
QT_MOC_LITERAL(8, 131, 7), // "message"
QT_MOC_LITERAL(9, 139, 17), // "onProcessFinished"
QT_MOC_LITERAL(10, 157, 8), // "exitCode"
QT_MOC_LITERAL(11, 166, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(12, 187, 10), // "exitStatus"
QT_MOC_LITERAL(13, 198, 14), // "onProcessError"
QT_MOC_LITERAL(14, 213, 22), // "QProcess::ProcessError"
QT_MOC_LITERAL(15, 236, 5), // "error"
QT_MOC_LITERAL(16, 242, 24), // "onProcessReadyReadStdOut"
QT_MOC_LITERAL(17, 267, 24) // "onProcessReadyReadStdErr"

    },
    "ProtocolCLIGenerator\0generationComplete\0"
    "\0CommandSettingsDialog::ProtocolConfig\0"
    "config\0generationFailed\0errorMessage\0"
    "progressUpdate\0message\0onProcessFinished\0"
    "exitCode\0QProcess::ExitStatus\0exitStatus\0"
    "onProcessError\0QProcess::ProcessError\0"
    "error\0onProcessReadyReadStdOut\0"
    "onProcessReadyReadStdErr"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProtocolCLIGenerator[] = {

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
       5,    1,   52,    2, 0x06 /* Public */,
       7,    1,   55,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    2,   58,    2, 0x08 /* Private */,
      13,    1,   63,    2, 0x08 /* Private */,
      16,    0,   66,    2, 0x08 /* Private */,
      17,    0,   67,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    8,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 11,   10,   12,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ProtocolCLIGenerator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProtocolCLIGenerator *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->generationComplete((*reinterpret_cast< const CommandSettingsDialog::ProtocolConfig(*)>(_a[1]))); break;
        case 1: _t->generationFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->progressUpdate((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->onProcessFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 4: _t->onProcessError((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 5: _t->onProcessReadyReadStdOut(); break;
        case 6: _t->onProcessReadyReadStdErr(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProtocolCLIGenerator::*)(const CommandSettingsDialog::ProtocolConfig & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolCLIGenerator::generationComplete)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProtocolCLIGenerator::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolCLIGenerator::generationFailed)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ProtocolCLIGenerator::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolCLIGenerator::progressUpdate)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ProtocolCLIGenerator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ProtocolCLIGenerator.data,
    qt_meta_data_ProtocolCLIGenerator,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProtocolCLIGenerator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProtocolCLIGenerator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProtocolCLIGenerator.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ProtocolCLIGenerator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ProtocolCLIGenerator::generationComplete(const CommandSettingsDialog::ProtocolConfig & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ProtocolCLIGenerator::generationFailed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ProtocolCLIGenerator::progressUpdate(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
