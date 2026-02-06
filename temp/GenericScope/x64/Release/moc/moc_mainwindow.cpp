/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../app/ui/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[23];
    char stringdata0[421];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 30), // "on_connectToggleButton_toggled"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 7), // "checked"
QT_MOC_LITERAL(4, 51, 34), // "on_portComboBox_currentTextCh..."
QT_MOC_LITERAL(5, 86, 4), // "text"
QT_MOC_LITERAL(6, 91, 38), // "on_baudRateComboBox_currentTe..."
QT_MOC_LITERAL(7, 130, 28), // "on_recordLogCheckBox_toggled"
QT_MOC_LITERAL(8, 159, 25), // "on_settingsButton_clicked"
QT_MOC_LITERAL(9, 185, 25), // "on_darkModeButton_toggled"
QT_MOC_LITERAL(10, 211, 21), // "on_menuButton_clicked"
QT_MOC_LITERAL(11, 233, 23), // "on_filterButton_clicked"
QT_MOC_LITERAL(12, 257, 26), // "onCommandSettingsTriggered"
QT_MOC_LITERAL(13, 284, 17), // "onProtocolChanged"
QT_MOC_LITERAL(14, 302, 4), // "name"
QT_MOC_LITERAL(15, 307, 25), // "onDeviceConnectionChanged"
QT_MOC_LITERAL(16, 333, 9), // "connected"
QT_MOC_LITERAL(17, 343, 20), // "onDeviceDataReceived"
QT_MOC_LITERAL(18, 364, 4), // "data"
QT_MOC_LITERAL(19, 369, 13), // "onDeviceError"
QT_MOC_LITERAL(20, 383, 5), // "error"
QT_MOC_LITERAL(21, 389, 13), // "onUpdateTimer"
QT_MOC_LITERAL(22, 403, 17) // "onDataUpdateTimer"

    },
    "MainWindow\0on_connectToggleButton_toggled\0"
    "\0checked\0on_portComboBox_currentTextChanged\0"
    "text\0on_baudRateComboBox_currentTextChanged\0"
    "on_recordLogCheckBox_toggled\0"
    "on_settingsButton_clicked\0"
    "on_darkModeButton_toggled\0"
    "on_menuButton_clicked\0on_filterButton_clicked\0"
    "onCommandSettingsTriggered\0onProtocolChanged\0"
    "name\0onDeviceConnectionChanged\0connected\0"
    "onDeviceDataReceived\0data\0onDeviceError\0"
    "error\0onUpdateTimer\0onDataUpdateTimer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   89,    2, 0x08 /* Private */,
       4,    1,   92,    2, 0x08 /* Private */,
       6,    1,   95,    2, 0x08 /* Private */,
       7,    1,   98,    2, 0x08 /* Private */,
       8,    0,  101,    2, 0x08 /* Private */,
       9,    1,  102,    2, 0x08 /* Private */,
      10,    0,  105,    2, 0x08 /* Private */,
      11,    0,  106,    2, 0x08 /* Private */,
      12,    0,  107,    2, 0x08 /* Private */,
      13,    1,  108,    2, 0x08 /* Private */,
      15,    1,  111,    2, 0x08 /* Private */,
      17,    1,  114,    2, 0x08 /* Private */,
      19,    1,  117,    2, 0x08 /* Private */,
      21,    0,  120,    2, 0x08 /* Private */,
      22,    0,  121,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::QByteArray,   18,
    QMetaType::Void, QMetaType::QString,   20,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_connectToggleButton_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->on_portComboBox_currentTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->on_baudRateComboBox_currentTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->on_recordLogCheckBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->on_settingsButton_clicked(); break;
        case 5: _t->on_darkModeButton_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_menuButton_clicked(); break;
        case 7: _t->on_filterButton_clicked(); break;
        case 8: _t->onCommandSettingsTriggered(); break;
        case 9: _t->onProtocolChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->onDeviceConnectionChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->onDeviceDataReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 12: _t->onDeviceError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->onUpdateTimer(); break;
        case 14: _t->onDataUpdateTimer(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
