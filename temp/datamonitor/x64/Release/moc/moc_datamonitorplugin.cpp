/****************************************************************************
** Meta object code from reading C++ file 'datamonitorplugin.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../core_plugin/core_plugins/datamonitor/datamonitorplugin.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'datamonitorplugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DataMonitorPlugin_t {
    QByteArrayData data[9];
    char stringdata0[118];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DataMonitorPlugin_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DataMonitorPlugin_t qt_meta_stringdata_DataMonitorPlugin = {
    {
QT_MOC_LITERAL(0, 0, 17), // "DataMonitorPlugin"
QT_MOC_LITERAL(1, 18, 20), // "sendMessageToManager"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 18), // "CorePluginMetaData"
QT_MOC_LITERAL(4, 59, 4), // "data"
QT_MOC_LITERAL(5, 64, 17), // "sendMessageToMain"
QT_MOC_LITERAL(6, 82, 19), // "onThresholdExceeded"
QT_MOC_LITERAL(7, 102, 9), // "fieldName"
QT_MOC_LITERAL(8, 112, 5) // "value"

    },
    "DataMonitorPlugin\0sendMessageToManager\0"
    "\0CorePluginMetaData\0data\0sendMessageToMain\0"
    "onThresholdExceeded\0fieldName\0value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DataMonitorPlugin[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       5,    1,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    2,   35,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Double,    7,    8,

       0        // eod
};

void DataMonitorPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DataMonitorPlugin *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendMessageToManager((*reinterpret_cast< const CorePluginMetaData(*)>(_a[1]))); break;
        case 1: _t->sendMessageToMain((*reinterpret_cast< const CorePluginMetaData(*)>(_a[1]))); break;
        case 2: _t->onThresholdExceeded((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CorePluginMetaData >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CorePluginMetaData >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DataMonitorPlugin::*)(const CorePluginMetaData & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataMonitorPlugin::sendMessageToManager)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DataMonitorPlugin::*)(const CorePluginMetaData & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataMonitorPlugin::sendMessageToMain)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DataMonitorPlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DataMonitorPlugin.data,
    qt_meta_data_DataMonitorPlugin,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DataMonitorPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataMonitorPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DataMonitorPlugin.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "CorePluginsBase"))
        return static_cast< CorePluginsBase*>(this);
    if (!strcmp(_clname, "com.genericscope.CorePluginsBase"))
        return static_cast< CorePluginsBase*>(this);
    return QObject::qt_metacast(_clname);
}

int DataMonitorPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void DataMonitorPlugin::sendMessageToManager(const CorePluginMetaData & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DataMonitorPlugin::sendMessageToMain(const CorePluginMetaData & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

QT_PLUGIN_METADATA_SECTION
static constexpr unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', '!',
    // metadata version, Qt version, architectural requirements
    0, QT_VERSION_MAJOR, QT_VERSION_MINOR, qPluginArchRequirements(),
    0xbf, 
    // "IID"
    0x02,  0x78,  0x23,  'c',  'o',  'm',  '.',  'g', 
    'e',  'n',  'e',  'r',  'i',  'c',  's',  'c', 
    'o',  'p',  'e',  '.',  'p',  'l',  'u',  'g', 
    'i',  'n',  '.',  'd',  'a',  't',  'a',  'm', 
    'o',  'n',  'i',  't',  'o',  'r', 
    // "className"
    0x03,  0x71,  'D',  'a',  't',  'a',  'M',  'o', 
    'n',  'i',  't',  'o',  'r',  'P',  'l',  'u', 
    'g',  'i',  'n', 
    // "MetaData"
    0x04,  0xa7,  0x66,  'a',  'u',  't',  'h',  'o', 
    'r',  0x71,  'G',  'e',  'n',  'e',  'r',  'i', 
    'c',  'S',  'c',  'o',  'p',  'e',  ' ',  'T', 
    'e',  'a',  'm',  0x68,  'c',  'a',  't',  'e', 
    'g',  'o',  'r',  'y',  0x6a,  'm',  'o',  'n', 
    'i',  't',  'o',  'r',  'i',  'n',  'g',  0x6c, 
    'd',  'e',  'p',  'e',  'n',  'd',  'e',  'n', 
    'c',  'i',  'e',  's',  0x80,  0x6b,  'd',  'e', 
    's',  'c',  'r',  'i',  'p',  't',  'i',  'o', 
    'n',  0x78,  0x39,  uchar('\xe5'), uchar('\xae'), uchar('\x9e'), uchar('\xe6'), uchar('\x97'),
    uchar('\xb6'), uchar('\xe7'), uchar('\x9b'), uchar('\x91'), uchar('\xe6'), uchar('\x8e'), uchar('\xa7'), uchar('\xe4'),
    uchar('\xbc'), uchar('\xa0'), uchar('\xe6'), uchar('\x84'), uchar('\x9f'), uchar('\xe5'), uchar('\x99'), uchar('\xa8'),
    uchar('\xe6'), uchar('\x95'), uchar('\xb0'), uchar('\xe6'), uchar('\x8d'), uchar('\xae'), uchar('\xef'), uchar('\xbc'),
    uchar('\x8c'), uchar('\xe8'), uchar('\xb6'), uchar('\x85'), uchar('\xe8'), uchar('\xbf'), uchar('\x87'), uchar('\xe9'),
    uchar('\x98'), uchar('\x88'), uchar('\xe5'), uchar('\x80'), uchar('\xbc'), uchar('\xe6'), uchar('\x97'), uchar('\xb6'),
    uchar('\xe5'), uchar('\x8f'), uchar('\x91'), uchar('\xe5'), uchar('\x87'), uchar('\xba'), uchar('\xe8'), uchar('\xad'),
    uchar('\xa6'), uchar('\xe6'), uchar('\x8a'), uchar('\xa5'), 0x62,  'i',  'd',  0x78, 
    0x23,  'c',  'o',  'm',  '.',  'g',  'e',  'n', 
    'e',  'r',  'i',  'c',  's',  'c',  'o',  'p', 
    'e',  '.',  'p',  'l',  'u',  'g',  'i',  'n', 
    '.',  'd',  'a',  't',  'a',  'm',  'o',  'n', 
    'i',  't',  'o',  'r',  0x64,  'n',  'a',  'm', 
    'e',  0x72,  uchar('\xe6'), uchar('\x95'), uchar('\xb0'), uchar('\xe6'), uchar('\x8d'), uchar('\xae'),
    uchar('\xe7'), uchar('\x9b'), uchar('\x91'), uchar('\xe6'), uchar('\x8e'), uchar('\xa7'), uchar('\xe6'), uchar('\x8f'),
    uchar('\x92'), uchar('\xe4'), uchar('\xbb'), uchar('\xb6'), 0x67,  'v',  'e',  'r', 
    's',  'i',  'o',  'n',  0x65,  '1',  '.',  '0', 
    '.',  '0', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN(DataMonitorPlugin, DataMonitorPlugin)

QT_WARNING_POP
QT_END_MOC_NAMESPACE
