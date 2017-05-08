/****************************************************************************
** Meta object code from reading C++ file 'mainwidget.h'
**
** Created: Mon May 8 20:26:00 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SmartCabinet/mainwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWidget[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      22,   11,   11,   11, 0x0a,
      32,   11,   11,   11, 0x0a,
      44,   11,   11,   11, 0x0a,
      55,   11,   11,   11, 0x0a,
      78,   11,   11,   11, 0x0a,
      99,   11,   11,   11, 0x0a,
     121,   11,   11,   11, 0x0a,
     143,   11,   11,   11, 0x0a,
     162,   11,   11,   11, 0x0a,
     185,   11,   11,   11, 0x0a,
     208,   11,   11,   11, 0x0a,
     233,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWidget[] = {
    "MainWidget\0\0btn_one()\0btn_two()\0"
    "btn_three()\0btn_four()\0check_code(QByteArray)\0"
    "read_showinf(MedInf)\0New_Pri_User(UserInf)\0"
    "scan_user(QByteArray)\0cabinet_cleck(int)\0"
    "cabinet_cleck_one(int)\0cabinet_cleck_two(int)\0"
    "cabinet_cleck_three(int)\0"
    "cabinet_cleck_four(int)\0"
};

const QMetaObject MainWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MainWidget,
      qt_meta_data_MainWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWidget))
        return static_cast<void*>(const_cast< MainWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int MainWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: btn_one(); break;
        case 1: btn_two(); break;
        case 2: btn_three(); break;
        case 3: btn_four(); break;
        case 4: check_code((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 5: read_showinf((*reinterpret_cast< MedInf(*)>(_a[1]))); break;
        case 6: New_Pri_User((*reinterpret_cast< UserInf(*)>(_a[1]))); break;
        case 7: scan_user((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 8: cabinet_cleck((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: cabinet_cleck_one((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: cabinet_cleck_two((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: cabinet_cleck_three((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: cabinet_cleck_four((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
