/****************************************************************************
** Meta object code from reading C++ file 'datarecorder.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../common_component/record/datarecorder.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'datarecorder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DataRecorder_t {
    QByteArrayData data[10];
    char stringdata0[96];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DataRecorder_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DataRecorder_t qt_meta_stringdata_DataRecorder = {
    {
QT_MOC_LITERAL(0, 0, 12), // "DataRecorder"
QT_MOC_LITERAL(1, 13, 16), // "recordingStarted"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 16), // "recordingStopped"
QT_MOC_LITERAL(4, 48, 12), // "dataRecorded"
QT_MOC_LITERAL(5, 61, 5), // "count"
QT_MOC_LITERAL(6, 67, 12), // "RecordFormat"
QT_MOC_LITERAL(7, 80, 3), // "CSV"
QT_MOC_LITERAL(8, 84, 4), // "JSON"
QT_MOC_LITERAL(9, 89, 6) // "Binary"

    },
    "DataRecorder\0recordingStarted\0\0"
    "recordingStopped\0dataRecorded\0count\0"
    "RecordFormat\0CSV\0JSON\0Binary"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DataRecorder[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       1,   34, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,
       3,    0,   30,    2, 0x06 /* Public */,
       4,    1,   31,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,

 // enums: name, alias, flags, count, data
       6,    6, 0x0,    3,   39,

 // enum data: key, value
       7, uint(DataRecorder::CSV),
       8, uint(DataRecorder::JSON),
       9, uint(DataRecorder::Binary),

       0        // eod
};

void DataRecorder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DataRecorder *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->recordingStarted(); break;
        case 1: _t->recordingStopped(); break;
        case 2: _t->dataRecorded((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DataRecorder::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataRecorder::recordingStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DataRecorder::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataRecorder::recordingStopped)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DataRecorder::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataRecorder::dataRecorded)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DataRecorder::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DataRecorder.data,
    qt_meta_data_DataRecorder,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DataRecorder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataRecorder::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DataRecorder.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DataRecorder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void DataRecorder::recordingStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DataRecorder::recordingStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void DataRecorder::dataRecorded(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
