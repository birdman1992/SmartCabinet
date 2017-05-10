/****************************************************************************
** Meta object code from reading C++ file 'cabinetset.h'
**
** Created: Wed May 10 17:29:59 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SmartCabinet/Widgets/cabinetset.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cabinetset.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CabinetSet[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   12,   11,   11, 0x05,
      33,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      56,   11,   11,   11, 0x08,
      78,   11,   11,   11, 0x08,
     101,   11,   11,   11, 0x08,
     120,   11,   11,   11, 0x08,
     138,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CabinetSet[] = {
    "CabinetSet\0\0index\0winSwitch(int)\0"
    "setCabinet(QByteArray)\0on_add_left_clicked()\0"
    "on_add_right_clicked()\0on_clear_clicked()\0"
    "on_save_clicked()\0on_cancel_clicked()\0"
};

const QMetaObject CabinetSet::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CabinetSet,
      qt_meta_data_CabinetSet, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CabinetSet::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CabinetSet::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CabinetSet::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CabinetSet))
        return static_cast<void*>(const_cast< CabinetSet*>(this));
    return QWidget::qt_metacast(_clname);
}

int CabinetSet::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: winSwitch((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: setCabinet((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 2: on_add_left_clicked(); break;
        case 3: on_add_right_clicked(); break;
        case 4: on_clear_clicked(); break;
        case 5: on_save_clicked(); break;
        case 6: on_cancel_clicked(); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void CabinetSet::winSwitch(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CabinetSet::setCabinet(QByteArray _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
