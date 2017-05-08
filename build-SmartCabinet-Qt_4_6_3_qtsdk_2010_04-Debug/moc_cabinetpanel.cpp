/****************************************************************************
** Meta object code from reading C++ file 'cabinetpanel.h'
**
** Created: Mon May 8 11:01:10 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SmartCabinet/Cabinet/cabinetpanel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cabinetpanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CabinetPanel[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      40,   35,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CabinetPanel[] = {
    "CabinetPanel\0\0row\0lattice_inf(int)\0"
    "item\0double_click(QTableWidgetItem*)\0"
};

const QMetaObject CabinetPanel::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CabinetPanel,
      qt_meta_data_CabinetPanel, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CabinetPanel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CabinetPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CabinetPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CabinetPanel))
        return static_cast<void*>(const_cast< CabinetPanel*>(this));
    return QWidget::qt_metacast(_clname);
}

int CabinetPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: lattice_inf((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: double_click((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void CabinetPanel::lattice_inf(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
