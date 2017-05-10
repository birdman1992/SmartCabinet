/****************************************************************************
** Meta object code from reading C++ file 'showinf.h'
**
** Created: Wed May 10 17:30:01 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SmartCabinet/ShowInf/showinf.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'showinf.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ShowInf[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,    9,    8,    8, 0x05,
      33,    8,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      44,    8,    8,    8, 0x0a,
      55,    8,    8,    8, 0x0a,
      67,    8,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ShowInf[] = {
    "ShowInf\0\0med\0cabinet_inf(MedInf)\0"
    "inf_save()\0btn_save()\0btn_close()\0"
    "btn_out()\0"
};

const QMetaObject ShowInf::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ShowInf,
      qt_meta_data_ShowInf, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ShowInf::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ShowInf::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ShowInf::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ShowInf))
        return static_cast<void*>(const_cast< ShowInf*>(this));
    return QWidget::qt_metacast(_clname);
}

int ShowInf::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cabinet_inf((*reinterpret_cast< MedInf(*)>(_a[1]))); break;
        case 1: inf_save(); break;
        case 2: btn_save(); break;
        case 3: btn_close(); break;
        case 4: btn_out(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ShowInf::cabinet_inf(MedInf _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ShowInf::inf_save()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
